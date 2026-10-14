// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _TIMECODE_CONFORM_RULE_H
#define _TIMECODE_CONFORM_RULE_H

/* Internal */
#include "Definition.h"
#include "ConformRule.h"

VOID_NAMESPACE_OPEN

class VOID_API TimecodeRule : public ConformRule
{
public:
    TimecodeRule() = default;
    std::string Name() const override { return "TC"; }
    bool Match(
        const std::unordered_map<std::string, std::string>& sourceMedia,
        const std::unordered_map<std::string, std::string>& candidateMedia
    ) const override;
};

VOID_NAMESPACE_CLOSE

#endif // _TIMECODE_CONFORM_RULE_H
