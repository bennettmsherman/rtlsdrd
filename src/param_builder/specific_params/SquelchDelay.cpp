/*
 * SquelchDelay.cpp
 *
 *  Created on: Sep 3, 2017
 *      Author: Bennett Sherman
 */

// System Includes
#include <stdint.h>
#include <stdexcept>
#include <string>

// Project Includes
#include "SquelchDelay.hpp"

// Static Initialization
const char* const SquelchDelay::OPTION = "-t";
const std::string SquelchDelay::COMMAND = "SQUELCH_DELAY";

/**
 * Returns true if value is within the range allowable for this parameter,
 * false otherwise.
 */
bool SquelchDelay::isValid(const int32_t value)
{
    return NumericParameter<int32_t>::isValid(value, MIN_VALID, MAX_VALID);
}

/**
 * Constructs a new SquelchDelay instance if the parameter is valid.
 * Throws a std::out_of_range if the parameter is invalid.
 */
SquelchDelay::SquelchDelay(int32_t value) :
        NumericParameter(value, OPTION)
{
    if (!isValid(value))
    {
        throw std::out_of_range(
                NumericParameter<int32_t>::getOutOfRangeParamExceptionMessage(
                        value, MIN_VALID, MAX_VALID));
    }
}

/**
 * Gets the string passed to the daemon which is used to indicate
 * that a new parameter of this type should be created.
 */
const std::string SquelchDelay::getCommand()
{
    return COMMAND;
}
