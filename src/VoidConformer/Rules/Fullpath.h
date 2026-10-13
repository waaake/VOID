// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _FULLPATH_CONFORM_RULE_H
#define _FULLPATH_CONFORM_RULE_H

/* Internal */
#include "Definition.h"
#include "ConformRule.h"

VOID_NAMESPACE_OPEN

class VOID_API FullpathRule : public ConformRule
{
public:
    FullpathRule() = default;
    std::string Name() const override { return "Fullpath"; }
    bool Match(
        const std::unordered_map<std::string, std::string>& sourceMedia,
        const std::unordered_map<std::string, std::string>& candidateMedia
    ) const override;
};

VOID_NAMESPACE_CLOSE

#endif // _FULLPATH_CONFORM_RULE_H
