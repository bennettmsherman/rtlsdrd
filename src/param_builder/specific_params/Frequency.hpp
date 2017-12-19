/*
 * Frequency.hpp
 *
 *  Created on: Sep 2, 2017
 *      Author: Bennett Sherman
 */

#ifndef WRAPPER_FREQUENCY_HPP_
#define WRAPPER_FREQUENCY_HPP_

// System Includes
#include <stdint.h>
#include <string>

// Project Includes
#include "NumericParameter.hpp"

class Frequency : public NumericParameter<uint32_t>
{
public:
    Frequency(uint32_t value = DEFAULT_VALUE);
    Frequency(const std::string& value) :
            Frequency(stringToInt(value))
    {
    }
    ;

    static bool isValid(const uint32_t value);
    const std::string getCommand();

    static const std::string COMMAND;

private:
    static const char* const OPTION;

    // TODO find true min and max
    static const uint32_t MIN_VALID = 0;

    // 2GHz
    static const uint32_t MAX_VALID = 2000000000;

    // 91.1MHz
    static const uint32_t DEFAULT_VALUE = 91100000;
};

#endif /* WRAPPER_FREQUENCY_HPP_ */
