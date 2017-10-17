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
#include "RtlFmParameterBuilder.hpp"

template <typename Commandable>
class Command
{
public:

    // Alias declarations to the rescue!
    using CommandFunction = void (Commandable::*)(const std::string&, std::string*);

    ////////////////////////////////
    // Public interface functions //
    ////////////////////////////////
    Command(std::string commandStringParam, CommandFunction cmdFuncParam, std::string descriptionParam) :
            commandString(commandStringParam), cmdFunc(cmdFuncParam), description(descriptionParam) {};

    /**
     * Executes the function pointed to by cmdFunc with the parameter funcParam.
     * cmdRef is used as the object on which cmdFunc is called.
     * updatableMsg is a string intended to be updated by the client if it wants to
     * return information to the caller.
     */
    void exec(const std::string& funcParam, std::string* updatableMsg, Commandable& cmdRef) const
    {
        (cmdRef.*cmdFunc)(funcParam, updatableMsg);
    }

    std::string getCommandString() const
    {
        return commandString;
    }

    std::string getCommandDescription() const
    {
        return description;
    }

private:
    //////////////////////////////
    // Private member variables //
    //////////////////////////////
    const std::string commandString;
    const CommandFunction cmdFunc;
    const std::string description;

};

#endif /* COMMAND_COMMAND_HPP_ */
