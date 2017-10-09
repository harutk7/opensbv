//
// Created by harut on 9/7/17.
//

#include "opensbv/audio/AudioCAP.h"

AudioCAP::AudioCAP(ushort bufferSize) {
    m_data_buffer_size = bufferSize;
}

AudioCAP::AudioCAP() {
    m_data_buffer_size = 2000;
}

AudioCAP::~AudioCAP() {
    delete m_captureDev;
    delete m_mainContext;
    delete m_mainDev;
}

void AudioCAP::StartCapture() {

//    printf("Available playback devices:\n");

    m_devices = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
    m_ptr = m_devices;
//    while (*m_ptr)
//    {
//        printf("   %s\n", m_ptr);
//        m_ptr += strlen(m_ptr) + 1;
//    }

//    printf("Opening playback device:\n");
    m_mainDev = alcOpenDevice(NULL);
    if (m_mainDev == NULL)
    {
        printf("Unable to open playback device!\n");
        return;
    }
    m_devices = alcGetString(m_mainDev, ALC_DEVICE_SPECIFIER);

//    printf("   opened device '%s'\n", m_devices);
    m_mainContext = alcCreateContext(m_mainDev, NULL);
    if (m_mainContext == NULL)
    {
        printf("Unable to create playback context!\n");
        return;
    }
//    printf("   created playback context\n");

// Make the playback context current
    alcMakeContextCurrent(m_mainContext);
    alcProcessContext(m_mainContext);

// Print the list of capture devices

//    printf("Available capture devices:\n");
    m_devices = alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER);
    m_ptr = m_devices;
//    while (*m_ptr)
//    {
//        printf("   %s\n", m_ptr);
//        m_ptr += strlen(m_ptr) + 1;
//    }

//    printf("Opening capture device:\n");
    m_captureDev = alcCaptureOpenDevice(NULL, 8000, AL_FORMAT_MONO16, 800);
    if (m_captureDev == NULL)
    {
        printf("   Unable to open device!\n");
        return;
    }
    m_devices = alcGetString(m_captureDev, ALC_CAPTURE_DEVICE_SPECIFIER);
//    printf("   opened device %s\n", m_devices);

// Wait for three seconds to prompt the user
//    for (i = 3; i > 0; i--)
//    {
//        printf("Starting capture in %d...\r", i);
//        fflush(stdout);
//        m_lastTime = time(NULL);
//        m_currentTime = m_lastTime;
//        while (m_currentTime == m_lastTime)
//        {
//            m_currentTime = time(NULL);
//            usleep(100000);
//        }
//    }

//    printf("Starting capture NOW!\n");
//    fflush(stdout);
//    m_lastTime = m_currentTime;

// Capture (roughly) five seconds of audio
    alcCaptureStart(m_captureDev);

    m_bufferSize = 0;

    m_data_buffer = (unsigned char *)malloc(m_data_buffer_size);
    m_data_tmp_ptr = m_data_buffer;
    m_running = true;

    std::cout << "audio capture started" << std::endl;

    while (m_running)
    {
        alcGetIntegerv(m_captureDev, ALC_CAPTURE_SAMPLES, 1, &m_samplesAvailable);

        if (m_samplesAvailable > 0)
        {
            alcCaptureSamples(m_captureDev, m_data_tmp_ptr, m_samplesAvailable);

            m_bufferSize += m_samplesAvailable * 2;

            if (m_bufferSize > m_data_buffer_size - 1000) {

                OnSampleReady(m_data_buffer, m_bufferSize);

                m_bufferSize = 0;
                bzero(m_data_buffer, m_data_buffer_size);
                m_data_tmp_ptr = m_data_buffer;
            }

            m_data_tmp_ptr += m_samplesAvailable * 2;
        }

        // Wait for a bit
        usleep(10000);

        // Update the clock
        m_currentTime = time(NULL);
    }
    free(m_data_buffer);
}

void AudioCAP::StopCapture() {
    try {
        if (this->m_running) {
            this->m_running = false;
            alcCaptureStop(m_captureDev);
        }
        std::cout << "audio capture stopped" << std::endl;
    } catch (...) {
        std::cerr << "error in AudioCAP::StopCapture()" << std::endl;
    }
}

bool AudioCAP::IsRunning() {
    return m_running;
}

void AudioCAP::OnSampleReady(const void *p, size_t size) {

}