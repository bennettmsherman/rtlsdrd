/*
 * TunerGain.cpp
 *
 *  Created on: Sep 17, 2017
 *      Author: bensherman
 */

// System Includes
#include <stdint.h>
#include <stdexcept>
#include <string>

// Project Includes
#include "TunerGain.hpp"

// Static Initialization
const char* const TunerGain::OPTION = "-t";

/**
 * Returns true if value is within the range allowable for this parameter,
 * false otherwise.
 */
bool TunerGain::isValid(const int32_t value)
{
    return NumericParameter<int32_t>::isValid(value, minValid, maxValid);
}

/**
 * Returns a new TunerGain instance if the parameter is valid. Throws a std::out_of_range
 * if the parameter is invalid.
 */
const TunerGain TunerGain::create(const int32_t value)
{
    if (!isValid(value))
    {
        throw std::out_of_range(NumericParameter<int32_t>::getOutOfRangeParamExceptionMessage(value, minValid, maxValid));
    }
    else
    {
        return TunerGain(value);
    }
}

const TunerGain TunerGain::create(const std::string& value)
{
    return create(stringToInt(value));
}



