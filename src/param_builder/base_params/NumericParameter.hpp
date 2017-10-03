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
#include <string>

// Project Includes
#include "BaseParameter.hpp"

// The type used here should be an arithmetic type.
template <typename T>
class NumericParameter : public BaseParameter<T>
{
public:
    virtual ~NumericParameter() {};

protected:
    NumericParameter(T value, const char* const option, T minValid, T maxValid) :
        BaseParameter<T>(std::to_string(value), option), minValid(minValid), maxValid(maxValid) {};

    static bool isValid(const T value, const T min, const T max);

    static std::string getOutOfRangeParamExceptionMessage(const T value, const T min, const T max);

    static T stringToInt(const std::string& str);

private:
    // maxValid and minValid represent the maximum and minimum allowable values
    // for this parameter, respectively, inclusive.
    T minValid;
    T maxValid;

    static std::string getNonNumericCharacterInParamExceptionMessage(const char badChar, const size_t index);

    static const char OUT_OF_RANGE_EXCEPTION_MSG_FORMAT_STRING[];
    static const size_t OUT_OF_RANGE_EXCEPTION_MSG_MAX_SIZE = 100;

    static const char NON_NUMERIC_CHAR_IN_PARAM_EXCEPTION_MSG_FORMAT_STRING[];
    static const size_t NON_NUMERIC_CHAR_IN_PARAM_EXCEPTION_MSG_MAX_SIZE = 100;
};


#endif /* WRAPPER_NUMERICPARAMETER_HPP_ */
