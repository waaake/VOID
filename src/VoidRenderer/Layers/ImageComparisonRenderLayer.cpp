// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* GLEW */
#include <GL/glew.h>

/* Internal */
#include "ImageComparisonRenderLayer.h"

VOID_NAMESPACE_OPEN

ImageComparisonRenderLayer::ImageComparisonRenderLayer()
    : m_TextureA(0)
    , m_TextureB(0)
{
    /* Data to be shared across with the Renderer */
    m_ImageData = new Renderer::ImageComparisonRenderData;
    /* Image Texture Renderer */
    m_ImageRenderer = new ImageComparisonRenderGear;
    /* Swiper on the screen when comparing */
    m_SwiperRenderer = new SwiperRenderGear;

    /* Setup the data with default values */
    m_ImageData->exposure = 0.f;
    m_ImageData->gamma = 1.f;
    m_ImageData->gain = 1.f;

    m_ImageData->channelMode = 5; /* RGBA */
    m_ImageData->blendMode = 0;
    m_ImageData->comparisonMode = 0;

    /* Swiper */
    m_ImageData->swipeX = 0.5f;
    m_ImageData->offset = 0.f;
}

ImageComparisonRenderLayer::~ImageComparisonRenderLayer()
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
    if (m_SwiperRenderer)
    {
        delete m_SwiperRenderer;
        m_SwiperRenderer = nullptr;
    }

    /* Destroy the bound texture */
    glDeleteTextures(1, &m_TextureA);
    glDeleteTextures(1, &m_TextureB);
}

void ImageComparisonRenderLayer::Initialize()
{
    m_InternalFormatA = 0;
    m_InternalFormatB = 0;

    /* Initialize the Image Render Component */
    m_ImageRenderer->Initialize();
    m_SwiperRenderer->Initialize();

    /* Gen Texture for Render */
    glGenTextures(1, &m_TextureA);
    glGenTextures(1, &m_TextureB);

    /* Update the texture ID on the Image Data */
    m_ImageData->textureA = m_TextureA;
    m_ImageData->textureB = m_TextureB;
}

void ImageComparisonRenderLayer::InitTextureA(const SharedPixels& image)
{
    glTexImage2D(GL_TEXTURE_2D, 0, image->GLInternalFormat(), image->Width(), image->Height(), 0, image->GLFormat(), image->GLType(), nullptr);
    m_InternalFormatA = image->GLInternalFormat();
}

void ImageComparisonRenderLayer::InitTextureB(const SharedPixels& image)
{
    glTexImage2D(GL_TEXTURE_2D, 0, image->GLInternalFormat(), image->Width(), image->Height(), 0, image->GLFormat(), image->GLType(), nullptr);
    m_InternalFormatB = image->GLInternalFormat();
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

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image->Width(), image->Height(), image->GLFormat(), image->GLType(), image->Pixels());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    /* Update the colorspace on the Image Data */
    m_ImageData->inputColorSpaceA = static_cast<int>(image->InputColorSpace());
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

    // glTexImage2D(GL_TEXTURE_2D, 0, image->GLFormat(), image->Width(), image->Height(), 0, image->GLFormat(), image->GLType(), image->Pixels());
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image->Width(), image->Height(), image->GLFormat(), image->GLType(), image->Pixels());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    /* Update the colorspace on the Image Data */
    m_ImageData->inputColorSpaceB = static_cast<int>(image->InputColorSpace());
}

void ImageComparisonRenderLayer::Render(const glm::mat4& projection)
{
    /* Update the Data for Render */
    m_ImageData->projection = projection;

    /* Cast the data into const void* to pass it on to the renderers */
    const void* data = static_cast<const void*>(m_ImageData);

    /* And Proceed for Render */
    m_ImageRenderer->Render(data);

    /* If we're in Wipe Compare Mode -> Show the swiper */
    if (m_ImageData->comparisonMode == 1)
    {
        m_SwiperRenderer->Render(data);
    }
}

VOID_NAMESPACE_CLOSE
