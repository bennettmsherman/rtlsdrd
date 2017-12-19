/*
 * SampleRate.hpp
 *
 *  Created on: Sep 2, 2017
 *      Author: Bennett Sherman
 */

#ifndef WRAPPER_SAMPLERATE_HPP_
#define WRAPPER_SAMPLERATE_HPP_

// System Includes
#include <stdint.h>
#include <string>

// Project Includes
#include "NumericParameter.hpp"

class SampleRate : public NumericParameter<uint32_t>
{
public:
    SampleRate(uint32_t value = DEFAULT_VALUE);
    SampleRate(const std::string& value) :
            SampleRate(stringToInt(value))
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

    // 192 kS/s (aplay's max)
    static const uint32_t DEFAULT_VALUE = 192000;
};

#endif /* WRAPPER_SAMPLERATE_HPP_ */
