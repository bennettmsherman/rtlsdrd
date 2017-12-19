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
    const std::string& getValue() const
    {
        return value;
    }

    /**
     * Returns value.c_str(). Note that this pointer is only
     * valid while the string hasn't been destroyed
     * and when no non-const member functions have been called
     * since calling this function
     */
    const char* getValueCharPtr() const
    {
        return value.c_str();
    }

    /**
     * Gets the option representing this parameter.
     */
    const char* getOption() const
    {
        return option;
    }

    /**
     * Returns the command-line form of this parameter.
     * That is, the string "-<option> <value" is returned
     */
    const std::string getPopulatedCmdLineEntry() const;

    /**
     * Returns the string used to address the specified command.
     * For example, to set the frequency through the daemon,
     * "FREQUENCY" is used to create a new Frequency parameter.
     */
    virtual const std::string getCommand()
    {
        return std::string("IMPLEMENT_IN_DERIVED");
    }

    virtual ~BaseParameter()
    {
    }
    ;

protected:

    BaseParameter(const std::string& value, const char* const option) :
            value(value), option(option)
    {
    }
    ;

private:
    const std::string value;
    const char* const option;
};

#endif /* WRAPPER_BASEPARAMETER_HPP_ */
