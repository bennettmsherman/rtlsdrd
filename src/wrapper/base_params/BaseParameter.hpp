/*
 * BaseParameter.hpp
 *
 * The base for which all parameters passable to rtl_fm derive.
 *
 *  Created on: Sep 2, 2017
 *      Author: Bennett Sherman
 */

#ifndef WRAPPER_BASEPARAMETER_HPP_
#define WRAPPER_BASEPARAMETER_HPP_

// System Includes
#include <stdint.h>
#include <string>

template<typename T>
class BaseParameter
{
public:
    /**
     * Returns the value assigned to this parameter.
     */
    const T& getValue() const { return value; }

    /**
     * Gets the option representing this parameter.
     */
    const char getOption() const { return option; }

    /**
     * Returns the command-line form of this parameter.
     * That is, the string "-<option> <value" is returned
     */
    const std::string getPopulatedCmdLineEntry() const;

    virtual ~BaseParameter() {};

protected:

    BaseParameter(T value, char option) : value(value), option(option) {};

private:
    const T value;
    const char option;
};

#endif /* WRAPPER_BASEPARAMETER_HPP_ */
