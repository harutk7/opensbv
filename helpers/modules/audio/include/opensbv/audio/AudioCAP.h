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

/**
 * Audio Capture class
 */
class AudioCAP {
public:
    AudioCAP(ushort bufferSize); ///< Constructor
    AudioCAP(); ///< Constructor
    virtual ~AudioCAP(); ///< Destructor

    /// Start Capturing
    void StartCapture();

    /// Stop Capture
    void StopCapture();

    /// On Sample Ready
    virtual void OnSampleReady(const void *p, size_t size);

    /// is capture running
    bool IsRunning();
private:
    size_t               m_bufferSize;
    const ALCchar     *m_devices;
    const ALCchar     *m_ptr;
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
    bool              m_running;

};

#endif //OPENSBV_AUDIOCAP_H