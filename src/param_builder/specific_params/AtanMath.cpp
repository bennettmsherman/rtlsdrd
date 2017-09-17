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
const char* const AtanMath::option = "-A";
const std::vector<std::string> AtanMath::validValues {"std", "fast", "lut"};
const std::string AtanMath::defaultValue = "std";

/**
 * Returns true if value is within the range allowable for this parameter,
 * false otherwise.
 */
bool AtanMath::isValid(const std::string& value)
{
    return StringParameter::isValid(value, validValues);
}

/**
 * Returns a new AtanMath instance if the parameter is valid. Throws a std::invalid_argument
 * if the parameter is invalid.
 */
const AtanMath AtanMath::create(const std::string& value)
{
    if (!isValid(value))
    {
        throw std::invalid_argument(StringParameter::getInvalidArgExceptionMessage(value));
    }
    else
    {
        return AtanMath(value);
    }
}




