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
#include "TcpServer.hpp"

static TcpServer server{};

void terminationSignalHandler(int sigNum)
{
    std::cout << "Received signal: " << sigNum << std::endl;
    std::cout << "Attempting to kill aplay and rtl_fm" << std::endl;
    RtlFmRunner::killAplayAndRtlFm();

    server.terminate();

    std::cout << "Exiting!" << std::endl;
    exit(EXIT_SUCCESS);
}

int main()
{
    signal(SIGTERM, terminationSignalHandler);
    signal(SIGABRT, terminationSignalHandler);
    signal(SIGINT, terminationSignalHandler);
    signal(SIGKILL, terminationSignalHandler);

    // Have the server wait for, accept, and process connections
    server.run();
}
