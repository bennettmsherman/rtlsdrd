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
#include <stdexcept>

// Project Includes
#include "ArgParser.hpp"
#include "CommandParser.hpp"
#include "RtlFmParameterBuilder.hpp"
#include "SystemUtils.hpp"
#include "TcpServer.hpp"

void terminationSignalHandler(int sigNum)
{
    std::cout << "Received signal: " << strsignal(sigNum) << "(#" << sigNum <<
              ")" << std::endl;

    std::cout << "Attempting to kill aplay and rtl_fm" << std::endl;
    RtlFmRunner::getInstance().killAplayAndRtlFm();

    // If exceptions aren't caught when killing the server, std::terminate()
    // will get called recursively.
    try
    {
        TcpServer::getInstance().terminate();
    }
    // We expect an invalid_argument to be thrown when calling getInstance() in
    // the event that the server wasn't instantiated prior. We want to ignore
    // such errors.
    catch (std::invalid_argument& invalidArgErr)
    {
    }
    catch (std::exception& genErr)
    {
        std::cerr << "Caught exception when trying to kill TcpServer; Message: "
                  << genErr.what() << std::endl;
    }

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
