// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* GLEW */
#include <GL/glew.h>

/* GLM */
#include <glm/gtc/type_ptr.hpp>

/* Internal */
#include "ImageComparisonRenderLayer.h"

VOID_NAMESPACE_OPEN

ImageComparisonRenderLayer::ImageComparisonRenderLayer()
    : m_Exposure(0.f)
    , m_Gamma(1.f)
    , m_Gain(1.f)
    , m_ChannelMode(5) /* RGBA */
    , m_BlendMode(0)
    , m_ComparisonMode(0)
    , m_SwipeX(0.5f)
    , m_VAO(0)
    , m_VBO(0)
    , m_IBO(0)
    , m_PBOIndexA(0)
    , m_PBOIndexB(0)
    , m_UProjection(-1)
    , m_UTextureA(-1)
    , m_UTextureB(-1)
    , m_UExposure(-1)
    , m_UGamma(-1)
    , m_UGain(-1)
    , m_UChannelMode(-1)
    , m_UComparisonMode(-1)
    , m_UBlendMode(-1)
    , m_USwipeX(-1)
    , m_UInputColorSpaceA(-1)
    , m_UInputColorSpaceB(-1)
    , m_TextureA(0)
    , m_TextureB(0)
{
}

ImageComparisonRenderLayer::~ImageComparisonRenderLayer()
{
    /* Destroy the bound texture */
    glDeleteTextures(1, &m_TextureA);
    glDeleteTextures(1, &m_TextureB);

    if (m_Shader)
    {
        delete m_Shader;
        m_Shader = nullptr;
    }
}

void ImageComparisonRenderLayer::Reset()
{
    m_InternalFormatA = 0;
    m_InternalFormatB = 0;
}

void ImageComparisonRenderLayer::Initialize()
{
    Reset();

    /* Initialize the Image Render Component */
    m_Shader = new ImageComparisonShaderProgram;

    /* Initialize the Shaders */
    m_Shader->Initialize();

    /* Initialize the Array Buffers */
    SetupBuffers();

    /* Load all the locations for uniforms */
    m_UProjection = glGetUniformLocation(m_Shader->ProgramId(), "uMVP");
    m_UTextureA = glGetUniformLocation(m_Shader->ProgramId(), "uTexture");
    m_UTextureB = glGetUniformLocation(m_Shader->ProgramId(), "uTextureB");
    m_UExposure = glGetUniformLocation(m_Shader->ProgramId(), "exposure");
    m_UGamma = glGetUniformLocation(m_Shader->ProgramId(), "gamma");
    m_UGain = glGetUniformLocation(m_Shader->ProgramId(), "gain");
    m_UChannelMode = glGetUniformLocation(m_Shader->ProgramId(), "channelMode");
    m_UComparisonMode = glGetUniformLocation(m_Shader->ProgramId(), "comparisonMode");
    m_UBlendMode = glGetUniformLocation(m_Shader->ProgramId(), "blendMode");
    m_USwipeX = glGetUniformLocation(m_Shader->ProgramId(), "swipeX");
    m_UInputColorSpaceA = glGetUniformLocation(m_Shader->ProgramId(), "inputColorSpaceA");
    m_UInputColorSpaceB = glGetUniformLocation(m_Shader->ProgramId(), "inputColorSpaceB");

    /* Gen Texture for Render */
    glGenTextures(1, &m_TextureA);
    glGenTextures(1, &m_TextureB);
}

void ImageComparisonRenderLayer::InitTextureA(const SharedPixels& image)
{
    glTexImage2D(GL_TEXTURE_2D, 0, image->GLInternalFormat(), image->Width(), image->Height(), 0, image->GLFormat(), image->GLType(), nullptr);
    m_InternalFormatA = image->GLInternalFormat();

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_PBOs_A[0]);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, image->FrameSize(), nullptr, GL_STREAM_DRAW);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_PBOs_A[1]);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, image->FrameSize(), nullptr, GL_STREAM_DRAW);
}

void ImageComparisonRenderLayer::InitTextureB(const SharedPixels& image)
{
    glTexImage2D(GL_TEXTURE_2D, 0, image->GLInternalFormat(), image->Width(), image->Height(), 0, image->GLFormat(), image->GLType(), nullptr);
    m_InternalFormatB = image->GLInternalFormat();

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_PBOs_B[0]);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, image->FrameSize(), nullptr, GL_STREAM_DRAW);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_PBOs_B[1]);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, image->FrameSize(), nullptr, GL_STREAM_DRAW);
}

void ImageComparisonRenderLayer::SetImageA(const SharedPixels& image)
{
    /* Nothing to load */
    if (!image)
        return;

    /* Bind the Generated texture for Render */
    glBindTexture(GL_TEXTURE_2D, m_TextureA);
    /**
     * Load the image data onto the Texture 2D
     */
    if (m_InternalFormatA != image->GLInternalFormat())
        InitTextureA(image);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_PBOs_A[m_PBOIndexA]);
    m_PBOIndexA = (m_PBOIndexA + 1) % 2;

    if (void* iptr = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY))
    {
        memcpy(iptr, image->Pixels(), image->FrameSize());
        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    }

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image->Width(), image->Height(), image->GLFormat(), image->GLType(), 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    /* Update the colorspace on the Image Data */
    m_InputColorSpaceA = static_cast<int>(image->InputColorSpace());
}

void ImageComparisonRenderLayer::SetImageB(const SharedPixels& image)
{
    /* Nothing to load */
    if (!image)
        return;

    /* Bind the Generated texture for Render */
    glBindTexture(GL_TEXTURE_2D, m_TextureB);
    /**
     * Load the image data onto the Texture 2D
     */
    if (m_InternalFormatB != image->GLInternalFormat())
        InitTextureB(image);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_PBOs_B[m_PBOIndexB]);
    m_PBOIndexB = (m_PBOIndexB + 1) % 2;

    if (void* iptr = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY))
    {
        memcpy(iptr, image->Pixels(), image->FrameSize());
        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    }

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image->Width(), image->Height(), image->GLFormat(), image->GLType(), 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    /* Update the colorspace on the Image Data */
    m_InputColorSpaceB = static_cast<int>(image->InputColorSpace());
}

void ImageComparisonRenderLayer::Render(const glm::mat4& projection)
{
    /* Update the Data for Render */
    m_Projection = projection;

    if (PreDraw())
        Draw();

    PostDraw();
}

void ImageComparisonRenderLayer::ReinitShaderProgram()
{
    /* Re-Initialize the Shader */
    m_Shader->Reinitialize();

    /* Re-Load all the locations for uniforms */
    m_UProjection = glGetUniformLocation(m_Shader->ProgramId(), "uMVP");
    m_UTextureA = glGetUniformLocation(m_Shader->ProgramId(), "uTexture");
    m_UTextureB = glGetUniformLocation(m_Shader->ProgramId(), "uTextureB");
    m_UExposure = glGetUniformLocation(m_Shader->ProgramId(), "exposure");
    m_UGamma = glGetUniformLocation(m_Shader->ProgramId(), "gamma");
    m_UGain = glGetUniformLocation(m_Shader->ProgramId(), "gain");
    m_UChannelMode = glGetUniformLocation(m_Shader->ProgramId(), "channelMode");
    m_UComparisonMode = glGetUniformLocation(m_Shader->ProgramId(), "comparisonMode");
    m_UBlendMode = glGetUniformLocation(m_Shader->ProgramId(), "blendMode");
    m_USwipeX = glGetUniformLocation(m_Shader->ProgramId(), "swipeX");
    m_UInputColorSpaceA = glGetUniformLocation(m_Shader->ProgramId(), "inputColorSpaceA");
    m_UInputColorSpaceB = glGetUniformLocation(m_Shader->ProgramId(), "inputColorSpaceB");
}

void ImageComparisonRenderLayer::SetupBuffers()
{
    /**
     * Quad vertices with texture coords
     * Create Vertex Attrib Object and Vertex Buffer Objects
     */
    float vertices[16] = {
        // Positions  // Texture Coords
        -1.f, -1.f,  0.f,  1.f,
         1.f, -1.f,  1.f,  1.f,
         1.f,  1.f,  1.f,  0.f,
        -1.f,  1.f,  0.f,  0.f,
    };

    /**
     * Index/Element Buffer indices
     * Tells GL how to draw the triangles
     */
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    /**
     * Generate the vertex array object names
     * Generate 1 array
     */
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_IBO);
    glBindVertexArray(m_VAO);

    /* Bind Buffers */
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    /* Setup Vertex Attribs */
    /* Layout location 0 */
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    /* Layout location 1 */
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    /* Pixel Buffer Objects */
    glGenBuffers(2, m_PBOs_A);
    glGenBuffers(2, m_PBOs_B);

    /* Unbind */
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    glBindVertexArray(0);
}

bool ImageComparisonRenderLayer::PreDraw()
{
    /* Use the Shader Program */
    m_Shader->Bind();

    /* Bind the Vertex Array */
    glBindVertexArray(m_VAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    return true;
}

void ImageComparisonRenderLayer::Draw()
{
    glActiveTexture(GL_TEXTURE0);
    /* Bind the Generated texture for Render */
    glBindTexture(GL_TEXTURE_2D, m_TextureA);

    /* Tell the shader what texture to use */
    glUniform1i(m_UTextureA, 0);

    /* Bind the texture for render */
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_TextureB);

    /* Tell this to the shader to use this texture as the second sampler2D */
    glUniform1i(m_UTextureB, 1);

    glUniformMatrix4fv(m_UProjection, 1, GL_FALSE, glm::value_ptr(m_Projection));

    /* Update the viewer properties to the shader */
    glUniform1f(m_UExposure, m_Exposure);
    glUniform1f(m_UGamma, m_Gamma);
    /**
     * Gain is the linear multiplier to amplify brigtness
     * Can be used befor or after exposure or gamma correction
     */
    glUniform1f(m_UGain, m_Gain);

    /**
     * Update the channels to be displayed on the renderer
     */
    glUniform1i(m_UChannelMode, m_ChannelMode);

    /**
     * Update the compare mode on the shader
     */
    glUniform1i(m_UComparisonMode, m_ComparisonMode);

    /**
     * Update the blend mode on the shader
     */
    glUniform1i(m_UBlendMode, m_BlendMode);

    /**
     * Update the swipe factor
     */
    glUniform1f(m_USwipeX, m_SwipeX);

    /**
     * Update the input colorspaces on the shader to ensure output is linear before applying the final
     * view tranform for the viewer
     */
    glUniform1i(m_UInputColorSpaceA, m_InputColorSpaceA);
    glUniform1i(m_UInputColorSpaceB, m_InputColorSpaceB);

    /**
     * Draw triangles as bound in the Index buffer as defined earlier
     *  3 ___ 2
     *   |  /|
     *   | / |
     *   |/__|
     *  0     1
     *
     * 0 - 1 - 2
     * 2 - 3 - 0
     */
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void ImageComparisonRenderLayer::PostDraw()
{
    /* Cleanup */
    /* Unbind texture */
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    m_Shader->Release();
}

VOID_NAMESPACE_CLOSE
