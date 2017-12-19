/*
 * ScannableFrequency.cpp
 *
 *  Created on: Oct 3, 2017
 *      Author: bensherman
 */

// System Includes
#include <stdint.h>
#include <stdexcept>
#include <string>
#include <regex>

// Project Includes
#include "ScannableFrequency.hpp"
#include "StringParameter.hpp"

// Static Initialization
const char* const ScannableFrequency::OPTION = "-f";
const std::string ScannableFrequency::DEFAULT_VALUE = "91100000:104300000:200k";
const std::string ScannableFrequency::COMMAND = "SCANNABLE_FREQUENCY";
const std::regex ScannableFrequency::VALID_PARAM_REGEX {
        "^[0-9]+:[0-9]+:[0-9]+?"};

/**
 * Returns true if value represents a valid parameter, false otherwise
 */
bool ScannableFrequency::isValid(const std::string& value)
{
    return StringParameter::isValid(value, VALID_PARAM_REGEX);
}

/**
 * Constructs a new ScannableFrequency instance if the parameter is valid.
 * Throws a std::invalid_argument if the parameter is invalid.
 */
ScannableFrequency::ScannableFrequency(const std::string& value) :
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
const std::string ScannableFrequency::getCommand()
{
    return COMMAND;
}

