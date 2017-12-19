/*
 * Oversampling.hpp
 *
 *  Created on: Sep 2, 2017
 *      Author: Bennett Sherman
 */

#ifndef WRAPPER_OVERSAMPLING_HPP_
#define WRAPPER_OVERSAMPLING_HPP_

// System Includes
#include <stdint.h>
#include <string>

// Project Includes
#include "NumericParameter.hpp"

class Oversampling : public NumericParameter<uint32_t>
{
public:
    Oversampling(uint32_t value = DEFAULT_VALUE);
    Oversampling(const std::string& value) :
            Oversampling(stringToInt(value))
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

    static const uint32_t MAX_VALID = 10;

    static const uint32_t DEFAULT_VALUE = 1;
};

#endif /* WRAPPER_OVERSAMPLING_HPP_ */
