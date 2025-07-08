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
     * Main Render Call
     * This invokes the PreDraw first and check the return status to invoke Draw or not
     * Then the PostDraw is invoked always
     */
    // void Render(const RenderKit& data);

protected:
    /**
     * Setup Array Buffers
     * Initialize the Array Buffers to be used in the program
     */
    virtual void SetupBuffers() = 0;

    // /**
    //  * Pre-Draw Call
    //  * Setup anything which is required before drawing anything on the screen
    //  */
    // virtual bool PreDraw() = 0;

    // /**
    //  * The Main Draw Call
    //  * This is invoked if the PreDraw is successful
    //  * The data to be drawn is passed to it
    //  */
    // virtual void Draw(const RenderKit& data) = 0;

    // /**
    //  * The Post Draw Call
    //  * Anything to be cleaned up after the draw is completed can be done here
    //  * This method always gets called even if the draw isn't called this will be
    //  */
    // virtual void PostDraw() = 0;

};

VOID_NAMESPACE_CLOSE

#endif // _VOID_BASIC_RENDER_GEAR_H