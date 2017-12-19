/*
 * ResampleRate.cpp
 *
 *  Created on: Sep 2, 2017
 *      Author: Bennett Sherman
 */

// System Includes
#include <stdint.h>
#include <stdexcept>
#include <string>

// Project Includes
#include "ResampleRate.hpp"

// Static Initialization
const char* const ResampleRate::OPTION = "-r";
const std::string ResampleRate::COMMAND = "RESAMPLE_RATE";

/**
 * Returns true if value is within the range allowable for this parameter,
 * false otherwise.
 */
bool ResampleRate::isValid(const uint32_t value)
{
    return NumericParameter<uint32_t>::isValid(value, MIN_VALID, MAX_VALID);
}

/**
 * Constructs a new ResampleRate instance if the parameter is valid.
 * Throws a std::out_of_range if the parameter is invalid.
 */
ResampleRate::ResampleRate(uint32_t value) :
        NumericParameter(value, OPTION)
{
    if (!isValid(value))
    {
        throw std::out_of_range(
                NumericParameter<uint32_t>::getOutOfRangeParamExceptionMessage(
                        value, MIN_VALID, MAX_VALID));
    }
}

/**
 * Gets the string passed to the daemon which is used to indicate
 * that a new parameter of this type should be created.
 */
const std::string ResampleRate::getCommand()
{
    return COMMAND;
}

const char* ResampleRate::getOption()
{
    return OPTION;
}
