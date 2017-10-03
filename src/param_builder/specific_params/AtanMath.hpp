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
    static const AtanMath create(const std::string& value=DEFAULT_VALUE);
    const std::string getCommand();

    static const std::string COMMAND;

protected:
    AtanMath(const std::string& value) : StringParameter(value, OPTION) {};

private:
    static const char* const OPTION;

    static const std::vector<std::string> VALID_VALUES;

    static const std::string DEFAULT_VALUE;
};

#endif /* WRAPPER_SPECIFIC_PARAMS_ATANMATH_HPP_ */
