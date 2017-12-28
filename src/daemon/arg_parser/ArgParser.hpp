/*
 * ArgParser.hpp
 *
 *  Created on: Dec 23, 2017
 *      Author: bensherman
 */

#ifndef DAEMON_ARG_PARSER_ARGPARSER_HPP_
#define DAEMON_ARG_PARSER_ARGPARSER_HPP_

// System Includes
#include <cstdint>
#include <string>

// Project Includes
#include "ArgParserFunction.hpp"

class ArgParser
{
public:
    static void parse(int argc, const char *argv[]);

private:

    enum class ArgType {SHORT, EXTENDED, PARAM, INVALID};

    // Functions which handle their associated arguments
    static void setAudioControlName(const std::string& audioControlName);
    static void setAudioOutputDevice(const std::string& audioOutputDevice);
    static void printHelp(const std::string& UNUSED);

    static ArgType determineArgType(const std::string& strToCheck);
    static void incrementArgIdx(const ArgParserFunction * const func, int& argIdx);

    static const ArgParserFunction* findByShortSpecifier(const std::string& shortSpecifierWithIdentifier);
    static const ArgParserFunction* findByExtendedSpecifier(const std::string& extendedSpecifierWithIdentifier);

    // Class variables
    static const ArgParserFunction ARG_PARSER_FUNCTIONS[];
    static const size_t ARG_PARSER_FUNCTIONS_LIST_LENGTH;
};

#endif /* DAEMON_ARG_PARSER_ARGPARSER_HPP_ */
