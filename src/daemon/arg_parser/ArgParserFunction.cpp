/*
 * ArgParserFunction.cpp
 *
 *  Created on: Dec 23, 2017
 *      Author: bensherman
 */

// System Includes
#include <string>

// Project Includes
#include "ArgParserFunction.hpp"

// Static initialization
const std::string ArgParserFunction::SHORT_SPECIFIER_IDENTIFIER = "-";
const std::string ArgParserFunction::EXTENDED_SPECIFIER_IDENTIFIER = "--";

/**
 * Constructs an ArgParserFunction
 */
ArgParserFunction::ArgParserFunction(char shortSpecifier,
        std::string extendedSpecifier,
        ArgParserFunc argParserFunc,
        bool requiresParameter, std::string descriptionParam) :
        shortSpecifier(shortSpecifier),
        extendedSpecifier(extendedSpecifier),
        parserFunction(argParserFunc),
        requiresParameter(requiresParameter),
        description(descriptionParam)
{
};

/**
 * Executes the function pointed to by parserFunction with the parameter
 * funcParam.
 */
void ArgParserFunction::exec(std::string funcParam) const
{
    (*parserFunction)(funcParam);
}

std::string ArgParserFunction::getCommandDescription() const
{
    return description;
}

char ArgParserFunction::getShortSpecifier() const
{
    return shortSpecifier;
}

std::string ArgParserFunction::getExtendedSpecifier() const
{
    return extendedSpecifier;
}

/**
 * Returns true if the command line argument addressed by this instance
 * requires a parameter. If no argument is required, false is returned.
 */
bool ArgParserFunction::isParameterRequired() const
{
    return requiresParameter;
}

