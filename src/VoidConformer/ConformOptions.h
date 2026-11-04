// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _CONFORM_OPTIONS_H
#define _CONFORM_OPTIONS_H

/* Internal */
#include "Definition.h"
#include "ConformRule.h"

VOID_NAMESPACE_OPEN

typedef std::vector<ConformRule*> ConformRules;

struct ConformOptions
{
    ConformRules rules;
    bool tcmatch;
    bool reconform;

    ConformOptions(const ConformRules& rules, bool tcmatch = false, bool reconform = false)
        : rules(rules), tcmatch(tcmatch), reconform(reconform) {}
};

VOID_NAMESPACE_CLOSE

#endif // _CONFORM_OPTIONS_H
