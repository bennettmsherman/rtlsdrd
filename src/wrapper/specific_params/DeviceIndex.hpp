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

    static const DeviceIndex create(const uint32_t value=defaultValue);
    static const DeviceIndex create(const std::string& value);

protected:
    DeviceIndex(uint32_t value) : NumericParameter(value, option, minValid, maxValid) {};

private:
    static const char* const option;

    static const uint32_t minValid = 0;

    // We'll assume that at most 25 dongles can be plugged in at once.
    static const uint32_t maxValid = 25;

    static const uint32_t defaultValue = 0;
};

#endif /* WRAPPER_DEVICEINDEX_HPP_ */
