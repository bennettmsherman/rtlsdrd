/*
 * Frequency.hpp
 *
 *  Created on: Sep 2, 2017
 *      Author: Bennett Sherman
 */

#ifndef WRAPPER_FREQUENCY_HPP_
#define WRAPPER_FREQUENCY_HPP_

// System Includes
#include <stdint.h>
#include <string>

// Project Includes
#include "NumericParameter.hpp"

class Frequency : public NumericParameter<uint32_t>
{
public:
    static bool isValid(const uint32_t value);
    static const Frequency create(const uint32_t value=defaultValue);
    static const Frequency create(const std::string& value);
    const std::string getCommand();

    static const std::string command;

protected:
    Frequency(uint32_t value) : NumericParameter(value, option, minValid, maxValid) {};

private:
    static const char* const option;

    // TODO find true min and max
    static const uint32_t minValid = 0;

    // 2GHz
    static const uint32_t maxValid = 2000000000;

    // 91.1MHz
    static const uint32_t defaultValue = 91100000;
};

#endif /* WRAPPER_FREQUENCY_HPP_ */
