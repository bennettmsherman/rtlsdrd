/*
 * SquelchLevel.cpp
 *
 *  Created on: Sep 2, 2017
 *      Author: Bennett Sherman
 */

// System Includes
#include <stdint.h>
#include <stdexcept>
#include <string>

// Project Includes
#include "SquelchLevel.hpp"

// Static Initialization
const char* const SquelchLevel::OPTION = "-l";
const std::string SquelchLevel::COMMAND = "SQUELCH_LEVEL";

/**
 * Returns true if value is within the range allowable for this parameter,
 * false otherwise.
 */
bool SquelchLevel::isValid(const uint32_t value)
{
    return NumericParameter<uint32_t>::isValid(value, MIN_VALID, MAX_VALID);
}

/**
 * Returns a new SquelchLevel instance if the parameter is valid. Throws a std::out_of_range
 * if the parameter is invalid.
 */
const SquelchLevel SquelchLevel::create(const uint32_t value)
{
    if (!isValid(value))
    {
        throw std::out_of_range(NumericParameter<uint32_t>::getOutOfRangeParamExceptionMessage(value, MIN_VALID, MAX_VALID));
    }
    else
    {
        return SquelchLevel(value);
    }
}

const SquelchLevel SquelchLevel::create(const std::string& value)
{
    return create(stringToInt(value));
}

/**
 * Gets the string passed to the daemon which is used to indicate
 * that a new parameter of this type should be created.
 */
const std::string SquelchLevel::getCommand()
{
    return COMMAND;
}
