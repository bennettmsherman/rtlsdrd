/*
 * ArgParser.cpp
 *
 *  Created on: Dec 23, 2017
 *      Author: bensherman
 */

// System Includes
#include <cstdint>
#include <string>
#include <iostream>

// Project Includes
#include "ArgParserFunction.hpp"
#include "ArgParser.hpp"
#include "SystemUtils.hpp"
#include "RtlFmParameterBuilder.hpp"

// Static Initialization
// The parameters supported by the arg parser
const ArgParserFunction ArgParser::ARG_PARSER_FUNCTIONS[] =
{
    ArgParserFunction {'a', "audiocontrol", &setAudioControlName, "Sets the audio control name used for volume control; see output of 'amixer scontrols'"},
    ArgParserFunction {'o', "outputdevice", &setAudioOutputDevice, "Sets the ALSA name/address of the device to output the audio stream to; see 'aplay -L' and 'aplay -l'"}
};

// Number of functions stored within ARG_PARSER_FUNCTIONS[]
const size_t ArgParser::ARG_PARSER_FUNCTIONS_LIST_LENGTH =
        sizeof(ARG_PARSER_FUNCTIONS)/sizeof(ArgParserFunction);

void ArgParser::parse(int argc, const char *argv[])
{
    std::cout << "Num args received: " << argc << std::endl;

    // Start at 1 to avoid element 0, which is the program name
    for (auto argIdx = 1; argIdx < argc; argIdx+=2)
    {
        std::string argAsStr(argv[argIdx]);
        ArgType argType = determineArgType(argAsStr);
        const ArgParserFunction* argParserFunc = nullptr;

        if (argType == ArgType::SHORT)
        {
            argParserFunc = findByShortSpecifier(argAsStr);
        }
        else if (argType == ArgType::EXTENDED)
        {
            argParserFunc = findByExtendedSpecifier(argAsStr);
        }
        else
        {
            continue;
        }

        if (argParserFunc != nullptr)
        {
            argParserFunc->exec(argv[argIdx+1]);
        }
    }
}

ArgParser::ArgType ArgParser::determineArgType(const std::string& strToCheck)
{
    if (strToCheck.find(ArgParserFunction::EXTENDED_SPECIFIER_IDENTIFIER) == 0)
    {
        std::cout << strToCheck << " is an extended type" << std::endl;
        return ArgType::EXTENDED;
    }
    else if (strToCheck.find(ArgParserFunction::SHORT_SPECIFIER_IDENTIFIER) == 0)
    {
        // +1 accounts for the single-character switch name
        if (strToCheck.length() > ArgParserFunction::SHORT_SPECIFIER_IDENTIFIER.length() + 1)
        {
            std::cerr << strToCheck << " is a short type with a switch greater"
                    " than a single character, which is invalid" << std::endl;
            return ArgType::INVALID;
        }
        else
        {
            std::cout << strToCheck << " is a short type" << std::endl;
            return ArgType::SHORT;
        }
    }
    else
    {
        std::cout << strToCheck << " cannot be matched to a short or long type" << std::endl;
        return ArgType::INVALID;
    }
}

/**
 * Returns a pointer to the ArgParserFunction whose short specifier member
 * matches the short specifier contained within the shortSpecifierWithIdentifier
 * parameter. Specifically, the shortSpecifierWithIdentifier param is expected
 * to be in the form "-<short specifier>". In the event that the specifier's
 * size is greater than 1, an error message is printed and a nullptr is
 * returned, since short specifiers should be a single character.
 */
const ArgParserFunction* ArgParser::findByShortSpecifier(const std::string& shortSpecifierWithIdentifier)
{
    std::string shortSpecifierStr = shortSpecifierWithIdentifier.
            substr(ArgParserFunction::SHORT_SPECIFIER_IDENTIFIER.length());

    if (shortSpecifierStr.length() > 1)
    {
        std::cerr << "ERROR! findByShortSpecifier() has detected short"
                "specifier of length greater than 1" << shortSpecifierStr.length()
                << std::endl;
        return nullptr;
    }

    // Since short specifiers are a single character, get the single char for
    // comparison against those stored in each ArgParserFunction in
    // ARG_PARSER_FUNCTIONS
    char shortSpecifierChar = shortSpecifierStr.c_str()[0];

    std::cout << "Got short flag :" << shortSpecifierChar << std::endl;

    for (size_t funcIdx = 0; funcIdx < ARG_PARSER_FUNCTIONS_LIST_LENGTH; ++funcIdx)
    {
        if (shortSpecifierChar == ARG_PARSER_FUNCTIONS[funcIdx].getShortSpecifier())
        {
            std::cout << "Found func for short flag :" << shortSpecifierChar << std::endl;
            return &ARG_PARSER_FUNCTIONS[funcIdx];
        }
    }
    return nullptr;
}


const ArgParserFunction* ArgParser::findByExtendedSpecifier(const std::string& extendedSpecifierWithIdentifier)
{
    std::string extendedSpecifier = extendedSpecifierWithIdentifier.
            substr(ArgParserFunction::EXTENDED_SPECIFIER_IDENTIFIER.length());

    std::cout << "Got extended identifier: " << extendedSpecifier << std::endl;

    for (size_t funcIdx = 0; funcIdx < ARG_PARSER_FUNCTIONS_LIST_LENGTH; ++funcIdx)
    {
        if (extendedSpecifier == ARG_PARSER_FUNCTIONS[funcIdx].getExtendedSpecifier())
        {
            std::cout << "Found func for extended specifier: " << extendedSpecifier << std::endl;
            return &ARG_PARSER_FUNCTIONS[funcIdx];
        }
    }
    return nullptr;
}

void ArgParser::setAudioControlName(const std::string& audioControlName)
{
    std::cout << "Using audio control name: " << audioControlName << std::endl;
    SystemUtils::getInstance(audioControlName.c_str());
}

void ArgParser::setAudioOutputDevice(const std::string& audioOutputDevice)
{
    std::cout << "Using audio output device: " << audioOutputDevice << std::endl;
    RtlFmParameterBuilder::setAudioOutputDeviceName(audioOutputDevice.c_str());

}
