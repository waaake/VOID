// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _FILENAME_CONFORM_RULE_H
#define _FILENAME_CONFORM_RULE_H

/* Internal */
#include "Definition.h"
#include "ConformRule.h"

VOID_NAMESPACE_OPEN

class VOID_API FilenameRule : public ConformRule
{
public:
    FilenameRule() = default;
    std::string Name() const override { return "Filename"; }
    bool Match(
        const std::unordered_map<std::string, std::string>& sourceMedia,
        const std::unordered_map<std::string, std::string>& candidateMedia
    ) const override;
};

VOID_NAMESPACE_CLOSE

#endif // _FILENAME_CONFORM_RULE_H
