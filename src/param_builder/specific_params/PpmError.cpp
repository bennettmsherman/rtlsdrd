/*
 * PpmError.cpp
 *
 *  Created on: Sep 3, 2017
 *      Author: Bennett Sherman
 */

// System Includes
#include <stdint.h>
#include <stdexcept>
#include <string>

// Project Includes
#include "PpmError.hpp"

// Static Initialization
const char* const PpmError::OPTION = "-p";
const std::string PpmError::COMMAND = "PPM_ERROR";

/**
 * Returns true if value is within the range allowable for this parameter,
 * false otherwise.
 */
bool PpmError::isValid(const uint32_t value)
{
    return NumericParameter<uint32_t>::isValid(value, MIN_VALID, MAX_VALID);
}

/**
 * Constructs a new PpmError instance if the parameter is valid.
 * Throws a std::invalid_argument if the parameter is invalid.
 */
PpmError::PpmError(uint32_t value) :
        NumericParameter(value, OPTION)
{
    if (!isValid(value))
    {
        throw std::invalid_argument(
                NumericParameter<uint32_t>::getOutOfRangeParamExceptionMessage(
                        value, MIN_VALID, MAX_VALID));
    }
}

/**
 * Gets the string passed to the daemon which is used to indicate
 * that a new parameter of this type should be created.
 */
const std::string PpmError::getCommand()
{
    return COMMAND;
}
