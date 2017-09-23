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
const char* const DeviceIndex::option = "-d";
const std::string DeviceIndex::command = "DEVICE_INDEX";

/**
 * Returns true if value is within the range allowable for this parameter,
 * false otherwise.
 */
bool DeviceIndex::isValid(const uint32_t value)
{
    return NumericParameter<uint32_t>::isValid(value, minValid, maxValid);
}

/**
 * Returns a new DeviceIndex instance if the parameter is valid. Throws a std::out_of_range
 * if the parameter is invalid.
 */
const DeviceIndex DeviceIndex::create(const uint32_t value)
{
    if (!isValid(value))
    {
        throw std::out_of_range(NumericParameter<uint32_t>::getOutOfRangeParamExceptionMessage(value, minValid, maxValid));
    }
    else
    {
        return DeviceIndex(value);
    }
}

const DeviceIndex DeviceIndex::create(const std::string& value)
{
    return create(stringToInt(value));
}

const std::string DeviceIndex::getCommand()
{
    return command;
}
