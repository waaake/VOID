// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* GLEW */
#include <GL/glew.h>

/* Internal */
#include "ImageRenderLayer.h"

VOID_NAMESPACE_OPEN

ImageRenderLayer::ImageRenderLayer()
    : m_Texture(0)
{
    /* Data to be shared across with the Renderer */
    m_ImageData = new Renderer::ImageRenderData;
    /* Image Texture Renderer */
    m_ImageRenderer = new ImageRenderGear;

    /* Setup the data with default values */
    m_ImageData->exposure = 0.f;
    m_ImageData->gamma = 1.f;
    m_ImageData->gain = 1.f;

    m_ImageData->channelMode = 5; /* RGBA */
}

ImageRenderLayer::~ImageRenderLayer()
{
    if (m_ImageData)
    {
        delete m_ImageData;
        m_ImageData = nullptr;
    }
    if (m_ImageRenderer)
    {
        delete m_ImageRenderer;
        m_ImageRenderer = nullptr;
    }

    /* Destroy the bound texture */
    glDeleteTextures(1, &m_Texture);
}

void ImageRenderLayer::Reset()
{
    m_InternalFormat = 0;
}

void ImageRenderLayer::Initialize()
{
    Reset();

    /* Initialize the Image Render Component */
    m_ImageRenderer->Initialize();

    /* Gen Texture for Render */
    glGenTextures(1, &m_Texture);

    /* Update the texture ID on the Image Data */
    m_ImageData->textureA = m_Texture;
}

void ImageRenderLayer::InitTexture(const SharedPixels& image)
{
    /* Init the bound tex with null image for Tex*/
    glTexImage2D(GL_TEXTURE_2D, 0, image->GLInternalFormat(), image->Width(), image->Height(), 0, image->GLFormat(), image->GLType(), nullptr);
    m_InternalFormat = image->GLInternalFormat();
    m_ImageRenderer->ReallocatePixelBuffer(image->FrameSize());
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
        InitTexture(image);

    m_ImageRenderer->RebindPixelBuffer();
    m_ImageRenderer->WritePixelData(image->Pixels(), image->FrameSize());
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image->Width(), image->Height(), image->GLFormat(), image->GLType(), 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    m_ImageRenderer->UnbindPixelBuffer();

    /* Update the colorspace on the Image Data */
    m_ImageData->inputColorSpace = static_cast<int>(image->InputColorSpace());
}

void ImageRenderLayer::Render(const glm::mat4& projection)
{
    /* Update the Data for Render */
    m_ImageData->projection = projection;

    /* And Proceed for Render */
    m_ImageRenderer->Render(static_cast<const void*>(m_ImageData));
}

VOID_NAMESPACE_CLOSE
