/*
 * ArgParser.cpp
 *
 *  Created on: Dec 23, 2017
 *      Author: bensherman
 */

// System Includes
#include <csignal>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

// Project Includes
#include "ArgParser.hpp"
#include "ArgParserFunction.hpp"
#include "RtlFmParameterBuilder.hpp"
#include "SystemUtils.hpp"
#include "TcpServer.hpp"

// Static Initialization
// The arguments supported by the arg parser
const ArgParserFunction ArgParser::ARG_PARSER_FUNCTIONS[] =
{
    ArgParserFunction {'h', "help", &printHelp, false,
        "Prints the arguments supported by this program"},
    ArgParserFunction {'a', "audiocontrol", &setAudioControlName, true,
        "Sets the audio control name used for volume control; see output of 'amixer scontrols'. Default: \"Master\""},
    ArgParserFunction {'o', "outputdevice", &setAudioOutputDevice, true,
        "Sets the ALSA name/address of the device to output the audio stream to; see 'aplay -L' and 'aplay -l'. Default: \"plughw:0,0\""},
    ArgParserFunction {'p', "password", &setServerPassword, true,
        "Sets the password which clients must provide to use this server. Default: no password"},
    ArgParserFunction {'P', "port", &setServerPort, true,
        "Set the TCP port number of this rtlsdrd server. Default: 2832"}
};

// Number of functions stored within ARG_PARSER_FUNCTIONS[]
const size_t ArgParser::ARG_PARSER_FUNCTIONS_LIST_LENGTH =
        sizeof(ARG_PARSER_FUNCTIONS)/sizeof(ArgParserFunction);

TcpServer::TcpServerBuilder ArgParser::tcpServerBuilder;
bool ArgParser::hasParserBeenExecuted = false;

/**
 * The expected input is the argc (number of arguments) and argv[] (argument
 * vector) provided to this program. This function will iterate over each
 * element in argv[], and will try to match its values to the argument types
 * specified in ARG_PARSER_FUNCTIONS[]. Once a match has been found, the
 * function associated with the detected ArgParserFunction will be executed.
 * If any errors occur during these processes, a std::invalid_argument or
 * runtime_error is thrown.
 *
 * This parser accepts arguments with "short specifiers", which are a single
 * character prefixed with "-". Also accepted are "extended specifiers", which
 * are greater than one character and have the prefix "--". Each argument can
 * have at most one parameter.
 *
 * This parser will configure and instantiate several singletons. Due to this,
 * if this function is executed more than once, all executions following the
 * first will not reinstantiate these singletons. All executions following the
 * first will result in a std::logic_error being thrown.
 */
void ArgParser::parse(int argc, const char *argv[])
{
    // If this function has been executed previously, throw a std::logic_error
    if (hasParserBeenExecuted)
    {
        throw std::logic_error("Executing ArgParser::parse() more than once!"
                " Parsed arguments will NOT be reflected.");
    }
    else
    {
        hasParserBeenExecuted = true;
    }

    // Start at 1 to avoid element 0, which is the program name
    int argIdx = 1;

    while (argIdx < argc)
    {
        std::string argAsStr(argv[argIdx]);
        const ArgParserFunction* argParserFunc = getArgParserFunctionFromArg(argAsStr);

        // If argAsStr was matched to an ArgParserFunction in
        // ARG_PARSER_FUNCTIONS
        if (argParserFunc != nullptr)
        {
            executeArgParserFunction(argParserFunc, argAsStr, argIdx, argc, argv);
        }
        // If argAsStr WAS NOT matched to an ArgParserFunction in
        // ARG_PARSER_FUNCTIONS
        else
        {
            throw std::invalid_argument("Argument: " + argAsStr +
                    " is not supported by this program. "
                    "Use \"-h\" or \"--help\" to show the supported arguments");
        }
    }

    // Configure and instantiate the TcpServer singleton
    TcpServer::getInstance(&tcpServerBuilder);
}

/**
 * This function will execute the function contained within the argParserFunc
 * parameter. If the ArgParserFunction addressed by argParserFunc requires a
 * parameter, this function will retrieve the parameter from argv[].
 * Once argParserFunc is executed, argvIdx is updated to index the next
 * argument which is expected to correspond to an ArgParserFunction within
 * argv[].
 *
 * If any errors occur, this function will throw a a std::invalid_argument and
 * argvIdx will NOT be updated.
 */
void ArgParser::executeArgParserFunction(const ArgParserFunction* argParserFunc,
        const std::string& argAsStr, int& argvIdx, const int argc,
        const char *argv[])
{
    // If the function associated with this ArgParserFunction DOES
    // require a parameter
    if (argParserFunc->isParameterRequired())
    {
        // If this arg is the last in argv, but a param is required
        // throw an exception
        if (argvIdx+1 >= argc)
        {
            throw std::invalid_argument("Argument: " + argAsStr + ", "
                    "the last element in argv, expects a param, "
                    "but none exists");
        }

        // Verifies that if a parameter is required for the current
        // argument, the next element in argv[] is actually a
        // param, and not another argument
        std::string nextArg = std::string{argv[argvIdx+1]};
        ArgType nextArgType = determineArgType(nextArg);
        if (nextArgType != ArgType::PARAM)
        {
            throw std::invalid_argument("Argument: " + argAsStr + " "
                    "requires a param, but the following element in "
                    "argv(" + nextArg + ") is not a param type");
        }
        // If this function requires an argument
        else
        {
            argParserFunc->exec(nextArg);
        }
    }
    // If the function associated with this ArgParserFunction DOESN'T
    // require a parameter
    else
    {
        argParserFunc->exec();
    }
    incrementArgIdx(argParserFunc, argvIdx);
}

/**
 * Given a single argument from argv[], this function will categorize the
 * argument. If the parsed argument is one which corresponds to an
 * ArgParserFunction instance in ArgParser::ARG_PARSER_FUNCTIONS[], then a ptr
 * to the associated function is returned. If the argument does not correspond
 * to an entry in ArgParser::ARG_PARSER_FUNCTIONS[], but the argument has valid
 * form (that is, if it's a short or extended type in an appropriate location),
 * nullptr is returned. If the parsed argument is found to be of type
 * ArgType::PARAM or ArgType::INVALID, std::invalid_argument is thrown.
 * std::runtime_error is thrown in the event that there was a general error.
 */
const ArgParserFunction* ArgParser::getArgParserFunctionFromArg(const std::string& singlarArg)
{
    ArgType argType = determineArgType(singlarArg);
    const ArgParserFunction* argParserFunc = nullptr;

    switch (argType)
    {
        case ArgType::SHORT:
            argParserFunc = findByShortSpecifier(singlarArg);
            break;

        case ArgType::EXTENDED:
            argParserFunc = findByExtendedSpecifier(singlarArg);
            break;

        // Each time a new argument is detected, argIdx is incremented
        // to point to the next argument, so that argv[argIdx] should never
        // point to the parameter to a cmdline argument. As such, if
        // argv[argIdx] does point to a parameter (which isn't prefixed by
        // "--" or "-", an error has occurred.
        case ArgType::PARAM:
            throw std::invalid_argument(singlarArg + " is a param to a"
                    " cmdline arg, although no associated cmdline arg was"
                    " detected");
            break;

        case ArgType::INVALID:
            throw std::invalid_argument("Invalid argument detected");

        default:
            throw std::runtime_error(
                    "Unknown error occurred in ArgParser");
    }
    return argParserFunc;
}


/**
 * If the ArgParserFunction pointed to by func requires a parameter, argIdx is
 * incremented by 2. Otherwise, argIdx is incremented by 1.
 */
void ArgParser::incrementArgIdx(const ArgParserFunction * const func, int& argIdx)
{
    if (func->isParameterRequired())
    {
        argIdx += 2;
    }
    else
    {
        argIdx += 1;
    }
}

/**
 * Finds what type of argument strToCheck is. In the event that an error is
 * detected, ArgType::INVALID is returned to the caller so that an appropriate
 * response can be taken.
 */
ArgParser::ArgType ArgParser::determineArgType(const std::string& strToCheck)
{
    if (strToCheck.find(ArgParserFunction::EXTENDED_SPECIFIER_IDENTIFIER) == 0)
    {
        // If strToCheck's value is "--" with no specifier
        if (strToCheck.length() == ArgParserFunction::EXTENDED_SPECIFIER_IDENTIFIER.length())
        {
            std::cerr << "detected an an extended type with a size less"
                    " than a single character, which is invalid" << std::endl;
            return ArgType::INVALID;
        }
        else
        {
            return ArgType::EXTENDED;
        }
    }
    else if (strToCheck.find(ArgParserFunction::SHORT_SPECIFIER_IDENTIFIER) == 0)
    {
        // +1 accounts for the single-character switch name
        if (strToCheck.length() > ArgParserFunction::SHORT_SPECIFIER_IDENTIFIER.length() + 1)
        {
            std::cerr << strToCheck << " is a short type with a size greater"
                    " than a single character, which is invalid. You should"
                    " likely use \"" <<
                    ArgParserFunction::EXTENDED_SPECIFIER_IDENTIFIER << "\""
                    "instead" << std::endl;
            return ArgType::INVALID;
        }
        // If strToCheck's value is "-" with no specifier
        else if (strToCheck.length() == ArgParserFunction::SHORT_SPECIFIER_IDENTIFIER.length())
        {
            std::cerr << "detected an a short type with a size less"
                    " than a single character, which is invalid" << std::endl;
            return ArgType::INVALID;
        }
        else
        {
            return ArgType::SHORT;
        }
    }
    else
    {
        return ArgType::PARAM;
    }
}

/**
 * Returns a pointer to the ArgParserFunction whose short specifier member
 * matches the short specifier contained within the shortSpecifierWithIdentifier
 * parameter. Specifically, the shortSpecifierWithIdentifier param is expected
 * to be in the form "-<short specifier>". In the event that the specifier's
 * size is greater than 1, a std::invalid_argument is thrown. If no
 * ArgParserFunction in ARG_PARSER_FUNCTIONS has a short specifier member which
 * matches the param, nullptr is returned.
 */
const ArgParserFunction* ArgParser::findByShortSpecifier(const std::string& shortSpecifierWithIdentifier)
{
    // Get the specifier without the "-" prefix
    std::string shortSpecifierStr = shortSpecifierWithIdentifier.
            substr(ArgParserFunction::SHORT_SPECIFIER_IDENTIFIER.length());

    // Short specifier arguments must always be a single character
    if (shortSpecifierStr.length() > 1)
    {
        throw std::invalid_argument("Short specifier-type argument " +
                shortSpecifierStr +
                " has a specifier of length greater than 1");
    }

    // Since short specifiers are a single character, get the single char for
    // comparison against those stored in each ArgParserFunction in
    // ARG_PARSER_FUNCTIONS
    char shortSpecifierChar = shortSpecifierStr.c_str()[0];

    for (size_t funcIdx = 0; funcIdx < ARG_PARSER_FUNCTIONS_LIST_LENGTH; ++funcIdx)
    {
        if (shortSpecifierChar == ARG_PARSER_FUNCTIONS[funcIdx].getShortSpecifier())
        {
            return &ARG_PARSER_FUNCTIONS[funcIdx];
        }
    }
    return nullptr;
}

/**
 * The input is assumed to be an extended-specifier-type argument (that is,
 * one which is preceded by "--"). Given this specifier, a const pointer to
 * the associated function in ARG_PARSER_FUNCTIONS (the ArgParserFunction whose
 * extendedSpecifier member matches the extended specifier in the
 * extendedSpecifierWithIdentifier param) will be returned if one such function
 * exists. If no function with this extended specifier exists in
 * ARG_PARSER_FUNCTIONS, nullptr is returned.
 */
const ArgParserFunction* ArgParser::findByExtendedSpecifier(const std::string& extendedSpecifierWithIdentifier)
{
    // Get the argument specifier/name without the "--" prefix
    std::string extendedSpecifier = extendedSpecifierWithIdentifier.
            substr(ArgParserFunction::EXTENDED_SPECIFIER_IDENTIFIER.length());

    for (size_t funcIdx = 0; funcIdx < ARG_PARSER_FUNCTIONS_LIST_LENGTH; ++funcIdx)
    {
        if (extendedSpecifier == ARG_PARSER_FUNCTIONS[funcIdx].getExtendedSpecifier())
        {
            return &ARG_PARSER_FUNCTIONS[funcIdx];
        }
    }
    return nullptr;
}

/**
 * Sets the audio control name used for volume control; see output of
 * 'amixer scontrols'
 */
void ArgParser::setAudioControlName(const std::string& audioControlName)
{
    std::cout << "(ArgParser) Using audio control name: " << audioControlName
            << std::endl;
    SystemUtils::getInstance(audioControlName.c_str());
}

/**
 * Sets the ALSA name/address of the device to output the audio stream to;
 * see 'aplay -L' and 'aplay -l'
 */
void ArgParser::setAudioOutputDevice(const std::string& audioOutputDevice)
{
    std::cout << "(ArgParser) Using audio output device: " << audioOutputDevice
            << std::endl;
    RtlFmParameterBuilder::setAudioOutputDeviceName(audioOutputDevice.c_str());
}

/**
 * Prints information about which arguments are supported by this program.
 * Following, this function sends a SIGTERM to the calling process, causing
 * the program to terminate. The logic behind this is that when the user
 * requests the help options, the program should quit so that they can determine
 * which options they want to execute it with.
 */
void ArgParser::printHelp(const std::string& UNUSED)
{
    (void) UNUSED;

    std::cout << "RTLSDRD Controller For RTL-SDR; AUTHOR: Bennett Sherman "
                 "(bennettmsherman@gmail.com)" << std::endl;

    std::cout << "AVAILABLE ARGUMENTS: " << std::endl;

    std::cout << "SHORT(" << ArgParserFunction::SHORT_SPECIFIER_IDENTIFIER
            << ")  EXTENDED("
            << ArgParserFunction::EXTENDED_SPECIFIER_IDENTIFIER
            << ")\tTAKES PARAM  DESCRIPTION" << std::endl;
    std::cout << "--------  ------------\t-----------  -----------"
            << std::endl;

    for (size_t cmdIdx = 0; cmdIdx < ARG_PARSER_FUNCTIONS_LIST_LENGTH; ++cmdIdx)
    {
        const ArgParserFunction* currFunc = &ARG_PARSER_FUNCTIONS[cmdIdx];
        std::printf("    %c\t  %-11s\t    %-7s  %s\n",
                currFunc->getShortSpecifier(),
                currFunc->getExtendedSpecifier().c_str(),
                currFunc->isParameterRequired() ? "yes" : "no",
                currFunc->getCommandDescription().c_str());
    }

    // Since printf() doesn't flush
    std::fflush(stdout);

    // Cause the program to terminate. The sighandler for SIGTERM is provided
    // in rtlsdrd.cpp
    raise(SIGTERM);
}

/**
 * Sets the password which the user must specify to utilize rtlsdrd
 */
void ArgParser::setServerPassword(const std::string& password)
{
    std::cout << "(ArgParser) Using rtlsdrd password: " << password
            << std::endl;
    tcpServerBuilder.withPassword(password);
}

/**
 * Sets the TCP port number which rtlsdrd will communicate on. If the specified
 * port is greater than the TCP limit (65535), as std::invalid_argument
 * will be thrown
 */
void ArgParser::setServerPort(const std::string& portAsStr)
{
    auto portAsUl = std::stoul(portAsStr);

    if (portAsUl > UINT16_MAX)
    {
        throw std::invalid_argument("Specified port number: " + portAsStr +
                " is invalid!");
    }

    std::cout << "(ArgParser) Using rtlsdrd port number: " << portAsStr
            << std::endl;

    tcpServerBuilder.withPort(portAsUl);
}
