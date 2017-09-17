/*
 * StringParameter.hpp
 *
 *  Created on: Sep 3, 2017
 *      Author: Bennett Sherman
 */

#ifndef WRAPPER_STRINGPARAMETER_HPP_
#define WRAPPER_STRINGPARAMETER_HPP_

// System Includes
#include <stdint.h>
#include <string>
#include <vector>

// Project Includes
#include "BaseParameter.hpp"

class StringParameter : public BaseParameter<std::string>
{
public:
    virtual ~StringParameter() {};

protected:
    StringParameter(const std::string& value, const char* const option, const std::vector<std::string>& validValues) :
        BaseParameter(value, option), validValues(validValues) {};

    static bool isValid(const std::string& value, const std::vector<std::string>& validValues);

    static std::string getInvalidArgExceptionMessage(const std::string& value);

private:
    const std::vector<std::string>& validValues;

    static const char INVALID_ARG_EXCEPTION_MSG_FORMAT_STRING[];
};

#endif /* WRAPPER_STRINGPARAMETER_HPP_ */
