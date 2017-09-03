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
    static bool isValid(const std::string& value);

    static const EnableOption create(const std::string& value=defaultValue);

protected:
    EnableOption(const std::string& value) : StringParameter(value, option, validValues) {};

private:
    static const char option ='E';

    static const std::vector<std::string> validValues;

    static const std::string defaultValue;
};

#endif /* WRAPPER_ENABLEOPTION_HPP_ */
