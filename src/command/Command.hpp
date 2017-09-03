/*
 * Command.hpp
 *
 *  Created on: Jun 30, 2017
 *      Author: Bennett Sherman
 */

#ifndef COMMAND_COMMAND_HPP_
#define COMMAND_COMMAND_HPP_

// Stdlib includes
#include <cstring>

// Project includes
#include "RDA5807M.hpp"
#include "RDA5807MWrapper.hpp"

template<typename T>
class Command
{
public:

    // Alias declarations to the rescue!
    using WrapperFunction = T (RDA5807MWrapper::*)(int);

    ////////////////////////////////
    // Public interface functions //
    ////////////////////////////////
    Command(std::string commandStringParam, WrapperFunction cmdFuncParam, std::string descriptionParam) :
            commandString(commandStringParam), cmdFunc(cmdFuncParam), description(descriptionParam) {};

    /**
     * Executes the function pointed to by cmdFunc with the parameter param.
     * wrapRef is used as the object on which cmdFunc is called.
     * The result of the function is returned
     */
    T exec(int param, RDA5807MWrapper& wrapRef) const
    {
        return (wrapRef.*cmdFunc)(param);
    }

    const std::string& getCommandString() const
    {
        return commandString;
    }

    const std::string& getCommandDescription() const
    {
        return description;
    }

private:
    //////////////////////////////
    // Private member variables //
    //////////////////////////////
    const std::string commandString;
    const WrapperFunction cmdFunc;
    const std::string description;

};

#endif /* COMMAND_COMMAND_HPP_ */
