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
#include "RtlFmRunner.hpp"
#include "SystemUtils.hpp"
#include "TcpServer.hpp"

class CommandParser
{
public:
    ////////////////////////////////
    // Public interface functions //
    ////////////////////////////////
    static const CommandParser& getInstance();

    std::string execute(const std::string& unparsedCommand, RtlFmParameterBuilder& rtlFmParamBuilder) const;

private:
    ///////////////////////
    // Private functions //
    ///////////////////////
    /**
     * A private constructor is required for the singleton pattern
     */
    CommandParser() = default;

    /**
     * Delete the default copy constructor
     */
    CommandParser(const CommandParser&) = delete;

    /**
     * Delete the default assignment operator
     */
    CommandParser& operator=(const CommandParser&) = delete;

    bool parse(const std::string& unparsedCommand, std::string& command, std::string& param) const;
    std::string getCommandStringList() const;

    /////////////////////////////
    // Private class Constants //
    /////////////////////////////
    static const Command<RtlFmParameterBuilder> RTL_FM_PARAMETER_BUILDER_CMDS[];
    static const Command<RtlFmRunner> RTL_FM_RUNNER_CMDS[];
    static const Command<SystemUtils> SYSTEM_UTILS_CMDS[];
    static const Command<TcpServer> SERVER_CMDS[];

    // The regex string used to parse commands
    static const std::regex CMD_REGEX;

    // The character used to separate functions from parameters
    static const std::string FUNCTION_AND_PARAM_SEPARATOR;

    // When no parameter is specified for a command, this value is used
    static const std::string UNUSED_PARAM_VALUE;

    // When this command is entered, a list of commands is returned
    static const std::string LIST_CMDS_COMMAND_STRING;

    // Lengths, in terms of the number of elements, in the different command lists
    static const size_t RTL_FM_PARAMETER_BUILDER_CMDS_LIST_LENGTH;
    static const size_t RTL_FM_RUNNER_CMDS_LIST_LENGTH;
    static const size_t SYSTEM_UTILS_CMDS_LIST_LENGTH;
    static const size_t SERVER_CMDS_LIST_LENGTH;

    // Constant strings representing different parse/execution results
    static const std::string NO_SUCH_COMMAND_EXISTS_STRING;
    static const std::string INVALID_PARAMETER_STRING;
    static const std::string EXECUTION_OK_STRING;
    static const std::string INVALID_SYNTAX_STRING;

};

#endif /* COMMAND_COMMANDPARSER_HPP_ */
