/*
 * DeviceIndex.cpp
 *
 *  Created on: Sep 3, 2017
 *      Author: Bennett Sherman
 */

// System Includes
#include <stdint.h>
#include <stdexcept>
#include <string>

// Project Includes
#include "DeviceIndex.hpp"

// Static Initialization
const char* const DeviceIndex::OPTION = "-d";
const std::string DeviceIndex::COMMAND = "DEVICE_INDEX";

/**
 * Returns true if value is within the range allowable for this parameter,
 * false otherwise.
 */
bool DeviceIndex::isValid(const uint32_t value)
{
    return NumericParameter<uint32_t>::isValid(value, MIN_VALID, MAX_VALID);
}

/**
 * Constructs a new DeviceIndex instance if the parameter is valid. Throws a std::out_of_range
 * if the parameter is invalid.
 */
DeviceIndex::DeviceIndex(uint32_t value) :
        NumericParameter(value, OPTION)
{
    if (!isValid(value))
    {
        throw std::out_of_range(
                NumericParameter<uint32_t>::getOutOfRangeParamExceptionMessage(
                        value, MIN_VALID, MAX_VALID));
    }
}
;

const std::string DeviceIndex::getCommand()
{
    return COMMAND;
}
