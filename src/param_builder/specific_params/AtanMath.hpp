/*
 * AtanMath.hpp
 *
 *  Created on: Sep 6, 2017
 *      Author: bensherman
 */

#ifndef WRAPPER_SPECIFIC_PARAMS_ATANMATH_HPP_
#define WRAPPER_SPECIFIC_PARAMS_ATANMATH_HPP_

// System Includes
#include <string>
#include <vector>

// Project Includes
#include "StringParameter.hpp"

class AtanMath : public StringParameter
{
public:
    static bool isValid(const std::string& value);

    static const AtanMath create(const std::string& value=defaultValue);

protected:
    AtanMath(const std::string& value) : StringParameter(value, option, validValues) {};

private:
    static const char* const option;

    static const std::vector<std::string> validValues;

    static const std::string defaultValue;
};

#endif /* WRAPPER_SPECIFIC_PARAMS_ATANMATH_HPP_ */
