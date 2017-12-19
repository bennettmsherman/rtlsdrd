/*
 * StringParameter.cpp
 *
 *  Created on: Sep 3, 2017
 *      Author: Bennett Sherman
 */

// System Includes
#include <stdint.h>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include <regex>

// Project Includes
#include "StringParameter.hpp"

// Static initialization
const char StringParameter::INVALID_ARG_EXCEPTION_MSG_FORMAT_STRING[] =
        "ERROR! %s is not a valid choice!";

/**
 * Returns true if the value can be matched to the validFormat regex, false otherwise.
 */
bool StringParameter::isValid(const std::string& value,
        const std::regex& validFormat)
{
    std::smatch sMatches;
    std::regex_match(value, sMatches, validFormat);
    return !sMatches.empty();
}

/**
 * Returns true if value is contained in validValues, false otherwise
 */
bool StringParameter::isValid(const std::string& value,
        const std::vector<std::string>& validValues)
{
    if (std::find(validValues.begin(), validValues.end(), value)
            != validValues.end())
    {
        return true;
    }
    return false;
}

std::string StringParameter::getInvalidArgExceptionMessage(
        const std::string& value)
{
    size_t msgSize = std::strlen(INVALID_ARG_EXCEPTION_MSG_FORMAT_STRING)
            + value.length() + 1;
    char buff[msgSize] {};
    std::snprintf(buff, msgSize, INVALID_ARG_EXCEPTION_MSG_FORMAT_STRING,
            value.c_str());
    buff[msgSize - 1] = 0;
    return std::string(buff);
}

