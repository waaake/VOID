// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* GLEW */
#include <GL/glew.h>

/* Internal */
#include "FontAtlas.h"
// #include "FontEngine.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

FontAtlas::FontAtlas()
    : m_Texture(0)
    , m_Width(0)
    , m_Height(0)
    , m_PenX(0)
    , m_PenY(0)
    , m_RowHeight(0)
    , m_Face(nullptr)
    , m_Size(0)
{
}

FontAtlas::FontAtlas(FT_Face face, int size, int width, int height)
    : m_Texture(0)
    , m_Width(width)
    , m_Height(height)
    , m_PenX(0)
    , m_PenY(0)
    , m_RowHeight(0)
    , m_Face(face)
    , m_Size(size)
{
    Create();
}

void FontAtlas::Create()
{
    // /* Set Font face Pixel size */
    // FT_Set_Pixel_Sizes(m_Ft_Face, 0, size);
    glGenTextures(1, &m_Texture);
    glBindTexture(GL_TEXTURE_2D, m_Texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_Width, m_Height, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
}

const FChar& FontAtlas::GetChar(char c)
{
    const auto& it = m_Characters.find(c);
    return it == m_Characters.end() ? AddChar(c) : it->second;
}

const FChar& FontAtlas::AddChar(char c)
{
    FT_Set_Pixel_Sizes(m_Face, 0, m_Size);

    if (!FT_Load_Char(m_Face, static_cast<FT_ULong>(c), FT_LOAD_RENDER))
    {
        static FChar character{};
        return character;
    }

    FT_Bitmap& bitmap = m_Face->glyph->bitmap;
    // const Character& ch = FontEngine::Instance().GetChar(m_Face, static_cast<FT_ULong>(c));


    // FT_Bitmap& bitmap = m_Face->glyph->bitmap;
    if (m_PenX + bitmap.width >= m_Width)
    {
        m_PenX = 0;
        m_PenY += m_RowHeight;
        m_RowHeight = 0;
    }

    glBindTexture(GL_TEXTURE_2D, m_Texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, m_PenX, m_PenY, bitmap.width, bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, bitmap.buffer);

    FChar ch;
    ch.uMin = m_PenX / (float)m_Width;
    ch.vMin = m_PenY / (float)m_Height;

    ch.uMax = m_PenX + bitmap.width / (float)m_Width;
    ch.vMax = m_PenY + bitmap.rows / (float)m_Height;

    ch.size = { bitmap.width, bitmap.rows };
    ch.bearing = { m_Face->glyph->bitmap_left, m_Face->glyph->bitmap_top };

    ch.advance = m_Face->glyph->advance.x >> 6;

    m_Characters[c] = ch;

    m_PenX += bitmap.width;
    m_RowHeight = std::max(m_RowHeight, static_cast<int>(bitmap.rows));

    return m_Characters[c];
}

VOID_NAMESPACE_CLOSE
