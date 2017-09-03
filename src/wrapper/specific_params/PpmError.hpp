/*
 * PpmError.hpp
 *
 *  Created on: Sep 3, 2017
 *      Author: bensherman
 */

#ifndef WRAPPER_PPMERROR_HPP_
#define WRAPPER_PPMERROR_HPP_

// System Includes
#include <stdint.h>

// Project Includes
#include "NumericParameter.hpp"

class PpmError : public NumericParameter<uint32_t>
{
public:
    static bool isValid(const uint32_t value);

    static const PpmError create(const uint32_t value=defaultValue);

protected:
    PpmError(uint32_t value) : NumericParameter(value, option, minValid, maxValid) {};

private:
    static const char option ='p';

    // TODO find true min and max
    static const uint32_t minValid = 0;

    static const uint32_t maxValid = 100000;

    static const uint32_t defaultValue = 0;
};

#endif /* WRAPPER_PPMERROR_HPP_ */
