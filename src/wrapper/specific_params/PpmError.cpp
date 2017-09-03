/*
 * PpmError.cpp
 *
 *  Created on: Sep 3, 2017
 *      Author: Bennett Sherman
 */

// System Includes
#include <stdint.h>
#include <stdexcept>

// Project Includes
#include "PpmError.hpp"

/**
 * Returns true if value is within the range allowable for this parameter,
 * false otherwise.
 */
bool PpmError::isValid(const uint32_t value)
{
    return NumericParameter<uint32_t>::isValid(value, minValid, maxValid);
}

/**
 * Returns a new PpmError instance if the parameter is valid. Throws a std::invalid_argument
 * if the parameter is invalid.
 */
const PpmError PpmError::create(const uint32_t value)
{
    if (!isValid(value))
    {
        throw std::invalid_argument(NumericParameter<uint32_t>::getOutOfRangeParamExceptionMessage(value, minValid, maxValid));
    }
    else
    {
        return PpmError(value);
    }
}





