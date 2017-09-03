/*
 * Command.hpp
 *
 *  Created on: Jun 30, 2017
 *      Author: Bennett Sherman
 */

#ifndef COMMAND_COMMAND_HPP_
#define COMMAND_COMMAND_HPP_

// Stdlib includes
#include <string>

// Project includes
#include "RtlFmWrapper.hpp"

class Command
{
public:

    // Alias declarations to the rescue!
    using WrapperFunction = void (RtlFmWrapper::*)(const std::string&);

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
    void exec(const std::string& param, RtlFmWrapper& wrapRef) const
    {
        (wrapRef.*cmdFunc)(param);
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
