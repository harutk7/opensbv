//
// Created by harut on 8/4/17.
//

#ifndef PB_MAIN_COMMANDS_H
#define PB_MAIN_COMMANDS_H

#define VERIFY_COMMANDS_CLIENT ("verifyString321")
#define CMD_DELIMITER ':'
#define TRACK_COORDINATES_DELIMITER ','
#define CMD_VERITY ("VERIFY:")
#define CMD_TRACKER ("TRACKER")
#define CMD_TRACKER_TARGET ("TRACKERTARGET")

#include <pthread.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <set>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <memory>

#include "Tracker.h"
#include "opensbv/helpers/general/GeneralHelper.h"

class Commands;

/// Listen struct for worker Thread
struct commandsListenWorkerStruct {
    int *sockFd; ///< Server socked fd
    Commands *commands; ///< Commands for control
    pthread_mutex_t *mutex_client; ///< Main mutex for storing frame and sending it
};

/// Listen struct for main worker Thread
struct commandsWorkerStruct {
    int *sockFd; ///< Server socked fd
    int newSockFD; ///< Client socked fd
    pthread_mutex_t *mutex_client; ///< Mutex for sending frames
    Commands *commands; ///< Commands for control
};

/// Commands server for working controlling program
/**
 * TCP Server based class for controlling the program from remote host
 */
class Commands {
public:
    Commands(uint16_t port); ///< Constructor
    ~Commands(); ///< Destructor

    /// Set Tracker Instance
    /**
     * Set Tracker class instance for control
     * @param tracker
     */
    void SetTracker(Tracker *tracker);

    /// Get Tracker Instance
    Tracker *GetTracker();

    /// Run Command Server
    /**
     * Run main tcp server and thread workers for listening and command processing
     */
    void Run();

private:
    Tracker *m_tracker; ///< Tracker class pointer
    int m_sockFd; ///< Server socket fd
    uint16_t m_port; ///< Stream server port
    struct sockaddr_in m_serv_addr; ///< Server socket struct
    cpu_set_t m_cpuSetClientWorker; ///< Cpu set for affinity set
    pthread_mutex_t m_mutex_client = PTHREAD_MUTEX_INITIALIZER; ///< Mutex for control frames
    pthread_t m_clientListenT; ///< Accept client and send stream threads
    commandsListenWorkerStruct m_listenParams; ///< Listen params for thread

    /// Bind TCP Socket
    bool BindTCP();

    /// Listen for Client connection Thread worker
    static void *listenWorker(void* ptr);

    /// Listen for Client command Thread worker
    static void *clientWorker(void* ptr); // listen thread function
};


#endif //PB_MAIN_COMMANDS_H