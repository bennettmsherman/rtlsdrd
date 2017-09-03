/*
 * CommandParser.cpp
 *
 *  Created on: Jun 30, 2017
 *      Author: bensherman
 */

//System includes
#include <iostream>
#include <regex>
#include <string>

// Project includes
#include "Command.hpp"
#include "CommandParser.hpp"
#include "RDA5807M.hpp"
#include "RDA5807MWrapper.hpp"

// Static initialization
const Command<RDA5807M::StatusResult> CommandParser::STATUS_RESULT_COMMANDS[] =
{
    Command<RDA5807M::StatusResult> { "FREQ", &RDA5807MWrapper::setFrequency, "Enter freq in as an integer"},
    Command<RDA5807M::StatusResult> { "VOL", &RDA5807MWrapper::setVolume, "Enter volume from 0 to 15"},
    Command<RDA5807M::StatusResult> { "MUTE", &RDA5807MWrapper::setMute, "1 to mute, 0 to unmute"},
    Command<RDA5807M::StatusResult> { "BASSBOOST", &RDA5807MWrapper::setBassBoost, "1 to enable bass boost, 0 to disable"},
    Command<RDA5807M::StatusResult> { "ENABLE", &RDA5807MWrapper::setRadioEnableState, "1 to enable/reset radio, 0 to disable"},
    Command<RDA5807M::StatusResult> { "HIGHIMPEDANCEOUTPUT", &RDA5807MWrapper::setHighImpedanceOutput, "1 to enable HiZ output, 0 to disable. Unstable - avoid"},
    Command<RDA5807M::StatusResult> { "STEREO", &RDA5807MWrapper::setStereo, "1 for stereo, 0 for mono"},
    Command<RDA5807M::StatusResult> { "SEEK", &RDA5807MWrapper::setSeek, "1 to enable seeking, 0 to disable. If 1, other commands may cause seeking, so be sure to set back to 0"},
    Command<RDA5807M::StatusResult> { "RDS", &RDA5807MWrapper::setRDS, "1 to enable RDS, 0 to disable"},
    Command<RDA5807M::StatusResult> { "NEWMETHOD", &RDA5807MWrapper::setNewMethod, "1 to enable new demodulate method, 0 to disable. Strongly recommend using 1 permanently"},
    Command<RDA5807M::StatusResult> { "SOFTRESET", &RDA5807MWrapper::setSoftReset, "Setting to 1 causes a soft reset"},
    Command<RDA5807M::StatusResult> { "SOFTMUTE", &RDA5807MWrapper::setSoftMute, "1 to enable softmute, 0 to disable"},
    Command<RDA5807M::StatusResult> { "TUNE", &RDA5807MWrapper::setTune, "1 to tune. Resets to 0 automatically after tune complete"},
    Command<RDA5807M::StatusResult> { "AFCD", &RDA5807MWrapper::setAFCD, "1 toe enable automatic frequency control (AFC), 0 tod disable"},
    Command<RDA5807M::StatusResult> { "DEEMPHASIS", &RDA5807MWrapper::setDeEmphasis, "SEVENTY_FIVE_US = 0, FIFTY_US = 1"},
    Command<RDA5807M::StatusResult> { "BAND", &RDA5807MWrapper::setBand, "US_EUR = 0, JAP = 1, WORLD_WIDE = 2, EAST_EUROPE= 3"},
    Command<RDA5807M::StatusResult> { "CHANNELSPACING", &RDA5807MWrapper::setChannelSpacing, "E_HUND_KHZ = 0, TWO_HUND_KHZ = 1, FIFTY_KHZ = 2, TWENTY_FIVE_KHZ = 3"},
    Command<RDA5807M::StatusResult> { "SEEKDIR", &RDA5807MWrapper::setSeekDirection, "SEEK_UP = 0, SEEK_DOWN = 1"},
    Command<RDA5807M::StatusResult> { "SEEKMODE", &RDA5807MWrapper::setSeekMode, "WRAP_AT_LIMIT = 0, STOP_AT_LIMIT = 1"},
    Command<RDA5807M::StatusResult> { "SOFTBLEND", &RDA5807MWrapper::setSoftBlend, "1 to enable soft blend, 0 to disable" },
    Command<RDA5807M::StatusResult> { "UPDATELOCALREGS", &RDA5807MWrapper::updateLocalRegisterMapFromDevice, "No param. Updates local regmap with regs from device"}
};

const Command<std::string> CommandParser::STRING_RESULT_COMMANDS[] =
{
    Command<std::string> { "STATUS", &RDA5807MWrapper::getStatusString, "No param. Prints status info"},
    Command<std::string> { "REGMAP", &RDA5807MWrapper::getRegisterMapString, "No param. Prints local register map"},
    Command<std::string> { "FREQMAP" , &RDA5807MWrapper::generateFreqMap, "Prints dotplot of freqs and their RSSI. No param for short search. Param=1 shows RDS support (takes a long time)"},
    Command<std::string> { "RDSINFO" , &RDA5807MWrapper::getRdsInfoString, "No param. Prints RDS information"},
    Command<std::string> { "GETREGFROMLOCALMAP", &RDA5807MWrapper::getLocalCopyOfReg, "Returns the local copy of the register addressed by the param (in hex)"},
    Command<std::string> { "SNOOPRDSGROUP2", &RDA5807MWrapper::snoopRdsGroupTwo, "Snoops RDS group 2 for param (in ms) milliseconds at 10ms intervals"}
};

const Command<uint32_t> CommandParser::UINT32_RESULT_COMMANDS[] =
{
    Command<uint32_t> { "RSSI", &RDA5807MWrapper::getRssi, "No param. Prints the RSSI"},
    Command<uint32_t> { "RDSPI", &RDA5807MWrapper::getRdsPiCode, "No param. Prints RDS program identification code"},
    Command<uint32_t> { "RDSGROUPTYPE", &RDA5807MWrapper::getRdsGroupTypeCode, "No param. Prints RDS group type"},
    Command<uint32_t> { "RDSVERSION", &RDA5807MWrapper::getRdsVersionCode, "No param. Prints RDS version code"},
    Command<uint32_t> { "RDSTRAFPROGRAMID", &RDA5807MWrapper::getRdsTrafficProgramIdCode, "No param. Prints RDS traffic ID code"},
    Command<uint32_t> { "RDSPROGRAMTYPE", &RDA5807MWrapper::getRdsProgramTypeCode, "No param. Prints RDS program type code"}
};

const size_t CommandParser::STATUS_RESULT_COMMANDS_LIST_LENGTH = sizeof(STATUS_RESULT_COMMANDS) / sizeof(Command<RDA5807M::StatusResult>);
const size_t CommandParser::STRING_RESULT_COMMANDS_LIST_LENGTH = sizeof(STRING_RESULT_COMMANDS) / sizeof(Command<std::string>);
const size_t CommandParser::UINT32_RESULT_COMMANDS_LIST_LENGTH = sizeof(UINT32_RESULT_COMMANDS) / sizeof(Command<uint32_t>);

const std::regex CommandParser::CMD_REGEX { "^([a-zA-Z0-9]+){1}=*([0-9]*)"};
const std::string CommandParser::LIST_CMDS_COMMAND_STRING = "HELP";

/**
 * The command specified by LIST_CMDS_COMMAND_STRING will result in a list of supported
 * functions being printed. If the entered command is invalid, "COMMAND NOT VALID!" is returned.
 * Otherwise, the result of the execution is returned.
 */
std::string CommandParser::execute(std::string& unparsedCommand)
{
    std::string cmd;
    int param;

    bool parseResult = parse(unparsedCommand, cmd, param);
    if (!parseResult)
    {
        return "COMMAND NOT VALID!";
    }

    if (cmd.compare(LIST_CMDS_COMMAND_STRING) == 0)
    {
        return getCommandStringList();
    }

    for (size_t idx = 0; idx < STATUS_RESULT_COMMANDS_LIST_LENGTH; ++idx)
    {
        Command<RDA5807M::StatusResult> statusResultCmd = STATUS_RESULT_COMMANDS[idx];
        if (cmd.compare(statusResultCmd.getCommandString()) == 0)
        {
            std::cout << "Executing: " << cmd << "(" << param << ")" << std::endl;
            return RDA5807M::statusResultToString(statusResultCmd.exec(param, radioWrapper));
        }
    }

    for (size_t idx = 0; idx < STRING_RESULT_COMMANDS_LIST_LENGTH; ++idx)
    {
        Command<std::string> stringResultCmd = STRING_RESULT_COMMANDS[idx];
        if (cmd.compare(stringResultCmd.getCommandString()) == 0)
        {
            std::cout << "Executing: " << cmd << "(" << param << ")" << std::endl;
            return stringResultCmd.exec(param, radioWrapper);
        }
    }

    for (size_t idx = 0; idx < UINT32_RESULT_COMMANDS_LIST_LENGTH; ++idx)
    {
        Command<uint32_t> uintResultCmd = UINT32_RESULT_COMMANDS[idx];
        if (cmd.compare(uintResultCmd.getCommandString()) == 0)
        {
            std::cout << "Executing: " << cmd << "(" << param << ")" << std::endl;
            return std::to_string(uintResultCmd.exec(param, radioWrapper));
        }
    }

    return "COMMAND NOT VALID!";
}

/**
 * Returns a list of commands supported by this interpreter.
 */
std::string CommandParser::getCommandStringList()
{
    std::string cmdList = "SUPPORTED COMMANDS:\n";

    cmdList.append("\nRETURN STATUS: \n");
    for (size_t idx = 0; idx < STATUS_RESULT_COMMANDS_LIST_LENGTH; ++idx)
    {
        cmdList.append(STATUS_RESULT_COMMANDS[idx].getCommandString());
        cmdList.append(" - ");
        cmdList.append(STATUS_RESULT_COMMANDS[idx].getCommandDescription());
        cmdList.append("\n");
    }

    cmdList.append("\nRETURN STRING: \n");
    for (size_t idx = 0; idx < STRING_RESULT_COMMANDS_LIST_LENGTH; ++idx)
    {
        cmdList.append(STRING_RESULT_COMMANDS[idx].getCommandString());
        cmdList.append(" - ");
        cmdList.append(STRING_RESULT_COMMANDS[idx].getCommandDescription());
        cmdList.append("\n");
    }

    cmdList.append("\nRETURN UINT32: \n");
    for (size_t idx = 0; idx < UINT32_RESULT_COMMANDS_LIST_LENGTH; ++idx)
    {
        cmdList.append(UINT32_RESULT_COMMANDS[idx].getCommandString());
        cmdList.append(" - ");
        cmdList.append(UINT32_RESULT_COMMANDS[idx].getCommandDescription());
        cmdList.append("\n");
    }

    return cmdList;
}

/**
 * Commands with params are of the form:
 * <COMMAND>=<VAL>
 *
 * Commands with no params are of the form:
 * <COMMAND>
 * and -1 is used as the value
 *
 * Returns true if a valid command is found, false otherwise
 */
bool CommandParser::parse(const std::string& unparsedCommand, std::string& command, int& param)
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
        param = std::stoi(matches[2].str());
    }

    return true;
}
