/*
 * DeviceIndex.hpp
 *
 *  Created on: Sep 3, 2017
 *      Author: Bennett Sherman
 */

#ifndef WRAPPER_DEVICEINDEX_HPP_
#define WRAPPER_DEVICEINDEX_HPP_

// System Includes
#include <stdint.h>
#include <string>

// Project Includes
#include "NumericParameter.hpp"

class DeviceIndex : public NumericParameter<uint32_t>
{
public:
    static bool isValid(const uint32_t value);
    static const DeviceIndex create(const uint32_t value=DEFAULT_VALUE);
    static const DeviceIndex create(const std::string& value);

    const std::string getCommand();

    static const std::string COMMAND;

protected:
    DeviceIndex(uint32_t value) : NumericParameter(value, OPTION, MIN_VALID, MAX_VALID) {};

private:
    static const char* const OPTION;

    static const uint32_t MIN_VALID = 0;

    // We'll assume that at most 25 dongles can be plugged in at once.
    static const uint32_t MAX_VALID = 25;

    static const uint32_t DEFAULT_VALUE = 0;
};

#endif /* WRAPPER_DEVICEINDEX_HPP_ */
