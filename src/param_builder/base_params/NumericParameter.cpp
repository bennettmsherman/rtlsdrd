/*
 * NumericParameter.cpp
 *
 *  Created on: Sep 2, 2017
 *      Author: Bennett Sherman
 */

// System Includes
#include <stdint.h>
#include <cstdio>
#include <string>
#include <stdexcept>

// Project Includes
#include "NumericParameter.hpp"

// Explicit template initialization
template class NumericParameter<uint32_t> ;
template class NumericParameter<int32_t> ;

// Static initialization
template<typename T>
const char NumericParameter<T>::OUT_OF_RANGE_EXCEPTION_MSG_FORMAT_STRING[] =
        "ERROR! %u is outside of the range %u - %u, inclusive";

template<typename T>
const char NumericParameter<T>::NON_NUMERIC_CHAR_IN_PARAM_EXCEPTION_MSG_FORMAT_STRING[] =
        "ERROR! Character \"%c\" at index: %zu not valid";

/**
 * Returns true if value <= min <= max, false otherwise.
 */
template<typename T>
bool NumericParameter<T>::isValid(const T value, const T min, const T max)
{
    return ((value >= min) && (value <= max));
}

template<typename T>
std::string NumericParameter<T>::getOutOfRangeParamExceptionMessage(
        const T value, const T min, const T max)
{
    char buff[OUT_OF_RANGE_EXCEPTION_MSG_MAX_SIZE] {};
    // TODO proper format string
    std::snprintf(buff, OUT_OF_RANGE_EXCEPTION_MSG_MAX_SIZE,
            OUT_OF_RANGE_EXCEPTION_MSG_FORMAT_STRING, value, min, max);
    buff[OUT_OF_RANGE_EXCEPTION_MSG_MAX_SIZE - 1] = 0;
    return std::string(buff);
}

template<typename T>
std::string NumericParameter<T>::getNonNumericCharacterInParamExceptionMessage(
        const char badChar, const size_t index)
{
    char buff[NON_NUMERIC_CHAR_IN_PARAM_EXCEPTION_MSG_MAX_SIZE] {};
    // TODO proper format string
    std::snprintf(buff, NON_NUMERIC_CHAR_IN_PARAM_EXCEPTION_MSG_MAX_SIZE,
            NON_NUMERIC_CHAR_IN_PARAM_EXCEPTION_MSG_FORMAT_STRING, badChar,
            index);
    buff[NON_NUMERIC_CHAR_IN_PARAM_EXCEPTION_MSG_MAX_SIZE - 1] = 0;
    return std::string(buff);
}

/**
 * Converts the string parameter to a uint32_t. Throws std::invalid_argument
 * if the string contains any characters outside of 0 through 9
 */
template<>
uint32_t NumericParameter<uint32_t>::stringToInt(const std::string& str)
{
    size_t firstInvalidCharIdx = str.find_first_not_of("0123456789");
    if (firstInvalidCharIdx != std::string::npos)
    {
        throw std::invalid_argument(
                getNonNumericCharacterInParamExceptionMessage(
                        str.c_str()[firstInvalidCharIdx], firstInvalidCharIdx));
    }

    return (std::stoull(str));
}

/**
 * Converts the string parameter to an int32_t. Throws std::invalid_argument
 * if the string contains any characters outside of 0 through 9, including the
 * negative symbol.
 */
template<>
int32_t NumericParameter<int32_t>::stringToInt(const std::string& str)
{
    size_t firstInvalidCharIdx = str.find_first_not_of("-0123456789");
    if (firstInvalidCharIdx != std::string::npos)
    {
        throw std::invalid_argument(
                getNonNumericCharacterInParamExceptionMessage(
                        str.c_str()[firstInvalidCharIdx], firstInvalidCharIdx));
    }

    return (std::stoi(str));
}

