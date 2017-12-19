/*
 * TunerGain.hpp
 *
 *  Created on: Sep 17, 2017
 *      Author: bensherman
 */

#ifndef PARAM_BUILDER_SPECIFIC_PARAMS_TUNERGAIN_HPP_
#define PARAM_BUILDER_SPECIFIC_PARAMS_TUNERGAIN_HPP_

// System Includes
#include <stdint.h>
#include <string>

// Project Includes
#include "NumericParameter.hpp"

class TunerGain : public NumericParameter<int32_t>
{
public:
    TunerGain(int32_t value = DEFAULT_VALUE);
    TunerGain(const std::string& value) :
            TunerGain(stringToInt(value))
    {
    }
    ;

    static bool isValid(const int32_t value);
    const std::string getCommand();

    static const std::string COMMAND;

private:
    static const char* const OPTION;

    static const int32_t MIN_VALID = -100;

    static const int32_t MAX_VALID = INT32_MAX;

    static const int32_t DEFAULT_VALUE = -100;
};

#endif /* PARAM_BUILDER_SPECIFIC_PARAMS_TUNERGAIN_HPP_ */
