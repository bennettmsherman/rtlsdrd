/*
 * CommandParser.hpp
 *
 *  Created on: Jun 30, 2017
 *      Author: Bennett Sherman
 */

#ifndef COMMAND_COMMANDPARSER_HPP_
#define COMMAND_COMMANDPARSER_HPP_

// System includes
#include <cstdint>
#include <regex>
#include <string>

// Project includes
#include "Command.hpp"
#include "RtlFmParameterBuilder.hpp"

class CommandParser
{
public:
    ////////////////////////////////
    // Public interface functions //
    ////////////////////////////////
    CommandParser(RtlFmParameterBuilder& rtlFmWrapper) :
            rtlFmParamBuilder(rtlFmWrapper) {};

    const std::string execute(std::string& unparsedCommand);

private:
    /////////////////////////////////
    // Private interface functions //
    /////////////////////////////////
    bool parse(const std::string& unparsedCommand, std::string& command, std::string& param);
    std::string getCommandStringList();

    /////////////////////////////
    // Private class Constants //
    /////////////////////////////
    static const Command<RtlFmParameterBuilder> RTL_FM_PARAMETER_BUILDER_CMDS[];

    // The regex string used to parse commands
    static const std::regex CMD_REGEX;

    // When no parameter is specified for a command, this value is used
    static const char UNUSED_PARAM_VALUE = ' ';

    // When this command is entered, a list of commands is returned
    static const std::string LIST_CMDS_COMMAND_STRING;

    // Lengths, in terms of the number of elements, in the different command lists
    static const size_t RTL_FM_PARAMETER_BUILDER_CMDS_LIST_LENGTH;

    // Constant strings representing different parse/execution results
    static const std::string NO_SUCH_COMMAND_EXISTS_STRING;
    static const std::string INVALID_PARAMETER_STRING;
    static const std::string EXECUTION_OK_STRING;
    static const std::string INVALID_SYNTAX_STRING;

    //////////////////////////////
    // Private member variables //
    //////////////////////////////
    RtlFmParameterBuilder& rtlFmParamBuilder;
};

#endif /* COMMAND_COMMANDPARSER_HPP_ */
