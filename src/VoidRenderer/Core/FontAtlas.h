// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _RENDERER_FONT_ATLAS_H
#define _RENDERER_FONT_ATLAS_H

/* STD */
#include <unordered_map>

/* Freetype */
#include <ft2build.h>
#include FT_FREETYPE_H

/* GLM */
#include <glm/glm.hpp>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

struct FChar
{
    float uMin, uMax;
    float vMin, vMax;

    glm::ivec2 size;
    glm::ivec2 bearing;

    unsigned int advance;
};

class FontAtlas
{
public:
    FontAtlas();
    FontAtlas(FT_Face face, int size, int width = 1024, int height = 1024);

    unsigned int TextureId() const { return m_Texture; }
    const FChar& GetChar(char c);

private: /* Members */
    unsigned int m_Texture;
    int m_Width, m_Height;
    int m_PenX, m_PenY, m_RowHeight;

    FT_Face m_Face;
    int m_Size;

    std::unordered_map<char, FChar> m_Characters;

private: /* Methods */
    void Create();
    /**
     * @brief Adds the glyph corresponding to the character into the underlying texture.
     * 
     * @param c Character to be added to the atlas.
     */
    const FChar& AddChar(char c);
};

VOID_NAMESPACE_CLOSE

#endif // _RENDERER_FONT_ATLAS_H
