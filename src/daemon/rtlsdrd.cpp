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

    // If argc == 2, use the second argument as the audio device to be
    // controlled. Then, set the system volume to 0.
    if (argc == 2)
    {
        SystemUtils::getInstance(argv[1]).setVolume(0);
    }

    // Have the server wait for, accept, and process connections
    TcpServer::getInstance().run();
}
