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
    static bool isValid(const int32_t value);

    static const TunerGain create(const int32_t value=defaultValue);
    static const TunerGain create(const std::string& value);

protected:
    TunerGain(int32_t value) : NumericParameter(value, OPTION, minValid, maxValid) {};

private:
    static const char* const OPTION;

    static const int32_t minValid = -100;

    static const int32_t maxValid = INT32_MAX;

    static const int32_t defaultValue = -100;
};

#endif /* PARAM_BUILDER_SPECIFIC_PARAMS_TUNERGAIN_HPP_ */
