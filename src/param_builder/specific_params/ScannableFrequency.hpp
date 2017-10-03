/*
 * ScannableFrequency.hpp
 *
 *  Created on: Oct 3, 2017
 *      Author: bensherman
 */

#ifndef PARAM_BUILDER_SPECIFIC_PARAMS_SCANNABLEFREQUENCY_HPP_
#define PARAM_BUILDER_SPECIFIC_PARAMS_SCANNABLEFREQUENCY_HPP_

// System Includes
#include <string>
#include <regex>

// Project Includes
#include "StringParameter.hpp"

class ScannableFrequency : public StringParameter
{
public:
    static bool isValid(const std::string& value);
    static const ScannableFrequency create(const std::string& value=DEFAULT_VALUE);
    const std::string getCommand();

    static const std::string COMMAND;

protected:
    ScannableFrequency(const std::string& value) : StringParameter(value, OPTION) {};

private:
    static const std::regex VALID_PARAM_REGEX;
    static const std::string DEFAULT_VALUE;
    static const char* const OPTION;
};

#endif /* PARAM_BUILDER_SPECIFIC_PARAMS_SCANNABLEFREQUENCY_HPP_ */
