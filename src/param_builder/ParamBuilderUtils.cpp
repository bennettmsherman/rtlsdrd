/*
 * ParamBuilderUtils.cpp
 *
 *  Created on: Sep 28, 2017
 *      Author: bensherman
 */

// System Includes
#include <string>
#include <cstdint>

// Project Includes
#include "ParamBuilderUtils.hpp"

/**
 * Converts an AM broadcast station's kHz (integer) frequency from a string to
 * a hz-unit uint32_t
 */
uint32_t ParamBuilderUtils::broadcastAmKilohertzToHertz(const std::string& integerKilohertz)
{
    return (std::stoul(integerKilohertz) * 1000);
}

/**
 * Converts the parameter, which is an AM broadcast station's frequency
 * in the form "XX.X" or "XX" to a from a string to a uint32_t with a
 * hertz unit.
 */
uint32_t ParamBuilderUtils::broadcastFmMegahertzToHertz(const std::string& floatMegahertz)
{
    // Multiplying by 10 will remove any unwanted digits
    // Then, multiply by 1*10^5 to finish the Hz conversion
    return (static_cast<uint32_t>(std::stod(floatMegahertz) * 10) * 100000);
}
