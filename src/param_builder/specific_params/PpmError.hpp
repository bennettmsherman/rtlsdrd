/*
 * PpmError.hpp
 *
 *  Created on: Sep 3, 2017
 *      Author: Bennett Sherman
 */

#ifndef WRAPPER_PPMERROR_HPP_
#define WRAPPER_PPMERROR_HPP_

// System Includes
#include <stdint.h>
#include <string>

// Project Includes
#include "NumericParameter.hpp"

class PpmError : public NumericParameter<uint32_t>
{
public:
    PpmError(uint32_t value = DEFAULT_VALUE);
    PpmError(const std::string& value) :
            PpmError(stringToInt(value))
    {
    }
    ;

    static bool isValid(const uint32_t value);
    const std::string getCommand();

    static const std::string COMMAND;

private:
    static const char* const OPTION;

    // TODO find true min and max
    static const uint32_t MIN_VALID = 0;

    static const uint32_t MAX_VALID = 100000;

    static const uint32_t DEFAULT_VALUE = 0;
};

#endif /* WRAPPER_PPMERROR_HPP_ */
