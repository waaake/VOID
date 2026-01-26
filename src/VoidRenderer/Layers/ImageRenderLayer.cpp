// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* GLEW */
#include <GL/glew.h>

/* GLM */
#include <glm/gtc/type_ptr.hpp>

/* Internal */
#include "ImageRenderLayer.h"

VOID_NAMESPACE_OPEN

ImageRenderLayer::ImageRenderLayer()
    : m_Exposure(0.f)
    , m_Gamma(1.f)
    , m_Gain(1.f)
    , m_ChannelMode(5) /* RGBA */
    , m_InputColorSpace(0)
    , m_VAO(0)
    , m_VBO(0)
    , m_IBO(0)
    , m_PBOIndex(0)
    , m_UProjection(-1)
    , m_UTexture(-1)
    , m_UExposure(-1)
    , m_UGamma(-1)
    , m_UGain(-1)
    , m_UChannelMode(-1)
    , m_UInputColorSpace(-1)
    , m_Texture(0)
{
}

ImageRenderLayer::~ImageRenderLayer()
{
    /* Destroy the bound texture */
    glDeleteTextures(1, &m_Texture);

    if (m_Shader)
    {
        delete m_Shader;
        m_Shader = nullptr;
    }
}

void ImageRenderLayer::Reset()
{
    m_InternalFormat = 0;
}

void ImageRenderLayer::Initialize()
{
    Reset();

    /* Initialize the Image Render Component */
    m_Shader = new ImageShaderProgram;

    /* Initialize the Shaders */
    m_Shader->Initialize();

    /* Initialize the Array Buffers */
    SetupBuffers();

    /* Load all the locations for uniforms */
    m_UProjection = glGetUniformLocation(m_Shader->ProgramId(), "uMVP");
    m_UTexture = glGetUniformLocation(m_Shader->ProgramId(), "uTexture");
    m_UExposure = glGetUniformLocation(m_Shader->ProgramId(), "exposure");
    m_UGamma = glGetUniformLocation(m_Shader->ProgramId(), "gamma");
    m_UGain = glGetUniformLocation(m_Shader->ProgramId(), "gain");
    m_UChannelMode = glGetUniformLocation(m_Shader->ProgramId(), "channelMode");
    m_UInputColorSpace = glGetUniformLocation(m_Shader->ProgramId(), "inputColorSpace");

    /* Gen Texture for Render */
    glGenTextures(1, &m_Texture);
}

void ImageRenderLayer::ReinitBuffer(const SharedPixels& image)
{
    /* Init the bound tex with null image for Tex*/
    glTexImage2D(GL_TEXTURE_2D, 0, image->GLInternalFormat(), image->Width(), image->Height(), 0, image->GLFormat(), image->GLType(), nullptr);
    m_InternalFormat = image->GLInternalFormat();

    /* Re-alloc the pixel buffer as the texture size has changed */
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_PBOs[0]);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, image->FrameSize(), nullptr, GL_STREAM_DRAW);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_PBOs[1]);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, image->FrameSize(), nullptr, GL_STREAM_DRAW);
}

void ImageRenderLayer::SetImage(const SharedPixels& image)
{
    /* Nothing to load */
    if (!image)
        return;

    /* Bind the Generated texture for Render */
    glBindTexture(GL_TEXTURE_2D, m_Texture);

    /**
     * Load the image data onto the Texture 2D
     */
    if (m_InternalFormat != image->GLInternalFormat())
        ReinitBuffer(image);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_PBOs[m_PBOIndex]);
    m_PBOIndex = (m_PBOIndex + 1) % 2;

    /* Copy new pixels */
    if (void* iptr = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY))
    {
        memcpy(iptr, image->Pixels(), image->FrameSize());
        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    }

    // int size = 0;
    // glGetBufferParameteriv(GL_PIXEL_UNPACK_BUFFER, GL_BUFFER_SIZE, &size);
    // VOID_LOG_INFO("Size: {0}, FrameSize: {1}", size, image->FrameSize());
    // assert(size >= image->FrameSize());

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image->Width(), image->Height(), image->GLFormat(), image->GLType(), 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    m_InputColorSpace = static_cast<int>(image->InputColorSpace());
}

void ImageRenderLayer::Render(const glm::mat4& projection)
{
    /* Update the Data for Render */
    m_Projection = projection;

    if (PreDraw())
        Draw();

    PostDraw();
}

void ImageRenderLayer::ReinitShaderProgram()
{
    /* Re-Initialize the Shader */
    m_Shader->Reinitialize();

    /* Re-Load all the locations for uniforms */
    m_UProjection = glGetUniformLocation(m_Shader->ProgramId(), "uMVP");
    m_UTexture = glGetUniformLocation(m_Shader->ProgramId(), "uTexture");
    m_UExposure = glGetUniformLocation(m_Shader->ProgramId(), "exposure");
    m_UGamma = glGetUniformLocation(m_Shader->ProgramId(), "gamma");
    m_UGain = glGetUniformLocation(m_Shader->ProgramId(), "gain");
    m_UChannelMode = glGetUniformLocation(m_Shader->ProgramId(), "channelMode");
    m_UInputColorSpace = glGetUniformLocation(m_Shader->ProgramId(), "inputColorSpace");
}

void ImageRenderLayer::SetupBuffers()
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

    /* PBOs */
    glGenBuffers(2, m_PBOs);

    /* Unbind */
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    glBindVertexArray(0);
}

bool ImageRenderLayer::PreDraw()
{
    /* Use the Shader Program */
    m_Shader->Bind();

    /* Bind the Vertex Array */
    glBindVertexArray(m_VAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    return true;
}

void ImageRenderLayer::Draw()
{
    glActiveTexture(GL_TEXTURE0);
    /* Bind the Generated texture for Render */
    glBindTexture(GL_TEXTURE_2D, m_Texture);

    /* Tell the shader what texture to use */
    glUniform1i(m_UTexture, 0);

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
     * Update the input colorspace on the shader to ensure output is linear before applying the final
     * view tranform for the viewer
     */
    glUniform1i(m_UInputColorSpace, m_InputColorSpace);

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

void ImageRenderLayer::PostDraw()
{
    /* Cleanup */
    /* Unbind texture */
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    m_Shader->Release();
}

VOID_NAMESPACE_CLOSE
