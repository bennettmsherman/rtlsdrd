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

/**
 * A command line option's representation. A short specifier is one which
 * is prefixed with "-". An extended specifier is prefixed with "--". Each
 * function is associated with a pointer to a function which takes a const
 * std::string& as its parameter, with a void return. Furthermore, each
 * ArgParserFunction has a description as a well as a boolean specifying
 * whether or not it accepts a parameter.
 */
class ArgParserFunction
{
public:

    typedef void (*ArgParserFunc) (const std::string&);

    ////////////////////////////////
    // Public interface functions //
    ////////////////////////////////
    ArgParserFunction(char shortSpecifier,
            std::string extendedSpecifier,
            ArgParserFunc argParserFunc,
            bool requiresParameter, std::string descriptionParam);

    void exec(std::string funcParam = "") const;

    std::string getCommandDescription() const;

    char getShortSpecifier() const;

    std::string getExtendedSpecifier() const;

    bool isParameterRequired() const;

    // Class members
    static const std::string SHORT_SPECIFIER_IDENTIFIER;
    static const std::string EXTENDED_SPECIFIER_IDENTIFIER;

private:
    // A single-character parameter specifier which will follow '-'
    const char shortSpecifier;

    // A more descriptive parameter specifier which will follow "--"
    const std::string extendedSpecifier;

    // Pointer to the function which will be called by exec()
    const ArgParserFunc parserFunction;

    // Specifies if this command line arg takes a parameter
    const bool requiresParameter;

    // Description of the purpose of this argument
    const std::string description;
};

#endif /* DAEMON_ARG_PARSER_ARGPARSERFUNCTION_HPP_ */
