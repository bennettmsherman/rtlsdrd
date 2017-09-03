/*
 * SquelchDelay.cpp
 *
 *  Created on: Sep 3, 2017
 *      Author: Bennett Sherman
 */
// System Includes
#include <stdint.h>
#include <stdexcept>

// Project Includes
#include "SquelchDelay.hpp"

/**
 * Returns true if value is within the range allowable for this parameter,
 * false otherwise.
 */
bool SquelchDelay::isValid(const int32_t value)
{
    return NumericParameter<int32_t>::isValid(value, minValid, maxValid);
}

/**
 * Returns a new SquelchDelay instance if the parameter is valid. Throws a std::out_of_range
 * if the parameter is invalid.
 */
const SquelchDelay SquelchDelay::create(const int32_t value)
{
    if (!isValid(value))
    {
        throw std::out_of_range(NumericParameter<int32_t>::getOutOfRangeParamExceptionMessage(value, minValid, maxValid));
    }
    else
    {
        return SquelchDelay(value);
    }
}



