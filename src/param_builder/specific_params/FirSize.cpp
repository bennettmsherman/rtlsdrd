/*
 * FirSize.cpp
 *
 *  Created on: Nov 5, 2017
 *      Author: bensherman
 */

// System Includes
#include <stdint.h>
#include <stdexcept>
#include <string>

// Project Includes
#include "FirSize.hpp"

// Static Initialization
const char* const FirSize::OPTION = "-F";
const std::vector<std::string> FirSize::VALID_VALUES { "-1", "0", "9"};
const std::string FirSize::DEFAULT_VALUE = "-1";
const std::string FirSize::COMMAND = "FIR_SIZE";

/**
 * Returns true if value is within the range allowable for this parameter,
 * false otherwise.
 */
bool FirSize::isValid(const std::string& value)
{
    return StringParameter::isValid(value, VALID_VALUES);
}

/**
 * Constructs a new FirSize instance if the parameter is valid.
 * Throws a std::invalid_argument if the parameter is invalid.
 */

FirSize::FirSize(const std::string& value) :
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
const std::string FirSize::getCommand()
{
    return COMMAND;
}

