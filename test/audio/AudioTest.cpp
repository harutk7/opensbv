//
// Created by harut on 9/7/17.
//

#include "AudioTest.h"

AudioTest::AudioTest() : m_sampleRate(8000), m_releaseSize(1024), m_maxReleaseSize(1024), AudioCAP() {

    SetSampleRate(m_sampleRate);
    SetReleaseSize(m_releaseSize);
    SetMaxReleaseSize(m_maxReleaseSize);

    m_data = (unsigned char *)malloc(100000);

    std::vector<std::string> list = GetDeviceList();

    SetDevice(list.front());

    // Print the list of capture devices
    printf("Available playback devices:\n");

    devices = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
    ptr = devices;
//while (ptr[0] != NULL)
    while (*ptr)
    {
        printf("   %s\n", ptr);
        ptr += strlen(ptr) + 1;
    }

// Open a playback device and create a context first
    printf("Opening playback device:\n");
    m_mainDev = alcOpenDevice(NULL);
    if (m_mainDev == NULL)
    {
        printf("Unable to open playback device!\n");
        exit(1);
    }
    devices = alcGetString(m_mainDev, ALC_DEVICE_SPECIFIER);
    printf("   opened device '%s'\n", devices);
    m_mainContext = alcCreateContext(m_mainDev, NULL);
    if (m_mainContext == NULL)
    {
        printf("Unable to create playback context!\n");
        exit(1);
    }
    printf("   created playback context\n");

// Make the playback context current
    alcMakeContextCurrent(m_mainContext);
    alcProcessContext(m_mainContext);

    alGenBuffers(NUM_BUFFERS, m_buffer);
    alGenSources(1, &m_source);

//    m_playParams.buffer = &buffer;
//    m_playParams.source = &source;
//    m_playParams.m_mtx = &m_mtx;

//    pthread_create(&this->m_thread,NULL,&playerWorker,&m_playParams);
}

void *playerWorker(void* ptr) {
    audioParams params = *(audioParams *)ptr;

    ALuint uiBuffer;
    ALint iBuffersProcessed,iTotalBuffersProcessed, val;
    iBuffersProcessed = 0;
    while (*params.m_running)
    {
        usleep(10 * 1000); // Sleep 10 msec periodically

        alGetSourcei(*params.m_source, AL_BUFFERS_PROCESSED, &iBuffersProcessed);

        iTotalBuffersProcessed += iBuffersProcessed;

        // For each processed buffer, remove it from the source queue, read the next chunk of
        // audio data from the file, fill the buffer with new data, and add it to the source queue
        while (iBuffersProcessed)
        {
            // Remove the buffer from the queue (uiBuffer contains the buffer ID for the dequeued buffer)
            uiBuffer = 0;
            alSourceUnqueueBuffers(*params.m_source, 1, &uiBuffer);

            // Read more pData audio data (if there is any)
            pthread_mutex_lock(params.m_mtx);
            // Copy audio data to buffer
            alBufferData(uiBuffer, AL_FORMAT_MONO16, params.m_data, *params.m_data_size, *params.m_sampleRate);
            // Insert the audio buffer to the source queue
            alSourceQueueBuffers(*params.m_source, 1, &uiBuffer);
            pthread_mutex_unlock(params.m_mtx);

            alGetSourcei(*params.m_source, AL_SOURCE_STATE, &val);
            if (val != AL_PLAYING)
                alSourcePlay(*params.m_source);

            iBuffersProcessed--;
        }
    }

    while(*params.m_running) {
        ALuint buffer;
        ALint val;

        alGetSourcei(*params.m_source, AL_BUFFERS_PROCESSED, &val);
        while (val <= 0) {
            usleep(10000);
            alGetSourcei(*params.m_source, AL_BUFFERS_PROCESSED, &val);
        }

//        if (fi.loop_count > 0)
//            break;

        alSourceUnqueueBuffers(*params.m_source, 1, &buffer);
        pthread_mutex_lock(params.m_mtx);
        alBufferData(buffer, AL_FORMAT_MONO16, params.m_data,
                     *params.m_data_size, *params.m_sampleRate);
        alSourceQueueBuffers(*params.m_source, 1, &buffer);
        if (alGetError() != AL_NO_ERROR)
            break;
        pthread_mutex_unlock(params.m_mtx);

        alGetSourcei(*params.m_source, AL_SOURCE_STATE, &val);
        if (val != AL_PLAYING) {
            alSourcePlay(*params.m_source);
        }

    }
}

AudioTest::~AudioTest() {
    free(m_data);
}

void AudioTest::OnSampleReady(const void *p, size_t size) {
//    m_streamer->Write((unsigned char *)p, size);
//
//    return;
    if (m_first) {
        for (int i = 0; i < NUM_BUFFERS; i++) {
            alBufferData(m_buffer[i], AL_FORMAT_MONO16, p,
                         size, m_sampleRate);
        }

        alSourceQueueBuffers(m_source, NUM_BUFFERS, m_buffer);
        alSourcePlay(m_source);
        if (alGetError() != AL_NO_ERROR) {
            fprintf(stderr, "Failed miserably\n");
            exit(1);
        }
        m_first = false;


        m_playParams.m_buffer = m_buffer;
        m_playParams.m_source = &m_source;
        m_playParams.m_mtx = &m_mtx;
        m_playParams.m_running = &m_running;
        m_playParams.m_data = m_data;
        m_playParams.m_data_size = &m_data_size;
        m_playParams.m_sampleRate = &m_sampleRate;

        pthread_create(&this->m_thread,NULL,&playerWorker,&m_playParams);
    } else {
        pthread_mutex_lock(&m_mtx);
        memcpy(m_data, (unsigned char *)p, size);
        m_data_size = size;
        pthread_mutex_unlock(&m_mtx);
    }

    m_streamer->Write((unsigned char *)p, size);
}

void AudioTest::Run() {

    StartCapture();
}

void AudioTest::Stop() {
    StopCapture();
}

void AudioTest::SetStreamer(Streamer *stream) {
    m_streamer = stream;
}