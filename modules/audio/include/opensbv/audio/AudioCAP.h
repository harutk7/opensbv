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

namespace opensbv {
    namespace audio {

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

            /// Set Device to Capture
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
            size_t               m_bufferSize; /// buffer size
            const ALCchar     *m_devices; /// speaker devices
            const ALCchar     *m_ptr; /// speaker device pointer
            std::string             m_currentDevice; /// current capture device
            ALCdevice         *m_mainDev; /// main device to capture
            ALCcontext        *m_mainContext; /// main content
            ALCdevice         *m_captureDev; /// capture device
            ALint             m_samplesAvailable; /// samples available for capture
            time_t            m_currentTime; /// current Time (for test purposes)
            time_t            m_lastTime; /// last time (for test purposes)
            ALuint            m_buffer; /// buffer of samples (for test)
            ALuint            m_source; /// source (for test)
            ALint             m_playState; /// current playing state (for test)
            unsigned char     *m_data_tmp_ptr; /// pointer to captured data buffer
            unsigned char     *m_data_buffer; /// captured data buffer
            ushort            m_data_buffer_size; /// capture data size
            bool              m_running = false; /// capturing or not
            int               m_sampleRate = 22050; /// samples rate
            int               m_releaseSize = 200; /// sample release size
            pthread_mutex_t   m_audioMutex = PTHREAD_MUTEX_INITIALIZER; /// mutex for capture device

        };
    }
}

#endif //OPENSBV_AUDIOCAP_H