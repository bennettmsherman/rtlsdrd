/*
 * CommandParser.hpp
 *
 *  Created on: Jun 30, 2017
 *      Author: bensherman
 */

#ifndef COMMAND_COMMANDPARSER_HPP_
#define COMMAND_COMMANDPARSER_HPP_

// System includes
#include <cstdint>
#include <regex>
#include <string>

// Project includes
#include "Command.hpp"
#include "RDA5807M.hpp"

class CommandParser
{
public:
    ////////////////////////////////
    // Public interface functions //
    ////////////////////////////////
    CommandParser(RDA5807MWrapper& radioWrapperParam) :
            radioWrapper(radioWrapperParam) {};

    std::string execute(std::string& unparsedCommand);

private:
    /////////////////////////////////
    // Private interface functions //
    /////////////////////////////////
    bool parse(const std::string& unparsedCommand, std::string& command, int& param);
    std::string getCommandStringList();

    /////////////////////////////
    // Private class Constants //
    /////////////////////////////
    static const Command<RDA5807M::StatusResult> STATUS_RESULT_COMMANDS[];
    static const Command<std::string> STRING_RESULT_COMMANDS[];
    static const Command<uint32_t> UINT32_RESULT_COMMANDS[];

    // The regex string used to parse commands
    static const std::regex CMD_REGEX;

    // When no parameter is specified for a command, this value is used
    static const int UNUSED_PARAM_VALUE = -1;

    // When this command is entered, a list of commands is returned
    static const std::string LIST_CMDS_COMMAND_STRING;

    // Lengths, in terms of the number of elements, in the different command lists
    static const size_t STATUS_RESULT_COMMANDS_LIST_LENGTH;
    static const size_t STRING_RESULT_COMMANDS_LIST_LENGTH;
    static const size_t UINT32_RESULT_COMMANDS_LIST_LENGTH;

    //////////////////////////////
    // Private member variables //
    //////////////////////////////
    RDA5807MWrapper& radioWrapper;
};

#endif /* COMMAND_COMMANDPARSER_HPP_ */
