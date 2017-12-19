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
const char* const ModulationMode::OPTION = "-M";
const std::vector<std::string> ModulationMode::VALID_VALUES { "fm", "wbfm",
        "raw", "am", "usb", "lsb"};
const std::string ModulationMode::DEFAULT_VALUE = "fm";
const std::string ModulationMode::COMMAND = "MODULATION_MODE";

/**
 * Returns true if value is within the range allowable for this parameter,
 * false otherwise.
 */
bool ModulationMode::isValid(const std::string& value)
{
    return StringParameter::isValid(value, VALID_VALUES);
}

/**
 * Constructs a new ModulationMode instance if the parameter is valid.
 * Throws a std::invalid_argument if the parameter is invalid.
 */
ModulationMode::ModulationMode(const std::string& value) :
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
const std::string ModulationMode::getCommand()
{
    return COMMAND;
}
