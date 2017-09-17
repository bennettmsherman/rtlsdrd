/*
 * SampleRate.cpp
 *
 *  Created on: Sep 2, 2017
 *      Author: Bennett Sherman
 */

// System Includes
#include <stdint.h>
#include <stdexcept>
#include <string>

// Project Includes
#include "SampleRate.hpp"

// Static Initialization
const char* const SampleRate::option = "-s";

/**
 * Returns true if value is within the range allowable for this parameter,
 * false otherwise.
 */
bool SampleRate::isValid(const uint32_t value)
{
    return NumericParameter<uint32_t>::isValid(value, minValid, maxValid);
}

/**
 * Returns a new SampleRate instance if the parameter is valid. Throws a std::out_of_range
 * if the parameter is invalid.
 */
const SampleRate SampleRate::create(const uint32_t value)
{
    if (!isValid(value))
    {
        throw std::out_of_range(NumericParameter<uint32_t>::getOutOfRangeParamExceptionMessage(value, minValid, maxValid));
    }
    else
    {
        return SampleRate(value);
    }
}

const SampleRate SampleRate::create(const std::string& value)
{
    return create(stringToInt(value));
}

const char* SampleRate::getOption()
{
    return option;
}

