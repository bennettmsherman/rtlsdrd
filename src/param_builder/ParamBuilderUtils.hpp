/*
 * ParamBuilderUtils.hpp
 *
 *  Created on: Sep 28, 2017
 *      Author: bensherman
 */

#ifndef PARAM_BUILDER_PARAMBUILDERUTILS_HPP_
#define PARAM_BUILDER_PARAMBUILDERUTILS_HPP_

// System Includes
#include <cstdint>
#include <string>

namespace ParamBuilderUtils
{
    uint32_t broadcastAmKilohertzToHertz(const std::string& integerKilohertz);
    uint32_t broadcastFmMegahertzToHertz(const std::string& floatMegahertz);
}

#endif /* PARAM_BUILDER_PARAMBUILDERUTILS_HPP_ */
