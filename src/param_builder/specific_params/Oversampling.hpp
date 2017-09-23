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
    static bool isValid(const uint32_t value);
    static const Oversampling create(const uint32_t value=defaultValue);
    static const Oversampling create(const std::string& value);
    const std::string getCommand();

    static const std::string command;
protected:
    Oversampling(uint32_t value) : NumericParameter(value, option, minValid, maxValid) {};

private:
    static const char* const option;

    // TODO find true min and max
    static const uint32_t minValid = 0;

    static const uint32_t maxValid = 10;

    static const uint32_t defaultValue = 1;
};

#endif /* WRAPPER_OVERSAMPLING_HPP_ */
