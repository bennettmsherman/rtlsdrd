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

// Project Includes
#include "NumericParameter.hpp"

class SquelchDelay : public NumericParameter<int32_t>
{
public:
    static bool isValid(const int32_t value);

    static const SquelchDelay create(const int32_t value=defaultValue);

protected:
    SquelchDelay(int32_t value) : NumericParameter(value, option, minValid, maxValid) {};

private:
    static const char option ='t';

    // TODO find true min and max
    static const int32_t minValid = INT32_MIN;

    static const int32_t maxValid = INT32_MAX;

    static const int32_t defaultValue = 10;
};

#endif /* WRAPPER_SQUELCHDELAY_HPP_ */
