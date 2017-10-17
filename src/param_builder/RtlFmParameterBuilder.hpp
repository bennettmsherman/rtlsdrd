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

// Project Includes
#include "BaseParameter.hpp"
#include "NumericParameter.hpp"
#include "StringParameter.hpp"
#include "RtlFmRunner.hpp"

class RtlFmParameterBuilder
{
public:

    // Parameter setting commands
    void setDeviceIndex(const std::string& deviceIndex, std::string* updatableMessage=nullptr);
    void setEnableOption(const std::string& enableOption, std::string* updatableMessage=nullptr);
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

    // Strings needed for rtl_fm and aplay
    static const char* const RTL_FM_EXECUTABLE_PATH;
    static const char* APLAY_ARGV[];

    // Since aplay's sample rate is dependent on this the sample/resample rate of rtl_fm,
    // use this offset into APLAY_ARGV[] to set it
    static const uint32_t APLAY_SAMPLE_RATE_ARG_IDX = 2;
};



#endif /* RTLFMWRAPPER_HPP_ */
