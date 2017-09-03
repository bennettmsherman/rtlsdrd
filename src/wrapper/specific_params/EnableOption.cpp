/*
 * EnableOption.cpp
 *
 *  Created on: Sep 3, 2017
 *      Author: bensherman
 */

// System Includes
#include <stdint.h>
#include <stdexcept>

// Project Includes
#include "EnableOption.hpp"

// Static Initialization
// TODO is none okay?
const std::vector<std::string> EnableOption::validValues {"none", "edge", "dc", "deemp", "direct", "offset"};
const std::string EnableOption::defaultValue = "none";

/**
 * Returns true if value is within the range allowable for this parameter,
 * false otherwise.
 */
bool EnableOption::isValid(const std::string& value)
{
    return StringParameter::isValid(value, validValues);
}

/**
 * Returns a new EnableOption instance if the parameter is valid. Throws a std::invalid_argument
 * if the parameter is invalid.
 */
const EnableOption EnableOption::create(const std::string& value)
{
    if (!isValid(value))
    {
        throw std::invalid_argument(StringParameter::getInvalidArgExceptionMessage(value));
    }
    else
    {
        return EnableOption(value);
    }
}
