// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_ICON_TYPES_H
#define _VOID_ICON_TYPES_H

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

enum class IconType : char16_t
{
    icon_add            = 0xe145,
    icon_browse         = 0xeb13,
    icon_brush          = 0xe3ae,
    icon_close          = 0xe5cd,
    icon_delete         = 0xe872,
    icon_draw           = 0xe746,
    icon_ink_eraser     = 0xe6d0,
    icon_lists          = 0xe9b9,
    icon_library_add    = 0xe02e,
    icon_grid_view      = 0xe9b0,
    icon_playlist_play  = 0xe05f,
    icon_sort_by_alpha  = 0xe053,
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_ICON_TYPES_H
