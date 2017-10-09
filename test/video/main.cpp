//
// Created by harut on 8/2/17.
//



#include "Config.h"
#include "opensbv/streamer/Streamer.h"
#include "opensbv/serial/Serialrw.h"
#include "Tracker.h"
#include "Commands.h"

int main()
{
    // init Config
    Config config("../config.txt");

    // init Serial
    SerialRW serial(config.GetSerialRead(), config.GetSerialWrite(), 115200);

//    StreamerRTSP streamerRTSP; // run on 0 and 1 CPU
//    streamerRTSP.Run(5543,5543);

    // init the streamer
    Streamer streamer; // run on 0 and 1 CPU
    streamer.SetStreamType(STREAM_MJPG);
    streamer.SetStreamPort(7777);
    streamer.SetImageParams(FRAME_WIDTH, FRAME_HEIGHT, config.GetStreamQuality());
    streamer.SetStreamDataType(STREAM_DATA_IMAGE);
    streamer.SetImageColorType(IMAGE_COLOR_GRAYSCALE);
    streamer.Run(); // run streamer

    // init the tracker
    Tracker tracker(config.GetCamera());
    tracker.SetConfig(&config); // set config
    tracker.SetStreamer(&streamer); // set streamer
    tracker.SetSerial(&serial); // set serial

    // init  commands
    Commands commands(config.GetCommandsPort()); // run on 2 CPU
    commands.SetTracker(&tracker); // set tracker
    commands.Run(); // run commands

    // set cpu affinities for main thread
    cpu_set_t cpu_set; // cpu set for affinity set
    CPU_ZERO(&cpu_set);
    CPU_SET(4, &cpu_set);
    pthread_t current_thread = pthread_self();
    pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpu_set);

    // run tracker main instance
    tracker.Run(); // run on 4, 6 CPU

    return 0;
}

// ############### MAIN STRUCTURE ################
// Commands << Tracker << Config << Streamer
// ############### CPU USAGE ##################
// Streamer (0,1); Commands (2); Tracker (4,6)

