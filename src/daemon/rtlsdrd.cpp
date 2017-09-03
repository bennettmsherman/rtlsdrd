//============================================================================
// Name        : rtlsdrd.cpp
// Author      : Ben Sherman
// Version     :
// Copyright   : 
// Description : Hello World in C++, Ansi-style
//============================================================================

// System Includes
#include <stdint.h>
#include <SquelchLevel.hpp>
#include <iostream>
#include <vector>

// Project Includes
#include "DeviceIndex.hpp"
#include "Frequency.hpp"
#include "Oversampling.hpp"
#include "PpmError.hpp"
#include "SampleRate.hpp"
#include "SquelchDelay.hpp"
#include "StringParameter.hpp"
#include "ModulationMode.hpp"
#include "NumericParameter.hpp"
#include "BaseParameter.hpp"
#include "EnableOption.hpp"

int main()
{
    std::vector<NumericParameter<uint32_t>> unsignedParamVect;
    std::vector<NumericParameter<int32_t>> signedParamVect;
    std::vector<StringParameter> stringParamVect;

    unsignedParamVect.push_back(DeviceIndex::create(1));
    unsignedParamVect.push_back(Frequency::create(2));
    unsignedParamVect.push_back(Oversampling::create(3));
    unsignedParamVect.push_back(PpmError::create(4));
    unsignedParamVect.push_back(SampleRate::create(5));
    unsignedParamVect.push_back(SquelchLevel::create(6));

    signedParamVect.push_back(SquelchDelay::create(-7));

    stringParamVect.push_back(ModulationMode::create("wbfm"));
    stringParamVect.push_back(EnableOption::create("edge"));
    stringParamVect.push_back(EnableOption::create("dc"));
    stringParamVect.push_back(EnableOption::create("direct"));

    for (NumericParameter<uint32_t> param : unsignedParamVect)
    {
        std::cout << param.getPopulatedCmdLineEntry() << " ";
    }

    for (NumericParameter<int32_t> param : signedParamVect)
    {
        std::cout << param.getPopulatedCmdLineEntry() << " ";
    }

    for (StringParameter param : stringParamVect)
    {
        std::cout << param.getPopulatedCmdLineEntry() << " ";
    }
}
