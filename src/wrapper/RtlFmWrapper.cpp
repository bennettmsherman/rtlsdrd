/*
 * RtlFmWrapper.cpp
 *
 *  Created on: Sep 2, 2017
 *      Author: Bennett Sherman
 */

// System Includes
#include <string>
#include <iostream>
#include <unistd.h>

// Project Includes
#include "AtanMath.hpp"
#include "DeviceIndex.hpp"
#include "EnableOption.hpp"
#include "Frequency.hpp"
#include "ModulationMode.hpp"
#include "NumericParameter.hpp"
#include "Oversampling.hpp"
#include "PpmError.hpp"
#include "ResampleRate.hpp"
#include "RtlFmWrapper.hpp"
#include "SampleRate.hpp"
#include "SquelchDelay.hpp"
#include "SquelchLevel.hpp"
#include "StringParameter.hpp"

// Static Initialization
//TODO dynamically set sample rate for aplay
const char RtlFmWrapper::COMMAND_FORMAT[] = "rtl_fm %s | aplay -r 128000 -f S16_LE -t raw -c 1";
const uint32_t RtlFmWrapper::BASE_COMMAND_FORMAT_LENGTH = sizeof(COMMAND_FORMAT);

/**
 * Generates the command used to execute rtl_fm
 */
const std::string RtlFmWrapper::generateCommand()
{
    std::string command("");
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

    // Contains the parameters to be provided to rtl_fm
    std::string paramString = generateCommand();

    // needed for sprintf buffering
    char commandBuff[BASE_COMMAND_FORMAT_LENGTH + paramString.length() + 1] {};

    // commandBuff will contain the formatted command needed to execute rtl_fm
    std::sprintf(commandBuff, COMMAND_FORMAT, paramString.c_str());

    pid_t pid = fork();
    if (pid == 0)
    {
        std::cout << "Executing: " << commandBuff << std::endl;

        (void) system(commandBuff);
    }
    else
    {
        std::cout << "My kid's PID: " << pid << std::endl;
    }

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

/**
 * "-M wbfm" actually translates to "-M fm -s 170k -A fast -r 32k -l 0 -E deemp"
 * which sounds terrible. As such, we'll make our own version of wbfm
 * which translates to "-M fm -s 1M -A fast -r 128k -l 0 -E deemp"
 */
void RtlFmWrapper::setModulationMode(const std::string& modulationMode)
{
    if (!modulationMode.compare("wbfm"))
    {
        stringParams.push_back(ModulationMode::create("fm"));
        unsignedParams.push_back(SampleRate::create(1000000));
        unsignedParams.push_back(ResampleRate::create(128000));
        unsignedParams.push_back(SquelchLevel::create(0));
        stringParams.push_back(EnableOption::create("deemp"));
        stringParams.push_back(AtanMath::create("fast"));
    }
    else
    {
        stringParams.push_back(ModulationMode::create(modulationMode));
    }
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

void RtlFmWrapper::setResampleRate(const std::string& resampleRate)
{
    unsignedParams.push_back(ResampleRate::create(resampleRate));
}

void RtlFmWrapper::setAtanMath(const std::string& atanMath)
{
    stringParams.push_back(AtanMath::create(atanMath));
}
