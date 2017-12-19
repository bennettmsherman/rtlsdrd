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
#include <stdexcept>

// Project Includes
#include "AtanMath.hpp"
#include "DeviceIndex.hpp"
#include "EnableOption.hpp"
#include "FirSize.hpp"
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
const char* const RtlFmParameterBuilder::DEFAULT_AUDIO_OUTPUT_DEVICE_NAME = "plughw:0,0";
std::string RtlFmParameterBuilder::audioOutputDeviceName = DEFAULT_AUDIO_OUTPUT_DEVICE_NAME;
const std::string RtlFmParameterBuilder::DEFAULT_PARAMETER_SPECIFIER = "default";

const std::vector<const char*> RtlFmParameterBuilder::BASE_APLAY_ARGV {"/usr/bin/aplay", "-r",
        "24000", "-f", "S16_LE", "-t", "raw", "-c", "1", "-D",
        nullptr /*output device name*/, static_cast<char *>(NULL)};

/**
 * Instantiates an RtlFmParameterBuilder. aplayArgs is initialized to
 * BASE_APLAY_ARGV, and the audio output device name within aplayArgs is set to
 * the value stored in the audioOutputDeviceName
 */
RtlFmParameterBuilder::RtlFmParameterBuilder(): aplayArgs(BASE_APLAY_ARGV)
{
    aplayArgs[APLAY_OUTPUT_DEVICE_NAME_ARG_IDX] = audioOutputDeviceName.c_str();
}

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
 * Updates aplayArgs[APLAY_SAMPLE_RATE_ARG_IDX] with the value
 * of the first detected ResampleRate type parameter found in unsignedParams
 * whose value is greater than zero.
 * If no positive (> 0) ResampleRate is found in unsignedParams, the LAST
 * SampleRate param found in unsignedParams's value is used as the aplay sample
 * rate.
 */
void RtlFmParameterBuilder::setAplaySampleRate()
{
    /**
     * For each SampleRate parameter detected, the aplay argument will be updated, but the search
     * will continue. If a POSITIVE ResampleRate is detected, the aplay argument will be updated
     * and the search will exit. If a nonpositive (<= 0) ResampleRate is
     * detected, the search continues. The idea here is that resample rate gets
     * higher preference. Use an auto reference so that the c_str() pointer is
     * tied to the unsignedParam instance inside the vector
     */
    for (auto& unsignedParam : unsignedParams)
    {
        if (unsignedParam.getOption() == SampleRate::getOption())
        {
            aplayArgs[APLAY_SAMPLE_RATE_ARG_IDX] = unsignedParam.getValueCharPtr();
        }
        else if ((unsignedParam.getOption() == ResampleRate::getOption()) &&
                    (unsignedParam.getValue() != "0"))
        {
            aplayArgs[APLAY_SAMPLE_RATE_ARG_IDX] = unsignedParam.getValueCharPtr();
            break;
        }
    }
}

/**
 * If "default" is specified, don't provide this param to rtl_fm
 */
void RtlFmParameterBuilder::setDeviceIndex(const std::string& deviceIndex, std::string* updatableMessage)
{
    (void) updatableMessage;

    if (!isDefaultSpecifier(deviceIndex))
    {
        unsignedParams.push_back(DeviceIndex(deviceIndex));
    }
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

    RtlFmRunner::getInstance().execRtlFmCommand(rtlFmCmdList, &aplayArgs[0], userProvidedCommands);

    clearParamLists(UNUSED);

    // Inform each client that a new execution of rtl_fm and aplay has occurred
    TcpServer::getInstance().informAllClientsOfStateChange();
}

/**
 * If "default" is specified, don't provide this param to rtl_fm
 */
void RtlFmParameterBuilder::setEnableOption(const std::string& enableOption, std::string* updatableMessage)
{
    (void) updatableMessage;

    if (!isDefaultSpecifier(enableOption))
    {
        stringParams.push_back(EnableOption(enableOption));
    }
}

/**
 * If "default" is specified, don't provide this param to rtl_fm
 */
void RtlFmParameterBuilder::setFirSize(const std::string& firSize, std::string* updatableMessage)
{
    (void) updatableMessage;

    if (!isDefaultSpecifier(firSize))
    {
        stringParams.push_back(FirSize(firSize));
    }
}

/**
 * If "default" is specified, use the default frequency contained in the
 * Frequency class
 */
void RtlFmParameterBuilder::setFrequency(const std::string& frequency, std::string* updatableMessage)
{
    (void) updatableMessage;
    if (isDefaultSpecifier(frequency))
    {
        unsignedParams.push_back(Frequency());
    }
    else
    {
        unsignedParams.push_back(Frequency(frequency));
    }
}

/**
 * "-M wbfm" actually translates to "-M fm -s 170k -A fast -r 32k -l 0 -E deemp"
 * which sounds terrible. I removed these params from rtl_fm. To ensure that the
 * GUI is consistent, I'll require that the user specify fast atan math and
 * deemphasis, rather than setting them internally. Otherwise, it'll be the case
 * that the GUI doesn't truly reflect the state of the radio.
 *
 * If "default" is specified, use the default contained in the ModulationMode
 * class.
 */
void RtlFmParameterBuilder::setModulationMode(const std::string& modulationMode, std::string* updatableMessage)
{
    (void) updatableMessage;

    if (isDefaultSpecifier(modulationMode))
    {
        stringParams.push_back(ModulationMode());
    }
    else
    {
        stringParams.push_back(ModulationMode(modulationMode));
    }
}

/**
 * If "default" is specified, don't provide this param to rtl_fm
 */
void RtlFmParameterBuilder::setOversampling(const std::string& oversampling, std::string* updatableMessage)
{
    (void) updatableMessage;

    if (!isDefaultSpecifier(oversampling))
    {
        unsignedParams.push_back(Oversampling(oversampling));
    }
}

/**
 * If "default" is specified, don't provide this param to rtl_fm
 */
void RtlFmParameterBuilder::setPpmError(const std::string& ppmError, std::string* updatableMessage)
{
    (void) updatableMessage;

    if (!isDefaultSpecifier(ppmError))
    {
        unsignedParams.push_back(PpmError(ppmError));
    }
}

/**
 * If "default" is specified, utilize the default value stored in the
 * SampleRate class
 */
void RtlFmParameterBuilder::setSampleRate(const std::string& sampleRate, std::string* updatableMessage)
{
    (void) updatableMessage;

    if (isDefaultSpecifier(sampleRate))
    {
        unsignedParams.push_back(SampleRate());
    }
    else
    {
        unsignedParams.push_back(SampleRate(sampleRate));
    }
}

/**
 * If "default" is specified, don't provide this param to rtl_fm
 */
void RtlFmParameterBuilder::setSquelchDelay(const std::string& squelchDelay, std::string* updatableMessage)
{
    (void) updatableMessage;

    if (!isDefaultSpecifier(squelchDelay))
    {
        signedParams.push_back(SquelchDelay(squelchDelay));
    }
}

/**
 * If "default" is specified, don't provide this param to rtl_fm
 */
void RtlFmParameterBuilder::setSquelchLevel(const std::string& squelchLevel, std::string* updatableMessage)
{
    (void) updatableMessage;

    if (!isDefaultSpecifier(squelchLevel))
    {
        unsignedParams.push_back(SquelchLevel(squelchLevel));
    }
}

/**
 * If "default" is specified, don't provide this param to rtl_fm
 */
void RtlFmParameterBuilder::setResampleRate(const std::string& resampleRate, std::string* updatableMessage)
{
    (void) updatableMessage;

    if (!isDefaultSpecifier(resampleRate))
    {
        unsignedParams.push_back(ResampleRate(resampleRate));
    }
}

/**
 * If "default" is specified, don't provide this param to rtl_fm
 */
void RtlFmParameterBuilder::setAtanMath(const std::string& atanMath, std::string* updatableMessage)
{
    (void) updatableMessage;

    if (!isDefaultSpecifier(atanMath))
    {
        stringParams.push_back(AtanMath(atanMath));
    }
}

/**
 * If "default" is specified, don't provide this param to rtl_fm
 */
void RtlFmParameterBuilder::setTunerGain(const std::string& tunerGain, std::string* updatableMessage)
{
    (void) updatableMessage;

    if (!isDefaultSpecifier(tunerGain))
    {
        signedParams.push_back(TunerGain(tunerGain));
    }
}

/**
 * Convenience function which takes a broadcast AM station's frequency (in kHz)
 * as a parameter, and sets the:
 * - Frequency
 * - Sample rate (to 20k)
 * - Modulation mode (to AM)
 * - Direct sampling (enabled)
 */
void RtlFmParameterBuilder::broadcastAmStationMacro(const std::string& amFreqInKilohertz, std::string* updatableMessage)
{
    (void) updatableMessage;
    unsignedParams.push_back(Frequency(ParamBuilderUtils::broadcastAmKilohertzToHertz(amFreqInKilohertz)));
    unsignedParams.push_back(SampleRate(20000));
    stringParams.push_back(ModulationMode("am"));
    stringParams.push_back(EnableOption("direct"));
}

/**
 * Convenience function which takes a broadcast FM station's frequency (in MHz)
 * as a parameter, and sets the:
 * - Frequency
 * - Sample rate - 200 kHz
 * - Resample rate - 44.1 kHz
 * - atanMath = fast
 * - Modulation mode (to fm)
 */
void RtlFmParameterBuilder::broadcastFmStationMacro(const std::string& fmFreqInMegahertz, std::string* updatableMessage)
{
    (void) updatableMessage;
    unsignedParams.push_back(Frequency(ParamBuilderUtils::broadcastFmMegahertzToHertz(fmFreqInMegahertz)));
    unsignedParams.push_back(SampleRate(200000));
    unsignedParams.push_back(ResampleRate(44100));
    stringParams.push_back(EnableOption("deemp"));
    setModulationMode("fm");
}

void RtlFmParameterBuilder::setScannableFrequency(const std::string& scannableFrequency, std::string* updatableMessage)
{
    (void) updatableMessage;
    stringParams.push_back(ScannableFrequency(scannableFrequency));
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
 * have been entered since the last call to EXECUTE or CLEAR. This does NOT
 * include the volume command.
 */
void RtlFmParameterBuilder::getUserProvidedCommands(const std::string& UNUSED, std::string* updatableMessage)
{
    (void) UNUSED;

    updatableMessage->append("\n");
    for (std::string userCmd : userProvidedCommands)
    {
        updatableMessage->append(userCmd);
        updatableMessage->append("\n");
    }
}

/**
 * Sets the hasDeviceNameBeenSet member. This function should only be called
 * once at startup. If it's called a second time, an std::invalid_argument
 * exception is thrown.
 */
void RtlFmParameterBuilder::setAudioOutputDeviceName(const std::string& deviceName)
{
    static bool hasDeviceNameBeenSet = false;
    if (!hasDeviceNameBeenSet)
    {
        audioOutputDeviceName = deviceName;
        hasDeviceNameBeenSet = true;
    }
    else
    {
        throw std::invalid_argument("The audio output device name can only be set once");
    }
}

/**
 * Returns true of param == DEFAULT_PARAM_SPECIFIER, false otherwise
 */
bool RtlFmParameterBuilder::isDefaultSpecifier(const std::string& param)
{
    return (param == DEFAULT_PARAMETER_SPECIFIER);
}

