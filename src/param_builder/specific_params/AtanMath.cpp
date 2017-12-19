/*
 * AtanMath.cpp
 *
 *  Created on: Sep 6, 2017
 *      Author: bensherman
 */

// System Includes
#include <stdint.h>
#include <stdexcept>
#include <string>

// Project Includes
#include "AtanMath.hpp"

// Static Initialization
// TODO is none okay?
const char* const AtanMath::OPTION = "-A";
const std::vector<std::string> AtanMath::VALID_VALUES { "std", "fast", "lut"};
const std::string AtanMath::DEFAULT_VALUE = "std";
const std::string AtanMath::COMMAND = "ATAN_MATH";

/**
 * Returns true if value is within the range allowable for this parameter,
 * false otherwise.
 */
bool AtanMath::isValid(const std::string& value)
{
    return StringParameter::isValid(value, VALID_VALUES);
}

/**
 * Gets the string passed to the daemon which is used to indicate
 * that a new parameter of this type should be created.
 */
const std::string AtanMath::getCommand()
{
    return COMMAND;
}

/*
 * Constructs a new AtanMath instance if the parameter is valid.
 * Throws a std::invalid_argument if the parameter is invalid.
 */
AtanMath::AtanMath(const std::string& value) :
        StringParameter(value, OPTION)
{
    if (!isValid(value))
    {
        throw std::invalid_argument(
                StringParameter::getInvalidArgExceptionMessage(value));
    }
}

