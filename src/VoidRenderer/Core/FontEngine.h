#ifndef _VOID_RENDERER_FONT_ENGINE_H
#define _VOID_RENDERER_FONT_ENGINE_H

/* STD */
#include <unordered_map>
#include <functional>

/* Freetype */
#include <ft2build.h>
#include FT_FREETYPE_H

/* GLM */
#include <glm/vec2.hpp>

/* Qt */
#include <QFile>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

/**
 * Drawing text with Freetype, each character is rasterized into a grayscale bitmap
 * and each of the bitmap is then uploaded as a separate texture
 * 
 */
struct Character
{
    /* Glyph Texture ID */
    unsigned int textureId;

    /* Size of the glyph */
    glm::ivec2 size;

    /* Offset from the baseline */
    glm::ivec2 bearing;

    /* Horizontal advance of the glyph */
    unsigned int advance;
};

/**
 * A Key designed to hold a Font Face and it's size
 * Since the texture also depends on the font size, the size plays
 * a vital role in mapping the Character against it's FT_ULong type
 */
struct FontKey
{
    FT_Face face;
    size_t size;

    bool operator==(const FontKey& other) const
    {
        return (face == face) && (size == size);
    }
};

/* map hash for the FontKey */
struct FontKeyHash
{
    std::size_t operator()(const FontKey& key) const
    {
        return std::hash<FT_Face>()(key.face) ^ std::hash<size_t>()(key.size << 1);
    }
};


/**
 * The Font Engine acts as a bridge beteen freetype and QFont
 * to allow loading font(s) from QResource and load them as FT_Face
 * This also acts as a store of textures where a corresponding FT_ULong character
 * for a FT_Face font's equivalent texture is stored for use/reuse across rendering
 */
class FontEngine
{
    /* No Construction externally */
    FontEngine();

public:
    static FontEngine& Instance()
    {
        static FontEngine engine_;
        return engine_;
    }

    ~FontEngine();

    /* Disable Copy */
    FontEngine(const FontEngine&) = delete;
    // FontEngine& operator=(const FontEngine&) = delete;

    /* Disable Move */
    FontEngine(FontEngine&&) = delete;
    FontEngine& operator=(FontEngine&&) = delete;

    /**
     * Returns a Freetype Font face based on the standard text from resource
     * The font's internal byte array is stored within the class and if that needs to be deleted
     * to free memory call DeleteFace with the FT_Face.
     */
    inline FT_Face GetStandardFace() { return GetFace(":resources/fonts/Roboto-Regular.ttf"); }

    /**
     * Returns a Freetype Font face based on the given font file and size
     * The font's internal byte array is stored within the class and if that needs to be deleted
     * to free memory call DeleteFace with the FT_Face.
     */
    FT_Face GetFace(const std::string& path);

    /**
     * Deletes the Font Face and also any Bytearray data that was stored as a reference to maintain
     * the font face
     */
    void DeleteFace(FT_Face face);

    /**
     * Retrieves the Character from the store, if present else creates and returns the character info
     */
    const Character& GetChar(FT_Face face, FT_ULong character);

    /**
     * Clear all of the Font Texture data
     */
    void ClearTextures();

private: /* Members */
    /* Font Library */
    FT_Library m_FtLib;

    /* Map holding the font path, to an equvalent FT_Face */
    std::unordered_map<std::string, FT_Face> m_FaceStore;

    /**
     * Each of the Font Face needs it's corresponding QByteArray Data to work
     * As the FontFace only references the data and does not copy it in any way
     * so if the reference of the Font Data dies, the FT_Face will stop working
     */
    std::unordered_map<FT_Face, QByteArray> m_FontData;

    /* Map holding the Face, the character to it's equivalent Texture ID */
    std::unordered_map<FontKey, std::unordered_map<FT_ULong, Character>, FontKeyHash> m_TextureStore;

private: /* Methods */
    bool CreateFace(const std::string& path);

    /* Load the Character info (Texture, size) */
    Character Load(FT_Face face, FT_ULong character) const;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_RENDERER_FONT_ENGINE_H
