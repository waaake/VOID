// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _CONFORM_RULE_H
#define _CONFORM_RULE_H

/* STD */
#include <string>
#include <unordered_map>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

/// @brief Provides a rule checking whether the source media is suitable for relinking to
/// the candidiate item or not
class VOID_API ConformRule
{
public:
    virtual std::string Name() const = 0;
    virtual bool Match(
        const std::unordered_map<std::string, std::string>& sourceMedia,
        const std::unordered_map<std::string, std::string>& candidateMedia
    ) const = 0;
};

VOID_NAMESPACE_CLOSE

#endif // _CONFORM_RULE_H
