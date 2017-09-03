/*
 * NumericParameter.hpp
 *
 *  Created on: Sep 2, 2017
 *      Author: Bennett Sherman
 */

#ifndef WRAPPER_NUMERICPARAMETER_HPP_
#define WRAPPER_NUMERICPARAMETER_HPP_

// System Includes
#include <stdint.h>

// Project Includes
#include "BaseParameter.hpp"

// The type used here should be an arithmetic type.
template <typename T>
class NumericParameter : public BaseParameter<int64_t>
{

protected:
    NumericParameter(T value, char option, T minValid, T maxValid) :
        BaseParameter(value, option), minValid(minValid), maxValid(maxValid) {};

    static bool isValid(const T value, const T min, const T max);

    static std::string getOutOfRangeParamExceptionMessage(const T value, const T min, const T max);

private:
    // maxValid and minValid represent the maximum and minimum allowable values
    // for this parameter, respectively, inclusive.
    T minValid;
    T maxValid;

    static const char OUT_OF_RANGE_EXCEPTION_MSG_FORMAT_STRING[];
    static const size_t OUT_OF_RANGE_EXCEPTION_MSG_MAX_SIZE = 100;
};


#endif /* WRAPPER_NUMERICPARAMETER_HPP_ */
