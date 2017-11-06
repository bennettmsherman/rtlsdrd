/*
 * rtlsdrd.cpp
 *
 *  Created on: Sep 2, 2017
 *      Author: Bennett Sherman
 */

// System Includes
#include <string>
#include <iostream>
#include <csignal>

// Project Includes
#include "CommandParser.hpp"
#include "RtlFmParameterBuilder.hpp"
#include "SystemUtils.hpp"
#include "TcpServer.hpp"

void terminationSignalHandler(int sigNum)
{
    std::cout << "Received signal: " << sigNum << std::endl;
    std::cout << "Attempting to kill aplay and rtl_fm" << std::endl;
    RtlFmRunner::getInstance().killAplayAndRtlFm();

    TcpServer::getInstance().terminate();

    std::cout << "Exiting!" << std::endl;
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    // Execute terminationSignalHandler() in the event that any of the signals
    // below are caught
    signal(SIGTERM, terminationSignalHandler);
    signal(SIGABRT, terminationSignalHandler);
    signal(SIGINT, terminationSignalHandler);
    signal(SIGKILL, terminationSignalHandler);

    // If argc >= 2, use the second argument as the control name used for
    // volume control.
    if (argc >= 2)
    {
        SystemUtils::getInstance(argv[1]);
    }

    SystemUtils::getInstance().setVolume(0);

    // If argc >= 3, use the third argument as the audio device to output to.
    // This differs from argv[2] in that argv[2] is the simple volume control
    // name, whereas argv[3] is the hardware device name to stream the audio to.
    if (argc >= 3)
    {
        RtlFmParameterBuilder::setAudioOutputDeviceName(argv[2]);
    }

    // Have the server wait for, accept, and process connections
    TcpServer::getInstance().run();
}
