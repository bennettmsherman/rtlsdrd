/*
 * Oversampling.cpp
 *
 *  Created on: Sep 2, 2017
 *      Author: bensherman
 */

// System Includes
#include <stdint.h>
#include <stdexcept>

// Project Includes
#include "Oversampling.hpp"

/**
 * Returns true if value is within the range allowable for this parameter,
 * false otherwise.
 */
bool Oversampling::isValid(const uint32_t value)
{
    return NumericParameter<uint32_t>::isValid(value, minValid, maxValid);
}

/**
 * Returns a new Oversampling instance if the parameter is valid. Throws a std::out_of_range
 * if the parameter is invalid.
 */
const Oversampling Oversampling::create(const uint32_t value)
{
    if (!isValid(value))
    {
        throw std::out_of_range(NumericParameter<uint32_t>::getOutOfRangeParamExceptionMessage(value, minValid, maxValid));
    }
    else
    {
        return Oversampling(value);
    }
}



