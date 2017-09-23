/*
 * CommandParser.cpp
 *
 *  Created on: Jun 30, 2017
 *      Author: Bennett Sherman
 */

//System includes
#include <iostream>
#include <regex>
#include <string>
#include <stdexcept>

// Project includes
#include "Command.hpp"
#include "CommandParser.hpp"
#include "AtanMath.hpp"
#include "DeviceIndex.hpp"
#include "EnableOption.hpp"
#include "Frequency.hpp"
#include "ModulationMode.hpp"
#include "Oversampling.hpp"
#include "PpmError.hpp"
#include "ResampleRate.hpp"
#include "SampleRate.hpp"
#include "SquelchDelay.hpp"
#include "SquelchLevel.hpp"
#include "RtlFmParameterBuilder.hpp"
#include "RtlFmRunner.hpp"
#include "TunerGain.hpp"


// Static initialization
const Command<RtlFmParameterBuilder> CommandParser::RTL_FM_PARAMETER_BUILDER_CMDS[]
{
    Command<RtlFmParameterBuilder> { DeviceIndex::command, &RtlFmParameterBuilder::setDeviceIndex, "Sets the index of the RTL_SDR dongle to use"},
    Command<RtlFmParameterBuilder> { EnableOption::command, &RtlFmParameterBuilder::setEnableOption, "Enables extra options"},
    Command<RtlFmParameterBuilder> { Frequency::command, &RtlFmParameterBuilder::setFrequency, "Sets the frequency to tune"},
    Command<RtlFmParameterBuilder> { ModulationMode::command, &RtlFmParameterBuilder::setModulationMode, "Sets the modulation mode to use"},
    Command<RtlFmParameterBuilder> { Oversampling::command, &RtlFmParameterBuilder::setOversampling, "Sets the oversampling level"},
    Command<RtlFmParameterBuilder> { PpmError::command, &RtlFmParameterBuilder::setPpmError, "Sets the ppm level"},
    Command<RtlFmParameterBuilder> { SampleRate::command, &RtlFmParameterBuilder::setSampleRate, "Sets the sampling rate"},
    Command<RtlFmParameterBuilder> { SquelchDelay::command, &RtlFmParameterBuilder::setSquelchDelay, "Sets the squelch delay"},
    Command<RtlFmParameterBuilder> { SquelchLevel::command, &RtlFmParameterBuilder::setSquelchLevel, "Sets the squelch level"},
    Command<RtlFmParameterBuilder> { ResampleRate::command, &RtlFmParameterBuilder::setResampleRate, "Sets the resampling rate"},
    Command<RtlFmParameterBuilder> { AtanMath::command, &RtlFmParameterBuilder::setAtanMath, "Sets the archtangent math mode"},
    Command<RtlFmParameterBuilder> { TunerGain::command, &RtlFmParameterBuilder::setTunerGain, "Sets the tuner gain. -100 is automatic"},
    Command<RtlFmParameterBuilder> { "CLEAR", &RtlFmParameterBuilder::clearParamLists, "No param. Resets the lists of stored commands"},
    Command<RtlFmParameterBuilder> { "EXECUTE", &RtlFmParameterBuilder::executeCommand, "No param. Executes rtl_fm with the new params"},
};

const size_t CommandParser::RTL_FM_PARAMETER_BUILDER_CMDS_LIST_LENGTH = sizeof(RTL_FM_PARAMETER_BUILDER_CMDS) / sizeof(Command<RtlFmParameterBuilder>);

const std::regex CommandParser::CMD_REGEX { "^([A-Z0-9_]+)=?([-]?[0-9]*|[a-zA-Z]*)$"};
const std::string CommandParser::LIST_CMDS_COMMAND_STRING {"HELP"};
const std::string CommandParser::INVALID_SYNTAX_STRING {"INVALID COMMAND SYNTAX"};
const std::string CommandParser::NO_SUCH_COMMAND_EXISTS_STRING {"NO SUCH COMMAND EXISTS"};
const std::string CommandParser::INVALID_PARAMETER_STRING {"INVALID PARAMETER"};
const std::string CommandParser::EXECUTION_OK_STRING {"OK"};

/**
 * The command specified by LIST_CMDS_COMMAND_STRING will result in a list of supported
 * functions being printed. If the entered command is invalid, "COMMAND NOT VALID!" is returned.
 * Otherwise, the result of the execution is returned.
 */
const std::string CommandParser::execute(std::string& unparsedCommand)
{
    std::string cmd;
    std::string param;

    bool parseResult = parse(unparsedCommand, cmd, param);
    if (!parseResult)
    {
        return INVALID_SYNTAX_STRING;
    }

    if (cmd.compare(LIST_CMDS_COMMAND_STRING) == 0)
    {
        return getCommandStringList();
    }

    try
    {
        for (size_t idx = 0; idx < RTL_FM_PARAMETER_BUILDER_CMDS_LIST_LENGTH; ++idx)
        {
            Command<RtlFmParameterBuilder> statusResultCmd = RTL_FM_PARAMETER_BUILDER_CMDS[idx];
            if (cmd.compare(statusResultCmd.getCommandString()) == 0)
            {
                std::cout << "Executing: " << cmd << "(" << param << ")" << std::endl;
                statusResultCmd.exec(param, rtlFmParamBuilder);
                return EXECUTION_OK_STRING;
            }
        }
    }
    catch (const std::logic_error& err)
    {
        return INVALID_PARAMETER_STRING + "; " + err.what();
    }

    return NO_SUCH_COMMAND_EXISTS_STRING;
}

/**
 * Returns a list of commands supported by this interpreter.
 */
std::string CommandParser::getCommandStringList()
{
    std::string cmdList = "SUPPORTED COMMANDS:\n";

    cmdList.append("RTL FM PARAMETER BUILDER COMMANDS: \n");
    for (size_t idx = 0; idx < RTL_FM_PARAMETER_BUILDER_CMDS_LIST_LENGTH; ++idx)
    {
        cmdList.append(RTL_FM_PARAMETER_BUILDER_CMDS[idx].getCommandString());
        cmdList.append(" - ");
        cmdList.append(RTL_FM_PARAMETER_BUILDER_CMDS[idx].getCommandDescription());
        cmdList.append("\n");
    }

    return cmdList;
}

/**
 * Commands with params are of the form:
 * <COMMAND>=<VAL>, where <VAL> is a positive or negative number,
 * or is a string.
 *
 * Commands with no params are of the form:
 * <COMMAND>
 * and " " is used as the value
 *
 * Returns true if a valid command is found, false otherwise
 */
bool CommandParser::parse(const std::string& unparsedCommand, std::string& command, std::string& param)
{
    // Find the command and value if they exist
    std::smatch matches;

    std::regex_match(unparsedCommand, matches, CMD_REGEX);
    if (matches.size() != 3)
    {
        return false;
    }

    // matches[0] should be the whole string
    if (unparsedCommand.compare(matches[0].str()) != 0)
    {
        return false;
    }

    command = matches[1].str();

    // if matches[2] is not present, it means that this is a no-param command
    if (matches[2].length() == 0)
    {
        param = UNUSED_PARAM_VALUE;
    }
    else
    {
        param = matches[2].str();
    }

    return true;
}
