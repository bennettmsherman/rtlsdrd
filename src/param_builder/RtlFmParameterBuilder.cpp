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
#include "ParamBuilderUtils.hpp"
#include "PpmError.hpp"
#include "ResampleRate.hpp"
#include "RtlFmParameterBuilder.hpp"
#include "RtlFmRunner.hpp"
#include "SampleRate.hpp"
#include "ScannableFrequency.hpp"
#include "SquelchDelay.hpp"
#include "SquelchLevel.hpp"
#include "StringParameter.hpp"
#include "TcpServer.hpp"
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

void RtlFmParameterBuilder::setDeviceIndex(const std::string& deviceIndex, std::string* updatableMessage)
{
    (void) updatableMessage;
    unsignedParams.push_back(DeviceIndex::create(std::stoul(deviceIndex)));
}

/**
 * Clears each of the internal lists which store the state of the parameter
 * builder.
 */
void RtlFmParameterBuilder::clearParamLists(const std::string& UNUSED, std::string* updatableMessage)
{
    (void) UNUSED;
    (void) updatableMessage;
    unsignedParams.clear();
    signedParams.clear();
    stringParams.clear();
    userProvidedCommands.clear();
}

uint32_t RtlFmParameterBuilder::getNumberOfStoredParameters()
{
    return unsignedParams.size() + signedParams.size() + stringParams.size();
}

/**
 * Launches aplay and rtl_fm using the Parameters stored in each of the typed
 * vectors. Once the execution of both processes is complete, all clients
 * connected to this server are informed that a new execution of rtl_fm has
 * occurred, and that the parameters which reflect the new system state can be
 * retrieved.
 */
void RtlFmParameterBuilder::executeCommand(const std::string& UNUSED, std::string* updatableMessage)
{
    (void) UNUSED;
    (void) updatableMessage;

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

    RtlFmRunner::getInstance().execRtlFmCommand(rtlFmCmdList, APLAY_ARGV, userProvidedCommands);

    clearParamLists(UNUSED);

    // Inform each client that a new execution of rtl_fm and aplay has occurred
    TcpServer::getInstance().informAllClientsOfStateChange();
}

void RtlFmParameterBuilder::setEnableOption(const std::string& enableOption, std::string* updatableMessage)
{
    (void) updatableMessage;
    stringParams.push_back(EnableOption::create(enableOption));
}

void RtlFmParameterBuilder::setFrequency(const std::string& frequency, std::string* updatableMessage)
{
    (void) updatableMessage;
    unsignedParams.push_back(Frequency::create(frequency));
}

/**
 * "-M wbfm" actually translates to "-M fm -s 170k -A fast -r 32k -l 0 -E deemp"
 * which sounds terrible. As such, we'll make our own version of wbfm
 * which translates to "-M fm -s 2.4M -A fast -r 44.1k -l 0 -E deemp"
 */
void RtlFmParameterBuilder::setModulationMode(const std::string& modulationMode, std::string* updatableMessage)
{
    (void) updatableMessage;
    if (!modulationMode.compare("wbfm"))
    {
        stringParams.push_back(ModulationMode::create("fm"));
        unsignedParams.push_back(SampleRate::create(2400000));
        unsignedParams.push_back(ResampleRate::create(44100));
        unsignedParams.push_back(SquelchLevel::create(0));
        stringParams.push_back(EnableOption::create("deemp"));
        stringParams.push_back(AtanMath::create("fast"));
    }
    else
    {
        stringParams.push_back(ModulationMode::create(modulationMode));
    }
}

void RtlFmParameterBuilder::setOversampling(const std::string& oversampling, std::string* updatableMessage)
{
    (void) updatableMessage;
    unsignedParams.push_back(Oversampling::create(oversampling));
}

void RtlFmParameterBuilder::setPpmError(const std::string& ppmError, std::string* updatableMessage)
{
    (void) updatableMessage;
    unsignedParams.push_back(PpmError::create(ppmError));
}

void RtlFmParameterBuilder::setSampleRate(const std::string& sampleRate, std::string* updatableMessage)
{
    (void) updatableMessage;
    unsignedParams.push_back(SampleRate::create(sampleRate));
}

void RtlFmParameterBuilder::setSquelchDelay(const std::string& squelchDelay, std::string* updatableMessage)
{
    (void) updatableMessage;
    signedParams.push_back(SquelchDelay::create(squelchDelay));
}

void RtlFmParameterBuilder::setSquelchLevel(const std::string& squelchLevel, std::string* updatableMessage)
{
    (void) updatableMessage;
    unsignedParams.push_back(SquelchLevel::create(squelchLevel));
}

void RtlFmParameterBuilder::setResampleRate(const std::string& resampleRate, std::string* updatableMessage)
{
    (void) updatableMessage;
    unsignedParams.push_back(ResampleRate::create(resampleRate));
}

void RtlFmParameterBuilder::setAtanMath(const std::string& atanMath, std::string* updatableMessage)
{
    (void) updatableMessage;
    stringParams.push_back(AtanMath::create(atanMath));
}

void RtlFmParameterBuilder::setTunerGain(const std::string& tunerGain, std::string* updatableMessage)
{
    (void) updatableMessage;
    signedParams.push_back(TunerGain::create(tunerGain));
}

/**
 * Convenience function which takes a broadcast AM station's frequency (in kHz)
 * as a parameter, and sets the:
 * - Frequency
 * - Sample rate (to 2.4M)
 * - Modulation mode (to AM)
 * - Direct sampling (enabled)
 * - Resample rate (to 44.1k)
 */
void RtlFmParameterBuilder::broadcastAmStationMacro(const std::string& amFreqInKilohertz, std::string* updatableMessage)
{
    (void) updatableMessage;
    unsignedParams.push_back(Frequency::create(ParamBuilderUtils::broadcastAmKilohertzToHertz(amFreqInKilohertz)));
    unsignedParams.push_back(SampleRate::create(2400000));
    unsignedParams.push_back(ResampleRate::create(44100));
    stringParams.push_back(ModulationMode::create("am"));
    stringParams.push_back(EnableOption::create("direct"));
}

/**
 * Convenience function which takes a broadcast FM station's frequency (in MHz)
 * as a parameter, and sets the:
 * - Frequency
 * - Modulation mode (to wbfm)
 */
void RtlFmParameterBuilder::broadcastFmStationMacro(const std::string& fmFreqInMegahertz, std::string* updatableMessage)
{
    (void) updatableMessage;
    unsignedParams.push_back(Frequency::create(ParamBuilderUtils::broadcastFmMegahertzToHertz(fmFreqInMegahertz)));
    setModulationMode("wbfm");
}

void RtlFmParameterBuilder::setScannableFrequency(const std::string& scannableFrequency, std::string* updatableMessage)
{
    (void) updatableMessage;
    stringParams.push_back(ScannableFrequency::create(scannableFrequency));
}

/**
 * Adds the command in the form function + separator + param to
 * userProvidedCommands. This command is assumed to be valid,
 * and is expected to be called by the parser.
 */
void RtlFmParameterBuilder::storeUserEnteredCommand(const std::string& function,
                                                    const std::string& separator,
                                                    const std::string& param)
{
    userProvidedCommands.push_back(function + separator + param);
}

/**
 * Appends to updatableMessage the list of valid user supplied commands which
 * have been entered since the last call to EXECUTE or CLEAR.
 */
void RtlFmParameterBuilder::getUserProvidedCommands(const std::string& UNUSED, std::string* updatableMessage)
{
    (void) UNUSED;
    for (std::string userCmd : userProvidedCommands)
    {
        updatableMessage->append(userCmd);
        updatableMessage->append("\n");
    }
}


