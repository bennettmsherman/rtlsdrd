/*
 * SquelchDelay.hpp
 *
 *  Created on: Sep 3, 2017
 *      Author: Bennett Sherman
 */

#ifndef WRAPPER_SQUELCHDELAY_HPP_
#define WRAPPER_SQUELCHDELAY_HPP_

// System Includes
#include <stdint.h>
#include <string>

// Project Includes
#include "NumericParameter.hpp"

class SquelchDelay : public NumericParameter<int32_t>
{
public:
    static bool isValid(const int32_t value);
    static const SquelchDelay create(const int32_t value=DEFAULT_VALUE);
    static const SquelchDelay create(const std::string& value);
    const std::string getCommand();

    static const std::string COMMAND;

protected:
    SquelchDelay(int32_t value) : NumericParameter(value, OPTION, MIN_VALID, MAX_VALID) {};

private:
    static const char* const OPTION;

    // TODO find true min and max
    static const int32_t MIN_VALID = INT32_MIN;

    static const int32_t MAX_VALID = INT32_MAX;

    static const int32_t DEFAULT_VALUE = 10;
};

#endif /* WRAPPER_SQUELCHDELAY_HPP_ */
