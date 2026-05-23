// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _GRID_RENDER_LAYER_H
#define _GRID_RENDER_LAYER_H

/* STD */
#include <vector>
#include <utility> // std::pair

/* Internal */
#include "Definition.h"
#include "PixReader.h"
#include "VoidRenderer/Core/RenderTypes.h"
#include "VoidRenderer/Programs/ImageShaderProgram.h"

VOID_NAMESPACE_OPEN

struct ImageData
{
    int width;
    int height;
    int colorspace;

    ImageData() : width(1), height(1), colorspace(0) {}
    ImageData(int width, int height, int colorspace) : width(width), height(height), colorspace(colorspace) {}
};

class GridRenderLayer
{
public:
    GridRenderLayer();
    ~GridRenderLayer();

    void Initialize();
    void Reset();

    void SetImages(const std::vector<SharedPixels>& images);

    inline void SetExposure(const float exposure) { m_Exposure = exposure; }
    inline void SetGamma(const float gamma) { m_Gamma = gamma; }
    inline void SetGain(const float gain) { m_Gain = gain; }

    inline void SetChannelMode(const int mode) { m_ChannelMode = mode; }
    inline void SetChannelMode(const Renderer::ChannelMode& mode) { m_ChannelMode = static_cast<int>(mode); }

    void SetRows(int rows);
    void SetColumns(int columns);

    inline int Rows() const { return m_Rows; }
    inline int Columns() const { return m_Columns; }

    void ReinitShaderProgram();
    void Render(const glm::mat4& projection, float width, float height);

private: /* Members */
    /* Render Attributes affecting how the image is displayed */
    float m_Exposure;
    float m_Gamma;
    float m_Gain;
    int m_ChannelMode;

    ImageShaderProgram m_Shader;

    int m_UProjection;
    int m_UTexture;
    int m_UExposure;
    int m_UGamma;
    int m_UGain;
    int m_UChannelMode;
    int m_UInputColorSpace;

    float m_CellWidth, m_CellHeight;
    int m_Rows, m_Columns;
    unsigned int m_VAO, m_VBO, m_EBO;

    std::vector<unsigned int> m_Textures;
    std::vector<ImageData> m_TexData;

private: /* Methods */
    void SetupBuffers();
};

VOID_NAMESPACE_CLOSE

#endif // _GRID_RENDER_LAYER_H
