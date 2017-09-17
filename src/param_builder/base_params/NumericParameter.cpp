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

// Project Includes
#include "NumericParameter.hpp"

// Explicit template initialization
template class NumericParameter<uint32_t>;
template class NumericParameter<int32_t>;

// Static initialization
template <typename T>
const char NumericParameter<T>::OUT_OF_RANGE_EXCEPTION_MSG_FORMAT_STRING[] = "ERROR! %u is outside of the range %u - %u, inclusive";

/**
 * Returns true if value <= min <= max, false otherwise.
 */
template <typename T>
bool NumericParameter<T>::isValid(const T value, const T min, const T max)
{
    return ((value >= min) && (value <= max));
}

template <typename T>
std::string NumericParameter<T>::getOutOfRangeParamExceptionMessage(const T value, const T min,
        const T max)
{
    char buff[OUT_OF_RANGE_EXCEPTION_MSG_MAX_SIZE] {};
    // TODO proper format string
    std::snprintf(buff, OUT_OF_RANGE_EXCEPTION_MSG_MAX_SIZE, OUT_OF_RANGE_EXCEPTION_MSG_FORMAT_STRING, value, min, max);
    buff[OUT_OF_RANGE_EXCEPTION_MSG_MAX_SIZE - 1] = 0;
    return std::string(buff);
}

template <>
uint32_t NumericParameter<uint32_t>::stringToInt(const std::string& str)
{
    return (std::stoull(str));
}

template <>
int32_t NumericParameter<int32_t>::stringToInt(const std::string& str)
{
    return (std::stoi(str));
}

