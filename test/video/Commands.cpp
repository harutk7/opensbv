//
// Created by harut on 8/4/17.
//

#include "Commands.h"

Commands::Commands(uint16_t port) {
    this->m_port = port;
    this->m_mutex_client;
}

Commands::~Commands() {
    close(m_sockFd); // close server socket
}

void Commands::SetTracker(Tracker *tracker) {
    this->m_tracker = tracker;
}

void Commands::Run() {

    // bind tcp until it is ready
    while(!this->BindTCP()) {

        sleep(2); // sleep for 2 seconds after next try
    }

    // run thread that listens clients
    m_listenParams.sockFd = &this->m_sockFd;
    m_listenParams.commands = (Commands *)this;
    m_listenParams.mutex_client = &this->m_mutex_client;
    pthread_create(&this->m_clientListenT,NULL,&Commands::listenWorker,&m_listenParams);

    // set affinity of cpu
    CPU_ZERO(&this->m_cpuSetClientWorker);
    CPU_SET(2, &this->m_cpuSetClientWorker);
    pthread_setaffinity_np(m_clientListenT, sizeof(cpu_set_t), &this->m_cpuSetClientWorker);

}

bool Commands::BindTCP() {
    try {
        m_sockFd = socket(AF_INET, SOCK_STREAM, 0); // create socket
        int enable = 1;
        setsockopt(m_sockFd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)); // make it reusable after exit
        m_serv_addr.sin_family = AF_INET;
        m_serv_addr.sin_addr.s_addr = htons(INADDR_ANY);
        m_serv_addr.sin_port = htons(this->m_port);
        if (bind(m_sockFd, (struct sockaddr *) &m_serv_addr,sizeof(m_serv_addr)) < 0) {
            std::cout << "error running commands server... trying again." << std::endl;
            return false;
        }

        listen(m_sockFd,2); // listen to socket

        std::cout << "commands running on: " << this->m_port << std::endl;
    } catch (...) {
        std::cerr << "error on tcp... trying again." << std::endl;
        return false;
    }

    return true;
}

void *Commands::listenWorker(void *ptr) {
    try {
        // parameters structure
        commandsListenWorkerStruct params = *(commandsListenWorkerStruct *)ptr;
        commandsWorkerStruct workerParams; // worker struct
        workerParams.commands = params.commands;
        workerParams.mutex_client = params.mutex_client;
        workerParams.sockFd = params.sockFd;
        pthread_t clientThread;
        struct sockaddr_in cli_addr;
        socklen_t clilen;
        int newSockFd; // new client socket fd
        char buffer[BUFFER_SIZE] = {0}; // buffer for sending header
        char receiveBuffer[BUFFER_SIZE] = {0}; // receive buffer
        string receivedData; // received data

        sprintf(buffer, CMD_VERITY); // verify string

        clilen = sizeof(cli_addr);

        // accept connection from client
        while((newSockFd = accept(*(params.sockFd), (struct sockaddr *) &cli_addr, &clilen))) {

            try {
                pthread_mutex_lock(params.mutex_client); // lock

                std::cout << "client " << newSockFd << " connected" << std::endl;

                // send verify header
                if (send(newSockFd,buffer,strlen(buffer), MSG_NOSIGNAL) < 0) {
                    continue;
                }

                // receive verify password
                if (recv(newSockFd,receiveBuffer, BUFFER_SIZE, MSG_NOSIGNAL) >= 0) {
                    receivedData.assign(receiveBuffer); // assign to string

                    if (receivedData == VERIFY_COMMANDS_CLIENT) { // verify string

                        // new sock fd
                        workerParams.newSockFD = newSockFd;

                        // call client worker thread
                        pthread_create(&clientThread,NULL,&Commands::clientWorker,&workerParams);
                    } else
                        std::cout << newSockFd << " not verified " << endl;
                }

                pthread_mutex_unlock(params.mutex_client); // unlock
            } catch(...) {
                std::cerr << "error in listenWorker accept" << std::endl;
            }
        }
    } catch (...) {
        std::cerr << "error in commands listen thread" << std::endl;
    }
}

void *Commands::clientWorker(void *ptr) {
    try {
        // parameters structure
        commandsWorkerStruct params = *(commandsWorkerStruct *)ptr;
        char buffer[BUFFER_SIZE] = {0}; // buffer for sending header
        char receiveBuffer[BUFFER_SIZE] = {0}; // receive buffer
        string receivedData; // received data
        std::size_t found; // string search index
        std::vector<std::string> commandVector; // command vector after explode
        std::vector<std::string> trackerVector; // coordinates vector after explode

        // receive commands from client
        while (ssize_t n = recv(params.newSockFD,receiveBuffer, BUFFER_SIZE, MSG_NOSIGNAL)) {
            try {
                if (n >= 0) {
                    receivedData.assign(receiveBuffer); // assign to string

                    commandVector = GeneralHelper::explode(receivedData, CMD_DELIMITER);

                    if (!commandVector.empty()) {
                        if (commandVector.front() == CMD_TRACKER) {
                            std::cout << "Stoping Tracking" << std::endl;

                            // Disable Tracking
                            params.commands->GetTracker()->DisableTracker();
                        }
                        else if (commandVector.front() == CMD_TRACKER_TARGET) {
                            std::cout << "Starting Tracking" << std::endl;

                            trackerVector = GeneralHelper::explode(commandVector.back(), TRACK_COORDINATES_DELIMITER);

                            // Enable Tracker
                            params.commands->GetTracker()->EnableTracker(
                                    atoi(trackerVector.at(0).c_str()),
                                    atoi(trackerVector.at(1).c_str()),
                                    atoi(trackerVector.at(2).c_str()),
                                    atoi(trackerVector.at(3).c_str())
                            );
                        }
                    }

                    // clear buffer
                    memset(receiveBuffer, 0, sizeof receiveBuffer);
                }
            } catch(...) {
                std::cerr << "error in clientWorker listen" << std::endl;
            }
        }

        // close socket
        close(params.newSockFD);
    } catch (...) {
        std::cerr << "error in client working thread" << std::endl;
    }
}

Tracker *Commands::GetTracker() {
    return this->m_tracker;
}
