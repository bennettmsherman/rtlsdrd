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

void terminationSignalHandler(int sigNum)
{
    std::cout << "Received signal: " << sigNum << std::endl;
    std::cout << "Killing aplay and rtl_fm" << std::endl;
    RtlFmRunner::killAplayAndRtlFm();

    std::cout << "Exiting!" << std::endl;
    exit(EXIT_SUCCESS);
}

int main()
{
    signal(SIGTERM, terminationSignalHandler);
    signal(SIGABRT, terminationSignalHandler);
    signal(SIGINT, terminationSignalHandler);
    signal(SIGKILL, terminationSignalHandler);

    RtlFmParameterBuilder wrapper;
    CommandParser parser { wrapper };

    while (true) {
        std::cout << "Enter Command: " << std::flush;
        std::string line = "";
        std::getline(std::cin, line);
        std::string result = parser.execute(line);
        std::cout << "Exec Result: \n" << result << std::endl;
        std::cout << "\n" << std::endl;
    }

    RtlFmRunner::killAplayAndRtlFm();
}
