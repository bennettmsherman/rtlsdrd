/*
 * SquelchLevel.hpp
 *
 *  Created on: Sep 2, 2017
 *      Author: Bennett Sherman
 */

#ifndef WRAPPER_SQUELCHLEVEL_HPP_
#define WRAPPER_SQUELCHLEVEL_HPP_

// System Includes
#include <stdint.h>
#include <string>

// Project Includes
#include "NumericParameter.hpp"

class SquelchLevel : public NumericParameter<uint32_t>
{
public:
    static bool isValid(const uint32_t value);
    static const SquelchLevel create(const uint32_t value=defaultValue);
    static const SquelchLevel create(const std::string& value);
    const std::string getCommand();

    static const std::string command;

protected:
    SquelchLevel(uint32_t value) : NumericParameter(value, option, minValid, maxValid) {};

private:
    static const char* const option;

    // TODO find true min and max
    static const uint32_t minValid = 0;

    static const uint32_t maxValid = 100;

    static const uint32_t defaultValue = 0;
};

#endif /* WRAPPER_SQUELCHLEVEL_HPP_ */
