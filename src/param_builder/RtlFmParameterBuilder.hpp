/*
 * RtlFmWrapper.hpp
 *
 *  Created on: Sep 2, 2017
 *      Author: Bennett Sherman
 */

#ifndef RTLFMWRAPPER_HPP_
#define RTLFMWRAPPER_HPP_

// System Includes
#include <stdint.h>
#include <vector>
#include <string>

// Project Includes
#include "BaseParameter.hpp"
#include "NumericParameter.hpp"
#include "StringParameter.hpp"
#include "RtlFmRunner.hpp"

class RtlFmParameterBuilder
{
public:

    // Constructor
    RtlFmParameterBuilder();

    // Parameter setting commands
    void setDeviceIndex(const std::string& deviceIndex, std::string* updatableMessage=nullptr);
    void setEnableOption(const std::string& enableOption, std::string* updatableMessage=nullptr);
    void setFirSize(const std::string& firSize, std::string* updatableMessage);
    void setFrequency(const std::string& frequency, std::string* updatableMessage=nullptr);
    void setModulationMode(const std::string& modulationMode, std::string* updatableMessage=nullptr);
    void setOversampling(const std::string& oversampling, std::string* updatableMessage=nullptr);
    void setPpmError(const std::string& ppmError, std::string* updatableMessage=nullptr);
    void setSampleRate(const std::string& sampleRate, std::string* updatableMessage=nullptr);
    void setSquelchDelay(const std::string& squelchDelay, std::string* updatableMessage=nullptr);
    void setSquelchLevel(const std::string& squelchLevel, std::string* updatableMessage=nullptr);
    void setResampleRate(const std::string& resampleRate, std::string* updatableMessage=nullptr);
    void setAtanMath(const std::string& atanMath, std::string* updatableMessage=nullptr);
    void setTunerGain(const std::string& tunerGain, std::string* updatableMessage=nullptr);
    void broadcastAmStationMacro(const std::string& amFreqInKilohertz, std::string* updatableMessage=nullptr);
    void broadcastFmStationMacro(const std::string& fmFreqInMegahertz, std::string* updatableMessage=nullptr);
    void setScannableFrequency(const std::string& scannableFrequency, std::string* updatableMessage=nullptr);

    // Special function commands
    void clearParamLists(const std::string& UNUSED, std::string* updatableMessage=nullptr);
    void executeCommand(const std::string& UNUSED, std::string* updatableMessage=nullptr);
    void getUserProvidedCommands(const std::string& UNUSED, std::string* updatableMessage=nullptr);

    // Commands not callable by the interepreter
    void storeUserEnteredCommand(const std::string& function, const std::string& separator, const std::string& param);
    static void setAudioOutputDeviceName(const std::string& deviceName);
    static bool isDefaultSpecifier(const std::string& param);

private:
    uint32_t getNumberOfStoredParameters();
    void setAplaySampleRate();

    template <typename ParamType, typename VectType>
    static void appendEntriesToCommand(const char* cmdList[], uint32_t& insertionIdx, const std::vector<VectType>& vect);

    /*
     * These vectors will contain their respective parameter types.
     * When rtl_fm is to be executed, each of these vectors
     * will be iterated over to find which parameters are to be used.
     */
    std::vector<NumericParameter<uint32_t>> unsignedParams;
    std::vector<NumericParameter<int32_t>> signedParams;
    std::vector<StringParameter> stringParams;

    /**
     * Contains all *valid* functions supplied by the user in the form
     * "FUNCTION=PARAM". The command interpreter is tasked with add to this,
     * but the RtlFmParameterBuilder ensures that it gets cleaned out when the
     * user used the EXECUTE or CLEAR command
     */
    std::vector<std::string> userProvidedCommands;

    // Strings needed for rtl_fm and aplay
    static const char* const RTL_FM_EXECUTABLE_PATH;
    static const std::vector<const char*> BASE_APLAY_ARGV;

    // Local copy of BASE_APLAY_ARGV whose aplay sample rate will be set
    // dynamically
    std::vector<const char*> aplayArgs;

    // Since aplay's sample rate is dependent on this the sample/resample rate of rtl_fm,
    // use this offset into aplayArgs[] to set it
    static const uint32_t APLAY_SAMPLE_RATE_ARG_IDX = 2;

    // The name of the soundcard device specifier/alias for the aplay output
    static std::string audioOutputDeviceName;
    static const char* const DEFAULT_AUDIO_OUTPUT_DEVICE_NAME;

    // The index inside APLAY_ARGV containing the audio output device name,
    // which is to be set at startup.
    static const uint32_t APLAY_OUTPUT_DEVICE_NAME_ARG_IDX = 10;

    // When this string is specified as a parameter, perform some default action
    static const std::string DEFAULT_PARAMETER_SPECIFIER;
};

#endif /* RTLFMWRAPPER_HPP_ */
