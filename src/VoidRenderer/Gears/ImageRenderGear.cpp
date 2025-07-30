/* GLEW */
#include <GL/glew.h>

/* GLM */
#include <glm/gtc/type_ptr.hpp>

/* Internal */
#include "RenderTypes.h"
#include "ImageRenderGear.h"

VOID_NAMESPACE_OPEN

ImageRenderGear::ImageRenderGear()
    : m_VAO(0)
    , m_VBO(0)
    , m_IBO(0)
    , m_UProjection(-1)
    , m_UTexture(-1)
    , m_UExposure(-1)
    , m_UGamma(-1)
    , m_UGain(-1)
    , m_UChannelMode(-1)
    , m_UInputColorSpace(-1)
{
}

ImageRenderGear::~ImageRenderGear()
{
    if (m_Shader)
    {
        delete m_Shader;
        m_Shader = nullptr;
    }
}

void ImageRenderGear::Initialize()
{
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
}

void ImageRenderGear::Reinitialize()
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

void ImageRenderGear::SetupBuffers()
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

    /* Unbind */
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

bool ImageRenderGear::PreDraw()
{
    /* Use the Shader Program */
    m_Shader->Bind();

    /* Bind the Vertex Array */
    glBindVertexArray(m_VAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    return true;
}

void ImageRenderGear::Draw(const void* data)
{
    /* Cast the Data back to Renderable Image */
    const Renderer::ImageRenderData* d = static_cast<const Renderer::ImageRenderData*>(data);

    /* Cannot cast the data */
    if (!d)
        return;

    glActiveTexture(GL_TEXTURE0);
    /* Bind the Generated texture for Render */
    glBindTexture(GL_TEXTURE_2D, d->textureA);

    /* Tell the shader what texture to use */
    glUniform1i(m_UTexture, 0);

    // CalculateModelViewProjection();
    glUniformMatrix4fv(m_UProjection, 1, GL_FALSE, glm::value_ptr(d->projection));

    /* Update the viewer properties to the shader */
    glUniform1f(m_UExposure, d->exposure);
    glUniform1f(m_UGamma, d->gamma);
    /**
     * Gain is the linear multiplier to amplify brigtness
     * Can be used befor or after exposure or gamma correction
     */
    glUniform1f(m_UGain, d->gain);

    /**
     * Update the channels to be displayed on the renderer
     */
    glUniform1i(m_UChannelMode, d->channelMode);

    /**
     * Update the input colorspace on the shader to ensure output is linear before applying the final
     * view tranform for the viewer
     */
    glUniform1i(m_UInputColorSpace, d->inputColorSpace);

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

void ImageRenderGear::PostDraw()
{
    /* Cleanup */
    /* Unbind texture */
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    m_Shader->Release();
}

VOID_NAMESPACE_CLOSE
