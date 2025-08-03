// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* GLEW */
#include <GL/glew.h>

/* Internal */
#include "FontEngine.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

FontEngine::FontEngine()
{
    /* Initialize Freetype */
    if (FT_Init_FreeType(&m_FtLib))
    {
        VOID_LOG_ERROR("Unable to Initialize FreeType.");
    }
}

FontEngine::~FontEngine()
{
    /* Delete all of the Font Faces that were used */
    for (auto it = m_FontData.begin(); it != m_FontData.end(); )
    {
        /* Delete the Face pointer*/
        FT_Face f = it->first;

        if (f)
            FT_Done_Face(f);

        /* Remove the index from map */
        it = m_FontData.erase(it);
    }

    /* Clear faces */
    m_FaceStore.clear();

    /* Delete textures and clears map */
    ClearTextures();
}

FT_Face FontEngine::GetFace(const std::string& path)
{
    /* Check if we have faces defined for the path */
    auto it = m_FaceStore.find(path);

    /* It hasn't been used yet */
    if (it == m_FaceStore.end())
    {
        /* Create the Font */
        if (CreateFace(path))
            return m_FaceStore.at(path);
        
        /* Could not create the face from the font file */
        return nullptr;
    }

    /* We already have a font associated with the path */
    return m_FaceStore.at(path);
}

void FontEngine::DeleteFace(FT_Face face)
{
    /* Delete the face and it's data */
    m_FontData.erase(face);

    /* Free Face */
    FT_Done_Face(face);
}

void FontEngine::ClearTextures()
{
    /* For all of the font faces, delete all the texture id's that were assigned in the current GL context */
    for (auto it = m_TextureStore.begin(); it != m_TextureStore.end(); )
    {
        for (auto texIt = it->second.begin(); texIt != it->second.end(); ++texIt)
        {
            /* Free the Texture */
            glDeleteTextures(1, &texIt->second.textureId);
        }

        /* Erase from the map */
        it = m_TextureStore.erase(it);
    }
}

const Character& FontEngine::GetChar(FT_Face face, FT_ULong character)
{
    /**
     * Font Key
     * Face
     * Face's vertical size (pixels per EM)
     */
    FontKey key = {face, face->size->metrics.y_ppem};

    /* It's not yet present -> Load -> Cache the character */
    if (m_TextureStore.find(key) == m_TextureStore.end())
    {
        Character ch = Load(face, character);

        /* Cache it since it's not yet present */
        m_TextureStore[key] = {{character, ch}};
    }

    /* This time we have the data for the face present */
    std::unordered_map<FT_ULong, Character>& data = m_TextureStore.at(key);

    /* It's not yet present -> Load -> Cache the character */
    if (data.find(character) == data.end())
    {
        Character ch = Load(face, character);

        /* Cache it on the face data */
        data[character] = ch;
    }

    /**
     * Since we're returning a const reference to the character
     * so the returned reference has to point at the character from the underlying map struct
     * By now, we have ensured that we have the character added to the cache (if it wasn't already there)
     */
    return data.at(character);
}

bool FontEngine::CreateFace(const std::string& path)
{
    /* Read the File as Byte Array */
    QFile ff(path.c_str());
    /* Cannot read the File */
    if (!ff.open(QIODevice::ReadOnly))
    {
        VOID_LOG_ERROR("Not able to open Font File");
        return false;
    }

    /* Font Data */
    QByteArray data = ff.readAll();

    /* Font Face */
    FT_Face face;

    /* Load the Font Face */
    if (FT_New_Memory_Face(m_FtLib, reinterpret_cast<const FT_Byte*>(data.constData()), data.size(), 0, &face))
    {
        VOID_LOG_ERROR("Not Able to load Font from file: {0}", path);
        return false;
    }
    
    /* Add this to the Cache */
    m_FaceStore[path] = face;

    /* The Data needs to be preserved till the font is required */
    m_FontData[face] = data;

    /* Font created successfully */
    return true;
}

Character FontEngine::Load(FT_Face face, FT_ULong character) const
{
    /* Load the Glyph */
    FT_Load_Char(face, character, FT_LOAD_RENDER);

    /**
     * Most GL Textures are 4 bytes so gl auto requires rows to be aligned to 4 bytes
     * but grayscale glyphs require 1 byte and don't have any padding
     * This setting tells GL that the texture is tightly packed
     */
    /* Query the current value of Alignment */
    int alignment;
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &alignment);
    /* Set the alignment to 1 (for glyph) */
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    /* Create and Bind a Texture for this */
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    /* Character Bitmap --> Grayscale */
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* Reset the alignment */
    glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);

    Character ch = {
        texture,                                                                // Texture
        glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),        // Size
        glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),          // Bearing
        static_cast<unsigned int>(face->glyph->advance.x)                       // Horizontal Advance 
    };

    return ch;
}

VOID_NAMESPACE_CLOSE
