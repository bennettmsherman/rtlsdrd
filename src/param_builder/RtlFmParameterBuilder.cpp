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
#include <cstdlib>

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
#include "SampleRate.hpp"
#include "SquelchDelay.hpp"
#include "SquelchLevel.hpp"
#include "StringParameter.hpp"
#include "RtlFmParameterBuilder.hpp"
#include "RtlFmRunner.hpp"
#include "TunerGain.hpp"

// Static Initialization
const char* const RtlFmParameterBuilder::RTL_FM_EXECUTABLE_PATH = "/usr/local/bin/rtl_fm";

// Note that APLAY_ARGV[2], the aplay sample rate, is modified at execution time
const char* RtlFmParameterBuilder::APLAY_ARGV[] = {"/usr/bin/aplay", "-r", "24000", "-f", "S16_LE", "-t", "raw", "-c", "1", static_cast<char *>(NULL)};

/**
 * Iterates over each option in the vector parameter places each option's
 * option member at position insertionIdx and value member at position insertionIdx + 1.
 */
template <typename ParamType, typename VectType>
void RtlFmParameterBuilder::appendEntriesToCommand(const char* cmdList[], uint32_t& insertionIdx, const std::vector<VectType>& vect)
{
    for (auto const& param: vect)
    {
        cmdList[insertionIdx++] = param.getOption();
        cmdList[insertionIdx++] = param.getValueCharPtr();
    }
}

/**
 * Updates APLAY_ARGV[APLAY_SAMPLE_RATE_ARG_IDX] with the value
 * of the first detected ResampleRate type parameter found in unsignedParams.
 * If no ResampleRate is found in unsignedParams, the LAST SampleRate
 * param found in unsignedParams's value is used as the aplay sample rate.
 */
void RtlFmParameterBuilder::setAplaySampleRate()
{
    /**
     * For each SampleRate parameter detected, the aplay argument will be updated, but the search
     * will continue. If a ResampleRate is detected, the aplay argument will be updated
     * and the search will exit. The idea here is that resample rate gets higher preference.
     * Use an auto reference so that the c_str() pointer is tied to the unsignedParam instance
     * inside the vector
     */
    for (auto& unsignedParam : unsignedParams)
    {
        if (unsignedParam.getOption() == SampleRate::getOption())
        {
            APLAY_ARGV[APLAY_SAMPLE_RATE_ARG_IDX] = unsignedParam.getValueCharPtr();
        }
        else if (unsignedParam.getOption() == ResampleRate::getOption())
        {
            APLAY_ARGV[APLAY_SAMPLE_RATE_ARG_IDX] = unsignedParam.getValueCharPtr();
            break;
        }
    }
}

void RtlFmParameterBuilder::setDeviceIndex(const std::string& deviceIndex)
{
    unsignedParams.push_back(DeviceIndex::create(std::stoul(deviceIndex)));
}

void RtlFmParameterBuilder::clearParamLists(const std::string& UNUSED)
{
    (void) UNUSED;
    unsignedParams.clear();
    signedParams.clear();
    stringParams.clear();
}

uint32_t RtlFmParameterBuilder::getNumberOfStoredParameters()
{
    return unsignedParams.size() + signedParams.size() + stringParams.size();
}

void RtlFmParameterBuilder::executeCommand(const std::string& UNUSED)
{
    (void) UNUSED;

    setAplaySampleRate();

    /**
     * 2 * getNumberOfStoredParameters() represents the total number of pointers
     * needed to be added to the list to account for each switch and value
     * i.e. "-t 42" requires 2 positions.
     * In addition, 2 is added so that cmdList[0] is the rtl_fm binary path
     * and cmdList[last] is null as per the man page.
     */
    const uint32_t cmdListSize = 2 * getNumberOfStoredParameters() + 2;
    const char* rtlFmCmdList[cmdListSize] {RTL_FM_EXECUTABLE_PATH};
    rtlFmCmdList[cmdListSize-1] = static_cast<char *>(NULL);

    uint32_t insertionIdx = 1;
    appendEntriesToCommand<uint32_t, NumericParameter<uint32_t>>(rtlFmCmdList, insertionIdx, unsignedParams);
    appendEntriesToCommand<int32_t, NumericParameter<int32_t>>(rtlFmCmdList, insertionIdx, signedParams);
    appendEntriesToCommand<std::string, StringParameter>(rtlFmCmdList, insertionIdx, stringParams);

    RtlFmRunner::getInstance().execRtlFmCommand(rtlFmCmdList, APLAY_ARGV);

    clearParamLists(UNUSED);
}

void RtlFmParameterBuilder::setEnableOption(const std::string& enableOption)
{
    stringParams.push_back(EnableOption::create(enableOption));
}

void RtlFmParameterBuilder::setFrequency(const std::string& frequency)
{
    unsignedParams.push_back(Frequency::create(frequency));
}

/**
 * "-M wbfm" actually translates to "-M fm -s 170k -A fast -r 32k -l 0 -E deemp"
 * which sounds terrible. As such, we'll make our own version of wbfm
 * which translates to "-M fm -s 1M -A fast -r 128k -l 0 -E deemp"
 */
void RtlFmParameterBuilder::setModulationMode(const std::string& modulationMode)
{
    if (!modulationMode.compare("wbfm"))
    {
        stringParams.push_back(ModulationMode::create("fm"));
        unsignedParams.push_back(SampleRate::create(1000000));
        unsignedParams.push_back(ResampleRate::create(192000));
        unsignedParams.push_back(SquelchLevel::create(0));
        stringParams.push_back(EnableOption::create("deemp"));
        stringParams.push_back(AtanMath::create("fast"));
    }
    else
    {
        stringParams.push_back(ModulationMode::create(modulationMode));
    }
}

void RtlFmParameterBuilder::setOversampling(const std::string& oversampling)
{
    unsignedParams.push_back(Oversampling::create(oversampling));
}

void RtlFmParameterBuilder::setPpmError(const std::string& ppmError)
{
    unsignedParams.push_back(PpmError::create(ppmError));
}

void RtlFmParameterBuilder::setSampleRate(const std::string& sampleRate)
{
    unsignedParams.push_back(SampleRate::create(sampleRate));
}

void RtlFmParameterBuilder::setSquelchDelay(const std::string& squelchDelay)
{
    signedParams.push_back(SquelchDelay::create(squelchDelay));
}

void RtlFmParameterBuilder::setSquelchLevel(const std::string& squelchLevel)
{
    unsignedParams.push_back(SquelchLevel::create(squelchLevel));
}

void RtlFmParameterBuilder::setResampleRate(const std::string& resampleRate)
{
    unsignedParams.push_back(ResampleRate::create(resampleRate));
}

void RtlFmParameterBuilder::setAtanMath(const std::string& atanMath)
{
    stringParams.push_back(AtanMath::create(atanMath));
}

void RtlFmParameterBuilder::setTunerGain(const std::string& tunerGain)
{
    signedParams.push_back(TunerGain::create(tunerGain));
}
