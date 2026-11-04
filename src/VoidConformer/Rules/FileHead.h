// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _FILEHEAD_CONFORM_RULE_H
#define _FILEHEAD_CONFORM_RULE_H

/* Internal */
#include "Definition.h"
#include "ConformRule.h"

VOID_NAMESPACE_OPEN

class VOID_API FileHeadRule : public ConformRule
{
public:
    FileHeadRule() = default;
    std::string Name() const override { return "File Head"; }
    bool Match(
        const std::unordered_map<std::string, std::string>& sourceMedia,
        const std::unordered_map<std::string, std::string>& candidateMedia
    ) const override;
};

VOID_NAMESPACE_CLOSE

#endif // _FILEHEAD_CONFORM_RULE_H
