// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_BASIC_RENDER_GEAR_H
#define _VOID_BASIC_RENDER_GEAR_H

/* STD */
#include <memory>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

/**
 * The Render Data which will be used by the RenderGear component
 */
class RenderKit
{
public:
    // template <typename Ty>
    // RenderKit(Ty* data) : raw(static_cast<void*>(data)) {}

    template <typename Ty>
    RenderKit(std::shared_ptr<Ty> data) : shared(std::static_pointer_cast<void>(data)) {}

    std::shared_ptr<void> Get() const { return shared; }

private: /* Members */
    /* Raw Pointer Data */
    // void* raw;
    /* Shared Pointer Data */
    std::shared_ptr<void> shared;

};

/**
 * @brief Denotes the type of PixelBuffer to be bound in the current context
 * 
 * A refers to the Pixel Buffer for Texture A
 * B refers to the Pixel Buffer for Texture B
 */
enum class PixelBuffer
{
    A,
    B
};

/**
 * A Basic Render Gear:
 * Composits into the Main Render as a component which draws on Screen
 * This could draw anything on the screen with the Provided Data
 * 
 */
class RenderGear
{
public:
    /**
     * Initializes the shaders and the internals (Vertex Array Objects | Vertex Buffer Objects | Element/Index Buffer Objects)
     * This method gets called from the renderer after the GL context has been initialized
     */
    virtual void Initialize() = 0;

    /**
     * Reinitializes the shaders and internals (Vertex Array Objects | Vertex Buffer Objects | Element/Index Buffer Objects)
     */
    virtual void Reinitialize() = 0;

    /**
     * Main Render Call
     * This invokes the PreDraw first and check the return status to invoke Draw or not
     * Then the PostDraw is invoked always
     */
    void Render(const void* data);

    /**
     * @brief (Re)Allocates the internal PIXEL buffers with the provided size.
     * 
     * @param size Size of the allocation for the buffer.
     * @param buffer The Pixel buffer to allocate to.
     */
    virtual void ReallocatePixelBuffer(std::size_t size, const PixelBuffer& buffer);

    /**
     * @brief Binds the next index of the Pixel buffer for recieving data.
     * 
     * @param buffer The pixel buffer to bind.
     */
    virtual void RebindPixelBuffer(const PixelBuffer& buffer);

    /**
     * @brief Copy the data from the source to the pixel buffer.
     * 
     * @param data The data to be uploaded on the buffer to the GPU for rendering.
     * @param size Size of the data to be copied.
     */

    virtual void WritePixelData(const void* data, std::size_t size);
    /**
     * @brief Unbinds the active pixel buffer
     * 
     */
    virtual void UnbindPixelBuffer();

protected:
    /**
     * Setup Array Buffers
     * Initialize the Array Buffers to be used in the program
     */
    virtual void SetupBuffers() = 0;

    /**
     * Pre-Draw Call
     * Setup anything which is required before drawing anything on the screen
     */
    virtual bool PreDraw() = 0;

    /**
     * The Main Draw Call
     * This is invoked if the PreDraw is successful
     * The data to be drawn is passed to it
     */
    virtual void Draw(const void* data) = 0;

    /**
     * The Post Draw Call
     * Anything to be cleaned up after the draw is completed can be done here
     * This method always gets called even if the draw isn't called this will be
     */
    virtual void PostDraw() = 0;

};

VOID_NAMESPACE_CLOSE

#endif // _VOID_BASIC_RENDER_GEAR_H
