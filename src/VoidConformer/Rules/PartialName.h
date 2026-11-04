// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _PARTIAL_NAME_CONFORM_RULE_H
#define _PARTIAL_NAME_CONFORM_RULE_H

/* Internal */
#include "Definition.h"
#include "ConformRule.h"

VOID_NAMESPACE_OPEN

class VOID_API PartialNameRule : public ConformRule
{
public:
    PartialNameRule() = default;
    std::string Name() const override { return "Partial Name"; }
    bool Match(
        const std::unordered_map<std::string, std::string>& sourceMedia,
        const std::unordered_map<std::string, std::string>& candidateMedia
    ) const override;
};

VOID_NAMESPACE_CLOSE

#endif // _PARTIAL_NAME_CONFORM_RULE_H
