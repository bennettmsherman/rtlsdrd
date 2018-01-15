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
#include <string.h>

// Project Includes
#include "CommandParser.hpp"
#include "RtlFmParameterBuilder.hpp"
#include "SystemUtils.hpp"
#include "TcpServer.hpp"
#include "ArgParser.hpp"

void terminationSignalHandler(int sigNum)
{
    std::cout << "Received signal: " << strsignal(sigNum) << "(#" << sigNum <<
            ")" << std::endl;
    std::cout << "Attempting to kill aplay and rtl_fm" << std::endl;
    RtlFmRunner::getInstance().killAplayAndRtlFm();

    TcpServer::getInstance().terminate();

    std::cout << "Exiting!" << std::endl;
    exit(EXIT_SUCCESS);
}

int main(int argc, const char *argv[])
{
    // Execute terminationSignalHandler() in the event that any of the signals
    // below are caught
    signal(SIGTERM, terminationSignalHandler);
    signal(SIGABRT, terminationSignalHandler);
    signal(SIGINT, terminationSignalHandler);
    signal(SIGKILL, terminationSignalHandler);

    // Parse and apply the command line arguments
    ArgParser::parse(argc, argv);

    // Initially set the volume to 0%
    SystemUtils::getInstance().setVolume(0);

    // Have the server wait for, accept, and process connections
    TcpServer::getInstance().run();
}
