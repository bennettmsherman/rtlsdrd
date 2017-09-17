/*
 * rtlsdrd.cpp
 *
 *  Created on: Sep 2, 2017
 *      Author: Bennett Sherman
 */

// System Includes
#include <string>
#include <iostream>

// Project Includes
#include "CommandParser.hpp"
#include "RtlFmParameterBuilder.hpp"

int main()
{

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
}
