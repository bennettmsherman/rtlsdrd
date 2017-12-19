/*
 * FirSize.hpp
 *
 *  Created on: Nov 5, 2017
 *      Author: bensherman
 */

#ifndef PARAM_BUILDER_SPECIFIC_PARAMS_FIRSIZE_HPP_
#define PARAM_BUILDER_SPECIFIC_PARAMS_FIRSIZE_HPP_

// System Includes
#include <string>
#include <vector>

// Project Includes
#include "StringParameter.hpp"

class FirSize : public StringParameter
{
public:
    FirSize(const std::string& value = DEFAULT_VALUE);

    static bool isValid(const std::string& value);
    const std::string getCommand();

    static const std::string COMMAND;

private:
    static const char* const OPTION;

    static const std::vector<std::string> VALID_VALUES;

    static const std::string DEFAULT_VALUE;
};

#endif /* PARAM_BUILDER_SPECIFIC_PARAMS_FIRSIZE_HPP_ */
