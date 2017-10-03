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
    static bool isValid(const uint32_t value);
    static const SampleRate create(const uint32_t value=DEFAULT_VALUE);
    static const SampleRate create(const std::string& value);
    static const char* getOption();
    const std::string getCommand();

    static const std::string COMMAND;

protected:
    SampleRate(uint32_t value) : NumericParameter(value, OPTION, MIN_VALID, MAX_VALID) {};

private:
    static const char* const OPTION;

    // TODO find true min and max
    static const uint32_t MIN_VALID = 0;

    // 3.2 MS/s
    static const uint32_t MAX_VALID = 3200000;

    // 24 kS/s
    static const uint32_t DEFAULT_VALUE = 24000;
};

#endif /* WRAPPER_SAMPLERATE_HPP_ */
