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

    // Private constructor
    ArgParser();

    // Delete the default copy constructor
    ArgParser(const ArgParser&) = delete;

    // Delete the default assignment operator
    ArgParser& operator=(const ArgParser&) = delete;

    /* Describes the type of a detected argument.
     * SHORT refers to arguments preceded by "-"
     * EXTENDED refers to arguments preceded by "--"
     * PARAM refers to parameters to an argument
     * INVALID indicates that the character/string is incorrect
     * In "--a 10 -p pass", "--a" = EXTENDED, "10" = PARAM, "-p" = SHORT, and
     * "pass" = PARAM
     */
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
