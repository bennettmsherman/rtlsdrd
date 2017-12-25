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
#include "AtanMath.hpp"
#include "Command.hpp"
#include "CommandParser.hpp"
#include "DeviceIndex.hpp"
#include "EnableOption.hpp"
#include "FirSize.hpp"
#include "Frequency.hpp"
#include "ModulationMode.hpp"
#include "Oversampling.hpp"
#include "PpmError.hpp"
#include "ResampleRate.hpp"
#include "RtlFmParameterBuilder.hpp"
#include "RtlFmRunner.hpp"
#include "SampleRate.hpp"
#include "ScannableFrequency.hpp"
#include "SquelchDelay.hpp"
#include "SquelchLevel.hpp"
#include "SystemUtils.hpp"
#include "SystemUtils.hpp"
#include "TcpServer.hpp"
#include "TunerGain.hpp"

// Static initialization
const Command<RtlFmParameterBuilder> CommandParser::RTL_FM_PARAMETER_BUILDER_CMDS[]
{
    Command<RtlFmParameterBuilder> { DeviceIndex::COMMAND, &RtlFmParameterBuilder::setDeviceIndex, "Sets the index of the RTL_SDR dongle to use"},
    Command<RtlFmParameterBuilder> { EnableOption::COMMAND, &RtlFmParameterBuilder::setEnableOption, "Enables extra options"},
    Command<RtlFmParameterBuilder> { FirSize::COMMAND, &RtlFmParameterBuilder::setFirSize, "Sets the high quality FIR downsampler. -1 = off, 0 or 9 enable it"},
	Command<RtlFmParameterBuilder> { Frequency::COMMAND, &RtlFmParameterBuilder::setFrequency, "Sets the frequency to tune"},
    Command<RtlFmParameterBuilder> { ModulationMode::COMMAND, &RtlFmParameterBuilder::setModulationMode, "Sets the modulation mode to use"},
    Command<RtlFmParameterBuilder> { Oversampling::COMMAND, &RtlFmParameterBuilder::setOversampling, "Sets the oversampling level"},
    Command<RtlFmParameterBuilder> { PpmError::COMMAND, &RtlFmParameterBuilder::setPpmError, "Sets the ppm level"},
    Command<RtlFmParameterBuilder> { SampleRate::COMMAND, &RtlFmParameterBuilder::setSampleRate, "Sets the sampling rate"},
    Command<RtlFmParameterBuilder> { SquelchDelay::COMMAND, &RtlFmParameterBuilder::setSquelchDelay, "Sets the squelch delay"},
    Command<RtlFmParameterBuilder> { SquelchLevel::COMMAND, &RtlFmParameterBuilder::setSquelchLevel, "Sets the squelch level"},
    Command<RtlFmParameterBuilder> { ResampleRate::COMMAND, &RtlFmParameterBuilder::setResampleRate, "Sets the resampling rate"},
    Command<RtlFmParameterBuilder> { AtanMath::COMMAND, &RtlFmParameterBuilder::setAtanMath, "Sets the archtangent math mode"},
    Command<RtlFmParameterBuilder> { TunerGain::COMMAND, &RtlFmParameterBuilder::setTunerGain, "Sets the tuner gain. -100 is automatic"},
    Command<RtlFmParameterBuilder> { "BROADCAST_AM", &RtlFmParameterBuilder::broadcastAmStationMacro, "Input: AM broadcast station's freq in kHz."
                                        " Sets sample rate = 128k, enables: direct sampling, AM demodulation"},
    Command<RtlFmParameterBuilder> { "BROADCAST_FM", &RtlFmParameterBuilder::broadcastFmStationMacro, "Input: FM broadcast station's freq in MHz."
                                        " Sets wbfm demodulation."},
    Command<RtlFmParameterBuilder> { ScannableFrequency::COMMAND, &RtlFmParameterBuilder::setScannableFrequency,
                                            "Scans a frequency range. Format is START:END:INCREMENT. SquelchLevel must be set to use this"},
    Command<RtlFmParameterBuilder> { "CLEAR", &RtlFmParameterBuilder::clearParamLists, "No param. Resets the lists of stored commands"},
    Command<RtlFmParameterBuilder> { "EXECUTE", &RtlFmParameterBuilder::executeCommand, "No param. Executes rtl_fm with the new params"},
    Command<RtlFmParameterBuilder> { "MY_STORED_CMDS", &RtlFmParameterBuilder::getUserProvidedCommands, "Returns all of the commands entered by this user after "
                                        "having executed CLEAR or EXECUTE"}
};

const Command<RtlFmRunner> CommandParser::RTL_FM_RUNNER_CMDS[]
{
    Command<RtlFmRunner> { "STOP", &RtlFmRunner::stopCommandHandler, "Kills rtl_fm and aplay. Stops the audio stream."},
    Command<RtlFmRunner> { "CMDS_IN_USE", &RtlFmRunner::getUserProvidedCommandsInUse, "Returns all user provided commands in use for the current invocation of"
                                "rtl_fm and aplay"}
};

const Command<SystemUtils> CommandParser::SYSTEM_UTILS_CMDS[]
{
    Command<SystemUtils> { SystemUtils::VOLUME_SETTER_COMMAND, &SystemUtils::setVolumeCommandHandler, "Sets the system volume. Specify as a percentage."}
};

const Command<TcpServer> CommandParser::SERVER_CMDS[]
{
    Command<TcpServer> { "SVR_INFO", &TcpServer::getAddressInfoHandler, "Gets the server's networking information"},
    Command<TcpServer> { "CLIENTS_INFO", &TcpServer::getClientsInfoHandler, "Gets the IP and port #s of connected clients"}
};

const size_t CommandParser::RTL_FM_PARAMETER_BUILDER_CMDS_LIST_LENGTH = sizeof(RTL_FM_PARAMETER_BUILDER_CMDS) / sizeof(Command<RtlFmParameterBuilder>);
const size_t CommandParser::RTL_FM_RUNNER_CMDS_LIST_LENGTH = sizeof(RTL_FM_RUNNER_CMDS) / sizeof(Command<RtlFmRunner>);
const size_t CommandParser::SYSTEM_UTILS_CMDS_LIST_LENGTH = sizeof(SYSTEM_UTILS_CMDS) / sizeof(Command<SystemUtils>);
const size_t CommandParser::SERVER_CMDS_LIST_LENGTH = sizeof(SERVER_CMDS) / sizeof(Command<TcpServer>);

const std::string CommandParser::FUNCTION_AND_PARAM_SEPARATOR = "=";
const std::regex CommandParser::CMD_REGEX { "^([A-Z0-9_]+)" + FUNCTION_AND_PARAM_SEPARATOR + "?([-]?[0-9a-zA-Z:]*\\.?[0-9a-zA-Z:]*)"};
const std::string CommandParser::LIST_CMDS_COMMAND_STRING {"HELP"};
const std::string CommandParser::INVALID_SYNTAX_STRING {"~INVALID COMMAND SYNTAX"};
const std::string CommandParser::NO_SUCH_COMMAND_EXISTS_STRING {"~NO SUCH COMMAND EXISTS"};
const std::string CommandParser::INVALID_PARAMETER_STRING {"~INVALID PARAMETER"};
const std::string CommandParser::EXECUTION_OK_STRING {"~OK"};
const std::string CommandParser::UNUSED_PARAM_VALUE = "default";

/**
 * The command specified by LIST_CMDS_COMMAND_STRING will result in a list of supported
 * functions being printed. If the command was invalid or there was a parsing error, a
 * string reporting the error will be returned. If the parse operation was successful,
 * the result of the execution will be returned.
 *
 * TODO: Throw exceptions for bad commands
 */
std::string CommandParser::execute(const std::string& unparsedCommand, RtlFmParameterBuilder& rtlFmParamBuilder) const
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
        return EXECUTION_OK_STRING + getCommandStringList();
    }

    // Attempt to find and execute the specified command
    try
    {
        std::string funcUpdatableString = EXECUTION_OK_STRING;
        // Parameter builder commands
        for (size_t idx = 0; idx < RTL_FM_PARAMETER_BUILDER_CMDS_LIST_LENGTH; ++idx)
        {
            Command<RtlFmParameterBuilder> rtlFmParamBuilderCmd = RTL_FM_PARAMETER_BUILDER_CMDS[idx];
            if (cmd.compare(rtlFmParamBuilderCmd.getCommandString()) == 0)
            {
                std::cout << "Executing: " << cmd << "(" << param << ")" << std::endl;
                rtlFmParamBuilderCmd.exec(param, &funcUpdatableString, rtlFmParamBuilder);

                // RtlFmParamBuilder commands w/o parameters are not added
                // to this list
                if (param != UNUSED_PARAM_VALUE)
                {
                    rtlFmParamBuilder.storeUserEnteredCommand(cmd, FUNCTION_AND_PARAM_SEPARATOR, param);
                }

                return funcUpdatableString;
            }
        }

        // RtlFmRunner commands
        for (size_t idx = 0; idx < RTL_FM_RUNNER_CMDS_LIST_LENGTH; ++idx)
        {
            Command<RtlFmRunner> rtlFmRunnerCmd = RTL_FM_RUNNER_CMDS[idx];
            if (cmd.compare(rtlFmRunnerCmd.getCommandString()) == 0)
            {
                std::cout << "Executing: " << cmd << "(" << param << ")" << std::endl;
                rtlFmRunnerCmd.exec(param, &funcUpdatableString, RtlFmRunner::getInstance());
                return funcUpdatableString;
            }
        }

        // SystemUtils commands
        for (size_t idx = 0; idx < SYSTEM_UTILS_CMDS_LIST_LENGTH; ++idx)
        {
            Command<SystemUtils>systemUtilsCmd = SYSTEM_UTILS_CMDS[idx];
            if (cmd.compare(systemUtilsCmd.getCommandString()) == 0)
            {
                std::cout << "Executing: " << cmd << "(" << param << ")" << std::endl;
                systemUtilsCmd.exec(param, &funcUpdatableString, SystemUtils::getInstance());
                return funcUpdatableString;
            }
        }

        // Server commands
        for (size_t idx = 0; idx < SERVER_CMDS_LIST_LENGTH; ++idx)
        {
            Command<TcpServer>serverCmd = SERVER_CMDS[idx];
            if (cmd.compare(serverCmd.getCommandString()) == 0)
            {
                std::cout << "Executing: " << cmd << "(" << param << ")" << std::endl;
                serverCmd.exec(param, &funcUpdatableString, TcpServer::getInstance());
                return funcUpdatableString;
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
std::string CommandParser::getCommandStringList() const
{
    std::string cmdList = "\nSUPPORTED COMMANDS:\n";

    cmdList.append("RTL FM PARAMETER BUILDER COMMANDS: \n");
    for (size_t idx = 0; idx < RTL_FM_PARAMETER_BUILDER_CMDS_LIST_LENGTH; ++idx)
    {
        cmdList.append(RTL_FM_PARAMETER_BUILDER_CMDS[idx].getCommandString());
        cmdList.append(" - ");
        cmdList.append(RTL_FM_PARAMETER_BUILDER_CMDS[idx].getCommandDescription());
        cmdList.append("\n");
    }

    cmdList.append("\nRTL FM RUNNER COMMANDS: \n");
    for (size_t idx = 0; idx < RTL_FM_RUNNER_CMDS_LIST_LENGTH; ++idx)
    {
        cmdList.append(RTL_FM_RUNNER_CMDS[idx].getCommandString());
        cmdList.append(" - ");
        cmdList.append(RTL_FM_RUNNER_CMDS[idx].getCommandDescription());
        cmdList.append("\n");
    }

    cmdList.append("\nSYSTEM UTILS COMMANDS: \n");
    for (size_t idx = 0; idx < SYSTEM_UTILS_CMDS_LIST_LENGTH; ++idx)
    {
        cmdList.append(SYSTEM_UTILS_CMDS[idx].getCommandString());
        cmdList.append(" - ");
        cmdList.append(SYSTEM_UTILS_CMDS[idx].getCommandDescription());
        cmdList.append("\n");
    }

    cmdList.append("\nSERVER COMMANDS: \n");
    for (size_t idx = 0; idx < SERVER_CMDS_LIST_LENGTH; ++idx)
    {
        cmdList.append(SERVER_CMDS[idx].getCommandString());
        cmdList.append(" - ");
        cmdList.append(SERVER_CMDS[idx].getCommandDescription());
        cmdList.append("\n");
    }

    return cmdList;
}

/**
 * Commands with params are of the form:
 * <COMMAND>=<VAL>, where <VAL> is a positive or negative number
 * (decimal allowed), or is a string.
 *
 * Commands with no params are of the form:
 * <COMMAND>
 * and " " is used as the value
 *
 * Returns true if a valid command is found, false otherwise
 */
bool CommandParser::parse(const std::string& unparsedCommand, std::string& command, std::string& param) const
{
    // Find the command and value if they exist
    std::smatch matches;

    std::regex_search(unparsedCommand, matches, CMD_REGEX);

    if (matches.size() != 3)
    {
        return false;
    }

    // matches[0] should be the whole string, minus any trailing whitespace
    if (unparsedCommand.substr(0, unparsedCommand.find_first_of("\r\n")).
            compare(matches[0].str()) != 0)
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

/**
 * Returns a reference to the singleton CommandParser instance
 */
const CommandParser& CommandParser::getInstance()
{
    static CommandParser instance{};
    return instance;
}

