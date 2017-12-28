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
        bool requiresArg, std::string descriptionParam) :
        shortSpecifier(shortSpecifier),
        extendedSpecifier(extendedSpecifier),
        parserFunction(argParserFunc),
        requiresParameter(requiresArg),
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

bool ArgParserFunction::doesRequireParameter() const
{
    return requiresParameter;
}

