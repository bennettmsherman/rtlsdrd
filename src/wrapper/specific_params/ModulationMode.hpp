/*
 * ModulationMode.hpp
 *
 *  Created on: Sep 3, 2017
 *      Author: bensherman
 */

#ifndef WRAPPER_MODULATIONMODE_HPP_
#define WRAPPER_MODULATIONMODE_HPP_

// System Includes
#include <string>
#include <vector>

// Project Includes
#include "StringParameter.hpp"

class ModulationMode : public StringParameter
{
public:
    static bool isValid(const std::string& value);

    static const ModulationMode create(const std::string& value=defaultValue);

protected:
    ModulationMode(const std::string& value) : StringParameter(value, option, validValues) {};

private:
    static const char option ='M';

    static const std::vector<std::string> validValues;

    static const std::string defaultValue;
};

#endif /* WRAPPER_MODULATIONMODE_HPP_ */
