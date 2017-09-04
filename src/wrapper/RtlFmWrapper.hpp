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
#include "DeviceIndex.hpp"
#include "EnableOption.hpp"
#include "Frequency.hpp"
#include "ModulationMode.hpp"
#include "NumericParameter.hpp"
#include "Oversampling.hpp"
#include "PpmError.hpp"
#include "SampleRate.hpp"
#include "SquelchDelay.hpp"
#include "SquelchLevel.hpp"
#include "StringParameter.hpp"

class RtlFmWrapper
{
public:

    // Parameter setting commands
    void setDeviceIndex(const std::string& deviceIndex);
    void setEnableOption(const std::string& enableOption);
    void setFrequency(const std::string& frequency);
    void setModulationMode(const std::string& modulationMode);
    void setOversampling(const std::string& oversampling);
    void setPpmError(const std::string& ppmError);
    void setSampleRate(const std::string& sampleRate);
    void setSquelchDelay(const std::string& squelchDelay);
    void setSquelchLevel(const std::string& squelchLevel);

    // Special function commands
    void clearParamLists(const std::string& UNUSED);
    void executeRtlFm(const std::string& UNUSED);

private:
    const std::string generateCommand();

    template <typename ParamType, typename VectType>
    static void appendEntriesToCommand(std::string& command, const std::vector<VectType>& vect);

    /*
     * These vectors will contain their respective parameter types.
     * When rtl_fm is to be executed, each of these vectors
     * will be iterated over to find which parameters are to be used.
     */
    std::vector<NumericParameter<uint32_t>> unsignedParams;
    std::vector<NumericParameter<int32_t>> signedParams;
    std::vector<StringParameter> stringParams;

    static const char RTL_FM_NAME[];
};



#endif /* RTLFMWRAPPER_HPP_ */
