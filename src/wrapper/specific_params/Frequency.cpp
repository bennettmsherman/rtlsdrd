/*
 * Frequency.cpp
 *
 *  Created on: Sep 2, 2017
 *      Author: Bennett Sherman
 */

// System Includes
#include <stdint.h>
#include <stdexcept>
#include <string>

// Project Includes
#include "Frequency.hpp"

// Static Initialization
const char* const Frequency::option = "-f";

/**
 * Returns true if value is within the range allowable for this parameter,
 * false otherwise.
 */
bool Frequency::isValid(const uint32_t value)
{
    return NumericParameter<uint32_t>::isValid(value, minValid, maxValid);
}

/**
 * Returns a new Frequency instance if the parameter is valid. Throws a std::out_of_range
 * if the parameter is invalid.
 */
const Frequency Frequency::create(const uint32_t value)
{
    if (!isValid(value))
    {
        throw std::out_of_range(NumericParameter<uint32_t>::getOutOfRangeParamExceptionMessage(value, minValid, maxValid));
    }
    else
    {
        return Frequency(value);
    }
}

const Frequency Frequency::create(const std::string& value)
{
    return create(stringToInt(value));
}
