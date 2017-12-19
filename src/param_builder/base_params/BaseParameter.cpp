/*
 * BaseParameter.cpp
 *
 *  Created on: Sep 2, 2017
 *      Author: Bennett Sherman
 */

// System Includes
#include <stdint.h>
#include <string>

// Project Includes
#include "BaseParameter.hpp"

// Explicit template instantiation
template class BaseParameter<int32_t> ;
template class BaseParameter<uint32_t> ;
template class BaseParameter<std::string> ;

/**
 * Returns the command-line form of this parameter.
 * That is, the string "-<option> <value" is returned
 */
template<typename T>
const std::string BaseParameter<T>::getPopulatedCmdLineEntry() const
{
    std::string populated;
    populated += option;
    populated += " ";
    populated += value;
    return populated;
}
