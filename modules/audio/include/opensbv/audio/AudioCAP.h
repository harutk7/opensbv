//
// Created by harut on 9/7/17.
//

#ifndef OPENSBV_AUDIOCAP_H
#define OPENSBV_AUDIOCAP_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <sys/time.h>
#include <ctime>
#include <vector>

/**
 * Audio Capture class
 */
class AudioCAP {
public:
    AudioCAP(ushort bufferSize, int sampleRate, int releaseSize); ///< Constructor
    AudioCAP(); ///< Constructor
    virtual ~AudioCAP(); ///< Destructor

    /// Start Capturing
    bool StartCapture();

    /// Stop Capture
    bool StopCapture();

    /// On Sample Ready
    virtual void OnSampleReady(const void *p, size_t size);

    /// Return list of devices
    std::vector<std::string> GetDeviceList();

    // Set Device to Capture
    void SetDevice(std::string device);

    /// is capture running
    bool IsRunning();

    /// Set Sample Rate
    void SetSampleRate(int rate);

    /// Set Release Size
    void SetReleaseSize(int size);

    /// Set Max Release Size
    void SetMaxReleaseSize(ushort size);
private:
    size_t               m_bufferSize;
    const ALCchar     *m_devices;
    const ALCchar     *m_ptr;
    std::string             m_currentDevice;
    ALCdevice         *m_mainDev;
    ALCcontext        *m_mainContext;
    ALCdevice         *m_captureDev;
    ALint             m_samplesAvailable;
    time_t            m_currentTime;
    time_t            m_lastTime;
    ALuint            m_buffer;
    ALuint            m_source;
    ALint             m_playState;
    unsigned char     *m_data_tmp_ptr;
    unsigned char     *m_data_buffer;
    ushort            m_data_buffer_size;
    bool              m_running = false;
    int               m_sampleRate = 22050;
    int               m_releaseSize = 200;
    pthread_mutex_t   m_audioMutex = PTHREAD_MUTEX_INITIALIZER;

};

#endif //OPENSBV_AUDIOCAP_H