/*
 * EnableOption.cpp
 *
 *  Created on: Sep 3, 2017
 *      Author: Bennett Sherman
 */

// System Includes
#include <stdint.h>
#include <stdexcept>
#include <string>

// Project Includes
#include "EnableOption.hpp"

// Static Initialization
// TODO is none okay?
const char* const EnableOption::OPTION = "-E";
const std::vector<std::string> EnableOption::VALID_VALUES { "none", "edge",
        "dc", "deemp", "direct", "offset"};
const std::string EnableOption::DEFAULT_VALUE = "none";
const std::string EnableOption::COMMAND = "ENABLE_OPTION";

/**
 * Returns true if value is within the range allowable for this parameter,
 * false otherwise.
 */
bool EnableOption::isValid(const std::string& value)
{
    return StringParameter::isValid(value, VALID_VALUES);
}

/**
 * Constructs a new EnableOption instance if the parameter is valid.
 * Throws a std::invalid_argument if the parameter is invalid.
 */

EnableOption::EnableOption(const std::string& value) :
        StringParameter(value, OPTION)
{
    if (!isValid(value))
    {
        throw std::invalid_argument(
                StringParameter::getInvalidArgExceptionMessage(value));
    }
}

/**
 * Gets the string passed to the daemon which is used to indicate
 * that a new parameter of this type should be created.
 */
const std::string EnableOption::getCommand()
{
    return COMMAND;
}
