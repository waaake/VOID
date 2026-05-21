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
#include "VoidRenderer/Programs/GridShaderProgram.h"

VOID_NAMESPACE_OPEN

class GridRenderLayer
{
public:
    GridRenderLayer();
    ~GridRenderLayer();

    void Initialize();
    void Reset();

    void SetImages(const std::vector<SharedPixels>& images);

    void ReinitShaderProgram();
    void Render(const glm::mat4& projection, float width, float height);

private: /* Members */
    glm::mat4 m_Projection;

    GridShaderProgram m_Shader;

    int m_UProjection;
    int m_UTexture;

    float m_CellWidth, m_CellHeight;
    int m_Rows, m_Columns;
    unsigned int m_VAO, m_VBO, m_EBO;

    std::vector<unsigned int> m_Textures;
    // std::vector<unsigned int> m_Vaos;
    // std::vector<unsigned int> m_Vbos;
    // std::vector<unsigned int> m_Ebos;
    std::vector<std::pair<int, int>> m_TexSizes;

private: /* Methods */
    void SetupBuffers();
    glm::mat4 ModelViewProjectionMat(float width, float height, float offsetx, float offsety) const;
};

VOID_NAMESPACE_CLOSE

#endif // _GRID_RENDER_LAYER_H
