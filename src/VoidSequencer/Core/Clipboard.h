// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SEQUENCER_CLIPBOARD_H
#define _SEQUENCER_CLIPBOARD_H

/* STD */
#include <unordered_set>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

enum class ClipboardContext
{
    COPY,
    CUT
};

template <typename _Ty>
struct Clipboard
{
    std::unordered_set<_Ty> items;
    ClipboardContext context;

    bool Empty() const noexcept { return items.empty(); }
    explicit operator bool() const noexcept { return !items.empty(); }
};

VOID_NAMESPACE_CLOSE

#endif // _SEQUENCER_CLIPBOARD_H
