//
// Created by harut on 9/7/17.
//

#ifndef OPENSBV_AUDIOTEST_H
#define OPENSBV_AUDIOTEST_H

#define NUM_BUFFERS 6

#include "opensbv/audio/AudioCAP.h"
#include "opensbv/streamer/Streamer.h"
void *playerWorker(void* ptr);
struct audioParams {
    ALuint *m_source;
    ALuint *m_buffer;
    pthread_mutex_t *m_mtx;
    bool *m_running;
    unsigned char *m_data;
    size_t *m_data_size;
    int *m_sampleRate;
};

class AudioTest: private AudioCAP {
public:
    AudioTest();
    ~AudioTest();

    void SetStreamer(Streamer *stream);
    void OnSampleReady(const void *p, size_t size) override;
    void Run();
    void Stop();
private:
    Streamer *m_streamer;

    ALuint            m_buffer[NUM_BUFFERS];
    ALuint            m_source;
    ALint             m_playState;
    ALCdevice *       m_mainDev;
    ALCcontext *      m_mainContext;

    const ALCchar *   devices;
    const ALCchar *         ptr;
    const ALCchar *curDev;
    bool m_first = true;
    bool m_running = true;
    pthread_t m_thread;
    audioParams m_playParams;
    size_t m_data_size;
    unsigned char* m_data;
    int m_sampleRate;
    int m_releaseSize;
    ushort m_maxReleaseSize;
    pthread_mutex_t m_mtx = PTHREAD_MUTEX_INITIALIZER;

};


#endif //OPENSBV_AUDIOTEST_H
