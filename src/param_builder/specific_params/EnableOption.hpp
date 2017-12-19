/*
 * EnableOption.hpp
 *
 *  Created on: Sep 3, 2017
 *      Author: Bennett Sherman
 */

#ifndef WRAPPER_ENABLEOPTION_HPP_
#define WRAPPER_ENABLEOPTION_HPP_

// System Includes
#include <string>
#include <vector>

// Project Includes
#include "StringParameter.hpp"

class EnableOption : public StringParameter
{
public:
    EnableOption(const std::string& value = DEFAULT_VALUE);

    static bool isValid(const std::string& value);
    const std::string getCommand();

    static const std::string COMMAND;

private:
    static const char* const OPTION;

    static const std::vector<std::string> VALID_VALUES;

    static const std::string DEFAULT_VALUE;
};

#endif /* WRAPPER_ENABLEOPTION_HPP_ */
