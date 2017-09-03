/*
 * RtlFmWrapper.cpp
 *
 *  Created on: Sep 2, 2017
 *      Author: Bennett Sherman
 */

// System Includes
#include <string>
#include <iostream>

// Project Includes
#include "RtlFmWrapper.hpp"

// Static Initialization

// Name of the rtl_fm executable
const char RtlFmWrapper::RTL_FM_NAME[] = "rtl_fm";

/**
 * Generates the command used to execute rtl_fm
 */
const std::string RtlFmWrapper::generateCommand()
{
    std::string command(RTL_FM_NAME);
    appendEntriesToCommand<uint32_t>(command, unsignedParams);
    appendEntriesToCommand<int32_t>(command, signedParams);
    appendEntriesToCommand<std::string>(command, stringParams);
    return command;
}

/**
 * Iterates over each option in the vector parameter and appends the string
 * form of the option, preceded by a space, to the command param
 */
template <typename ParamType, typename VectType>
void RtlFmWrapper::appendEntriesToCommand(std::string& command, const std::vector<VectType>& vect)
{
    for (auto const& param: vect)
    {
        command.append(" ");
        command.append(param.getPopulatedCmdLineEntry());
    }
}

void RtlFmWrapper::setDeviceIndex(const std::string& deviceIndex)
{
    unsignedParams.push_back(DeviceIndex::create(std::stoul(deviceIndex)));
}

void RtlFmWrapper::clearParamLists(const std::string& UNUSED)
{
    (void) UNUSED;
    unsignedParams.clear();
    signedParams.clear();
    stringParams.clear();
}

// TODO clean me up
void RtlFmWrapper::executeRtlFm(const std::string& UNUSED)
{
    (void) UNUSED;
    std::cout << "Executing: " << generateCommand() << std::endl;
    clearParamLists(UNUSED);
}

void RtlFmWrapper::setEnableOption(const std::string& enableOption)
{
    stringParams.push_back(EnableOption::create(enableOption));
}

void RtlFmWrapper::setFrequency(const std::string& frequency)
{
    unsignedParams.push_back(Frequency::create(frequency));
}

void RtlFmWrapper::setModulationMode(const std::string& modulationMode)
{
    stringParams.push_back(ModulationMode::create(modulationMode));
}

void RtlFmWrapper::setOversampling(const std::string& oversampling)
{
    unsignedParams.push_back(Oversampling::create(oversampling));
}

void RtlFmWrapper::setPpmError(const std::string& ppmError)
{
    unsignedParams.push_back(PpmError::create(ppmError));
}

void RtlFmWrapper::setSampleRate(const std::string& sampleRate)
{
    unsignedParams.push_back(SampleRate::create(sampleRate));
}

void RtlFmWrapper::setSquelchDelay(const std::string& squelchDelay)
{
    signedParams.push_back(SquelchDelay::create(squelchDelay));
}

void RtlFmWrapper::setSquelchLevel(const std::string& squelchLevel)
{
    unsignedParams.push_back(SquelchLevel::create(squelchLevel));
}


