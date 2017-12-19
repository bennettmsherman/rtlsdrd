/*
 * SquelchLevel.hpp
 *
 *  Created on: Sep 2, 2017
 *      Author: Bennett Sherman
 */

#ifndef WRAPPER_SQUELCHLEVEL_HPP_
#define WRAPPER_SQUELCHLEVEL_HPP_

// System Includes
#include <stdint.h>
#include <string>

// Project Includes
#include "NumericParameter.hpp"

class SquelchLevel : public NumericParameter<uint32_t>
{
public:
    SquelchLevel(uint32_t value = DEFAULT_VALUE);
    SquelchLevel(const std::string& value) :
            SquelchLevel(stringToInt(value))
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

    static const uint32_t MAX_VALID = UINT32_MAX;

    static const uint32_t DEFAULT_VALUE = 0;
};

#endif /* WRAPPER_SQUELCHLEVEL_HPP_ */
