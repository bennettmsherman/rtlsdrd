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
    ResampleRate(uint32_t value = DEFAULT_VALUE);
    ResampleRate(const std::string& value) :
            ResampleRate(stringToInt(value))
    {
    }
    ;

    static bool isValid(const uint32_t value);
    static const char* getOption();
    const std::string getCommand();

    static const std::string COMMAND;

private:
    static const char* const OPTION;

    // TODO find true min and max
    static const uint32_t MIN_VALID = 2000;

    // 3.2 MS/s
    static const uint32_t MAX_VALID = 3200000;

    // 24 kS/s
    static const uint32_t DEFAULT_VALUE = 24000;
};

#endif /* WRAPPER_SPECIFIC_PARAMS_RESAMPLERATE_HPP_ */
