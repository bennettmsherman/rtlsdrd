/*
 * ModulationMode.hpp
 *
 *  Created on: Sep 3, 2017
 *      Author: Bennett Sherman
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
    static const ModulationMode create(const std::string& value=DEFAULT_VALUE);
    const std::string getCommand();

    static const std::string COMMAND;

protected:
    ModulationMode(const std::string& value) : StringParameter(value, OPTION) {};

private:
    static const char* const OPTION;

    static const std::vector<std::string> VALID_VALUES;

    static const std::string DEFAULT_VALUE;
};

#endif /* WRAPPER_MODULATIONMODE_HPP_ */
