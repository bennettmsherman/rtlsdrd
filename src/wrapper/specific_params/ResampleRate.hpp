/*
 * ResampleRate.hpp
 *
 *  Created on: Sep 6, 2017
 *      Author: bensherman
 */

#ifndef WRAPPER_SPECIFIC_PARAMS_RESAMPLERATE_HPP_
#define WRAPPER_SPECIFIC_PARAMS_RESAMPLERATE_HPP_

// System Includes
#include <stdint.h>
#include <string>

// Project Includes
#include "NumericParameter.hpp"

class ResampleRate : public NumericParameter<uint32_t>
{
public:
    static bool isValid(const uint32_t value);

    static const ResampleRate create(const uint32_t value=defaultValue);
    static const ResampleRate create(const std::string& value);

protected:
    ResampleRate(uint32_t value) : NumericParameter(value, option, minValid, maxValid) {};

private:
    static const char option ='r';

    // TODO find true min and max
    static const uint32_t minValid = 0;

    // 3.2 MS/s
    static const uint32_t maxValid = 3200000;

    // 24 kS/s
    static const uint32_t defaultValue = 24000;
};

#endif /* WRAPPER_SPECIFIC_PARAMS_RESAMPLERATE_HPP_ */