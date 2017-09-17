/*
 * ModulationMode.cpp
 *
 *  Created on: Sep 3, 2017
 *      Author: Bennett Sherman
 */

// System Includes
#include <stdint.h>
#include <stdexcept>
#include <string>

// Project Includes
#include "ModulationMode.hpp"

// Static Initialization
const char* const ModulationMode::option ="-M";
const std::vector<std::string> ModulationMode::validValues {"fm", "wbfm", "raw", "am", "usb", "lsb"};
const std::string ModulationMode::defaultValue = "fm";

/**
 * Returns true if value is within the range allowable for this parameter,
 * false otherwise.
 */
bool ModulationMode::isValid(const std::string& value)
{
    return StringParameter::isValid(value, validValues);
}

/**
 * Returns a new ModulationMode instance if the parameter is valid. Throws a std::invalid_argument
 * if the parameter is invalid.
 */
const ModulationMode ModulationMode::create(const std::string& value)
{
    if (!isValid(value))
    {
        throw std::invalid_argument(StringParameter::getInvalidArgExceptionMessage(value));
    }
    else
    {
        return ModulationMode(value);
    }
}


