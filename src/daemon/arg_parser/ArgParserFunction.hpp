/*
 * ArgParserFunction.hpp
 *
 *  Created on: Dec 22, 2017
 *      Author: bensherman
 */

#ifndef DAEMON_ARG_PARSER_ARGPARSERFUNCTION_HPP_
#define DAEMON_ARG_PARSER_ARGPARSERFUNCTION_HPP_

// System Includes
#include <string>

class ArgParserFunction
{
public:

    typedef void (*ArgParserFunc) (const std::string&);

    ////////////////////////////////
    // Public interface functions //
    ////////////////////////////////
    ArgParserFunction(char shortSpecifier,
            std::string extendedSpecifier,
            ArgParserFunc argParserFunc, std::string descriptionParam);

    void exec(std::string funcParam) const;

    std::string getCommandDescription() const;

    char getShortSpecifier() const;

    std::string getExtendedSpecifier() const;

    // Class members
    static const std::string SHORT_SPECIFIER_IDENTIFIER;
    static const std::string EXTENDED_SPECIFIER_IDENTIFIER;

private:

    // A single-character parameter specifier which will follow '-'
    const char shortSpecifier;

    // A more descriptive parameter specifier which will follow "--"
    const std::string extendedSpecifier;

    const ArgParserFunc parserFunction;
    const std::string description;
};

#endif /* DAEMON_ARG_PARSER_ARGPARSERFUNCTION_HPP_ */
