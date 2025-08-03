// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* GLM */
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>     // for ortho/scale
#include <glm/gtc/type_ptr.hpp>             // for glm::value_ptr

/* Qt */
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>

/* Internal */
#include "VoidRenderer.h"
#include "VoidCore/ColorProcessor.h"
#include "VoidCore/Logging.h"

static const float MAX_ZOOM = 12.8;
static const float MIN_ZOOM = 0.1;

VOID_NAMESPACE_OPEN

VoidRenderer::VoidRenderer(QWidget* parent)
    : BasicRenderer(parent)
    , m_ImageA(nullptr)
    , m_ImageB(nullptr)
    , m_CompareMode(ComparisonMode::NONE)
    , m_BlendMode(BlendMode::UNDER)
    , m_SwipeX(0.5f)
    , m_SwipeOffet(0.f)
    , m_Swiping(false)
    , m_ZoomFactor(1.f)
    , m_TranslateX(0.f)
    , m_TranslateY(0.f)
    , m_Pressed(false)
    , m_Annotating(false)
    , m_Pan(0.f, 0.f)
{
    /* Renderer for Annotations */
    m_ImageRenderer = new ImageRenderLayer;
    m_ImageComparisonRenderer = new ImageComparisonRenderLayer;
    m_AnnotationsRenderer = new VoidAnnotationsRenderer;
}

VoidRenderer::~VoidRenderer()
{
    /* Delete the Render Layers */
    delete m_AnnotationsRenderer;
    delete m_ImageRenderer;
    delete m_ImageComparisonRenderer;

    /* Set Focus Policy for typing */
    setFocusPolicy(Qt::StrongFocus);
}

void VoidRenderer::Initialize()
{
    /* Initialize the Annotations Layer */
    m_AnnotationsRenderer->Initialize();

    /* Initialize the Image Render Layer */
    m_ImageRenderer->Initialize();
    /* Initialize the Comparison Image Render Layer */
    m_ImageComparisonRenderer->Initialize();

    /* (Re)Load Any textures if available */
    ReloadTextures();
}

void VoidRenderer::Draw()
{
    if (m_ImageA && !m_ImageA->Empty())
    {
        /* Calculate the Projection Matrix */
        CalculateModelViewProjection();

        /* Render Layers */
        if (m_CompareMode == ComparisonMode::NONE)
        {
            /* Render the Image Texture */
            m_ImageRenderer->Render(m_VProjection);

            /* Draw Annotations */
            if (m_Annotating)
            {
                /* Render the stokes with the projection */
                m_AnnotationsRenderer->Render(m_VProjection);
            }
        }
        else
        {
            /* Render Images with Comparison */
            m_ImageComparisonRenderer->Render(m_VProjection);
        }

        /* Exit Programs */
        glUseProgram(0);
    }
}

void VoidRenderer::mousePressEvent(QMouseEvent* event)
{
    /* Indicates that the mouse has been pressed */
    m_Pressed = true;

    /* Save the current mouse pos */
    #if _QT6    /* Qt6 Compat */
    m_LastMouse = event->position();
    #else
    m_LastMouse = event->pos();
    #endif // _QT6

    /* The Swipe only works when we're in Wipe mode for Comparison -- else the Swiper isn't shown */
    if (m_CompareMode == ComparisonMode::WIPE)
    {
        /* Convert the Swipe onto Qt Coordinates actual window width */
        float swipex = (m_ImageComparisonRenderer->SwipeX() + m_SwipeOffet) * width();

        /* A Buffer of 10 pixels to allow swiping */
        if (std::abs(m_LastMouse.x() - swipex) < 10.f)
        {
            /* Update to indicate that we're now swiping and not panning */
            m_Swiping = true;
        }
    }

    /* Annotating? */
    if (m_Annotating)
    {
        /**
         * Normalized x y points into Device Coordinates [-1, +1]
         */
        float glX = ((m_LastMouse.x() / float(width())) * 2.f) - 1.f;
        float glY = 1.f - (m_LastMouse.y() / float(height()) * 2.f);

        glm::vec2 p = InverseWorldPoint({glX, glY});

        /* and we have a brush active? */
        if (m_AnnotationsRenderer->DrawType() == DrawType::BRUSH)
        {
            /* Ensure that we have an annotation to draw on (creates if not yet present) */
            EnsureHasAnnotation();

            /* Add the Original Point */
            m_AnnotationsRenderer->DrawPoint(p);
        }
        else if (m_AnnotationsRenderer->DrawType() == DrawType::ERASER)
        {
            /* Remove a Stroke which contains the point */
            m_AnnotationsRenderer->EraseStroke(p);
        }
        else if (m_AnnotationsRenderer->DrawType() == DrawType::TEXT)
        {
            /* Ensure that we have an annotation to draw on (creates if not yet present) */
            EnsureHasAnnotation();

            /* Begin Typing at this point */
            m_AnnotationsRenderer->BeginTyping(p);

            /* Set Focus on the Widget to receive Key Events Correctly when typing */
            setFocus();
            grabKeyboard();
        }
    }
}

void VoidRenderer::mouseReleaseEvent(QMouseEvent* event)
{
    /* And indicates that the mouse was released */
    m_Pressed = false;
    m_Swiping = false;

    if (m_Annotating)
    {
        /* Commit the Stroke if we're annotating */
        m_AnnotationsRenderer->CommitStroke();

        /* Redraw */
        update();
    }
}

void VoidRenderer::mouseMoveEvent(QMouseEvent* event)
{
    /* Fetch the mouse position */
    #if _QT6    /* Qt6 Compat */
    int x = event->position().x();
    int y = event->position().y();
    #else
    int x = event->x();
    int y = event->y();
    #endif // _QT6

    /* If we're in annotation Mode don't pan the image or drag the slider */
    if (m_Annotating && m_Pressed)
    {
        /**
         * Normalized x y points into Device Coordinates [-1, +1]
         */
        float glX = ((x / float(width())) * 2.f) - 1.f;
        float glY = 1.f - (y / float(height()) * 2.f);

        /* Draw Textures on Screen */
        if (m_AnnotationsRenderer->DrawType() == DrawType::BRUSH)
        {
            /* Add the Point */
            m_AnnotationsRenderer->DrawPoint(InverseWorldPoint({glX, glY}));

            /* Redraw */
            update();
        }
        else if (m_AnnotationsRenderer->DrawType() == DrawType::ERASER)
        {
            /* Remove a Stroke which contains the point */
            m_AnnotationsRenderer->EraseStroke(InverseWorldPoint({glX, glY}));

            /* Redraw */
            update();
        }

        /* Return from Here as we're annotating */
        return;
    }

    /* The Swipe only works when we're in Wipe mode for Comparison -- else the Swiper isn't shown */
    if (m_CompareMode == ComparisonMode::WIPE)
    {
        /* Convert the Swipe onto Qt Coordinates actual window width */
        float swipex = (m_ImageComparisonRenderer->SwipeX() + m_SwipeOffet) * width();

        /* A Buffer of 10 pixels to allow swiping */
        if (std::abs(x - swipex) < 10.f)
        {
            /* Update the Mouse Cursor */
            setCursor(Qt::SizeHorCursor);
        }
        else
        {
            /* Restore the cursor */
            unsetCursor();
        }

        /* Check if we're Swiping */
        if (m_Swiping)
        {
            /* Update the Swipe in it's normalized system (0.f - +1.f)*/
            m_ImageComparisonRenderer->SetSwipeX(std::clamp(x / float(width()), 0.f, 1.f));

            /* Redraw the Texture */
            update();

            /* Return from here and not affect the pan */
            return;
        }
    }

    /**
     * If the mouse is currently Pressed
     * track how much we have moved it, this will translate into the panning factor
     * which moves the image around
     */
    if (m_Pressed)
    {
        #if _QT6    /* Qt6 Compat */
        Point delta = event->position() - m_LastMouse;
        /* Update the last mouse pos as well */
        m_LastMouse = event->position();
        #else
        Point delta = event->pos() - m_LastMouse;
        /* Update the last mouse pos as well */
        m_LastMouse = event->pos();
        #endif // _QT6

        /* Calculate the delta for the movement in screen view space */
        float dx = float(delta.x()) / width();
        float dy = float(delta.y()) / height();

        /**
         * Pan speed scales the movement to match the translated OpenGL normalized
         * coordinates which has happened over Screen-space units (or pixels)
         * This speed is also governed by how much we have zoomed in
         */
        float speed = 2.f / m_ZoomFactor;

        /**
         * Update the offset with which the swipe handle needs to be moved to maintain a 1:1
         * placement with the panned texture
         */
        m_SwipeOffet += dx;

        /**
         * Open GL expects the 0, 0 of the texture on the bottom left
         * where we're rendering the image upside down to compensate that
         * hence the negated delta y so that the mouse movement correctly corresponds
         * to the movement
         */
        m_Pan += glm::vec2(dx * speed, -dy * speed);

        /* Redraw the image texture, this time with the translation offset applied */
        update();
    }

    /* Update the X and Y Coordinates for the mouse movements */
    m_RenderStatus->SetMouseCoordinates(x, y);

    /* Fetch color values at the given point */
    float pixels[4]; /* R G B A*/

    /*
     * TODO: FIX the issue with offset in value
     * Currently there is an offset with which pixel values are read
     * The underlying issue is that we have a dock widget and the left side dock is shriking this widget
     * But then the glViewport(x, y, w, h) call is not happenning correctly causing the pixel values to get offset
     *
     * Fix is to play without the left dock, but is not very important at this point in development
     * We can come back to this at a later stage to figure out how Qt is handling the calls and come up with a fix accordingly
     */
    glReadPixels(x, height() - y, 1, 1, GL_RGBA, GL_FLOAT, pixels);

    /* Update the Pixel values on the Renderer Status bar */
    m_RenderStatus->SetColourValues(pixels[0], pixels[1], pixels[2], pixels[3]);
}

void VoidRenderer::wheelEvent(QWheelEvent* event)
{
    /* Normalize to OpenGL Coords */
    #if _QT6_COMPAT        /* Qt6 Compat */
    m_TranslateX = (2.f * event->position().x() / width()) - 1.f;
    m_TranslateY = 1.f - (2.f * event->position().y() / height());
    #else
    m_TranslateX = (2.f * event->pos().x() / width()) - 1.f;
    m_TranslateY = 1.f - (2.f * event->pos().y() / height());
    #endif // Qt6 Compat

    if (event->angleDelta().y() > 0)
    {
        /* +ve or Zoom In */
        m_ZoomFactor *= 1.1f;

        /* The zoom can be upto a max */
        m_ZoomFactor = std::min(m_ZoomFactor, MAX_ZOOM);
    }
    else
    {
        /* Zoom out */
        m_ZoomFactor /= 1.1f;

        /* And upto a minimum */
        m_ZoomFactor = std::max(m_ZoomFactor, MIN_ZOOM);
    }

    VOID_LOG_INFO("Zoom Level: {0}", m_ZoomFactor);

    /* Repaint */
    update();
}

void VoidRenderer::keyPressEvent(QKeyEvent* event)
{
    /* If we're annotating and typing */
    if (m_Annotating && m_AnnotationsRenderer->Typing())
    {
        HandleAnnotationTyping(event);
        /* No other operation while the typing is ongoing */
        return;
    }

    /* Base Key presses */
    BasicRenderer::keyPressEvent(event);
}

void VoidRenderer::Render(SharedPixels data)
{
    /* Update the image data */
    m_ImageA = data;
    /* Hide the Error Label */
    SetMessage("");

    /* We're no longer comparing */
    m_CompareMode = ComparisonMode::NONE;

    /* Clear Secondary Image Data*/
    m_ImageB = nullptr;

    /**
     * Update the render resolution
     * The resolution of the texture is not going to change in the draw (unless we apply a reformat to it)
     * So constantly redrawing this is just too ineffecient
     */
    if (m_ImageA)
    {
        m_RenderStatus->SetRenderResolution(m_ImageA->Width(), m_ImageA->Height());
    }

    /* When we Receive the data to be renderer, we also get the Annotation data pointer to be rendered */
    m_AnnotationsRenderer->SetAnnotation(nullptr);

    /* Load the Textures to be rendered */
    ReloadTextures();

    /* Trigger a Re-paint */
    update();
}

void VoidRenderer::Render(const SharedPixels& data, const SharedAnnotation& annotation)
{
    /* Update the image data */
    m_ImageA = data;
    /* Hide the Error Label */
    SetMessage("");

    /* We're no longer comparing */
    m_CompareMode = ComparisonMode::NONE;
    m_ImageComparisonRenderer->SetComparisonMode(m_CompareMode);

    /* Clear Secondary Image Data*/
    m_ImageB = nullptr;

    /**
     * Update the render resolution
     * The resolution of the texture is not going to change in the draw (unless we apply a reformat to it)
     * So constantly redrawing this is just too ineffecient
     */
    if (m_ImageA)
    {
        m_RenderStatus->SetRenderResolution(m_ImageA->Width(), m_ImageA->Height());
    }

    /* When we Receive the data to be renderer, we also get the Annotation data pointer to be rendered */
    m_AnnotationsRenderer->SetAnnotation(annotation);

    /* Load the Textures to be rendered */
    ReloadTextures();

    /* Trigger a Re-paint */
    update();
}

void VoidRenderer::Compare(SharedPixels first, SharedPixels second, ComparisonMode comparison, BlendMode blend)
{
    /* Update the image data */
    m_ImageA = first;
    m_ImageB = second;

    /* Update the Comparison Mode */
    m_CompareMode = comparison;
    /* Update the Blend Mode */
    m_BlendMode = blend;

    m_ImageComparisonRenderer->SetComparisonMode(m_CompareMode);
    m_ImageComparisonRenderer->SetBlendMode(blend);

    /* Hide the Error Label */
    SetMessage("");

    /**
     * Update the render resolution
     * The resolution of the texture is not going to change in the draw (unless we apply a reformat to it)
     * So constantly redrawing this is just too ineffecient
     */
    if (m_ImageA)
    {
        m_RenderStatus->SetRenderResolution(m_ImageA->Width(), m_ImageA->Height());
    }

    /* Load the Textures to be rendered */
    ReloadTextures();

    /* Trigger a Re-paint */
    update();
}

void VoidRenderer::Clear()
{
    /* Delete the reference to the Image Data */
    m_ImageA = nullptr;
    /* Clear the frame */
    ClearFrame();
    /* Hide the Error Label */
    SetMessage("");

    /*
     * Trigger a Re-paint
     * This will draw nothing but just clears COLOR and DEPTH
     */
    update();
}

void VoidRenderer::ZoomIn(float factor)
{
    m_ZoomFactor *= factor;
    /* Repaint */
    update();
}

void VoidRenderer::ZoomOut(float factor)
{
    m_ZoomFactor *= factor;
    /* Repaint */
    update();
}

void VoidRenderer::ZoomToFit()
{
    /* Reset all attributes contributing to zoom/pan */
    m_ZoomFactor = 1.f;
    m_TranslateX = 0.f;
    m_TranslateY = 0.f;

    /* Reset the panning as well */
    m_Pan = {0.f, 0.f};

    /* Reset the SwipeOffset to make it come to it's original place */
    m_SwipeOffet = 0.f;

    /* Repaint after the zoom attributes have been reset */
    update();
}

void VoidRenderer::UpdateZoom(const float zoom)
{
    m_ZoomFactor = zoom;

    /* Repaint */
    update();
}

void VoidRenderer::SetExposure(const float exposure)
{
    m_ImageRenderer->SetExposure(exposure);
    /* Redraw the texture */
    update();
}

void VoidRenderer::SetGamma(const float gamma)
{
    m_ImageRenderer->SetGamma(gamma);
    /* Redraw the texture */
    update();
}

void VoidRenderer::SetGain(const float gain)
{
    m_ImageRenderer->SetGain(gain);
    /* Redraw the texture */
    update();
}

void VoidRenderer::SetChannelMode(int mode)
{
    /* Update the channel mode for the Renderer */
    m_ImageRenderer->SetChannelMode(mode);

    /* Redraw the texture */
    update();
}

void VoidRenderer::SetColorDisplay(const std::string& display)
{
    /* Set the display on the Color Processor */
    ColorProcessor::Instance().SetDisplay(display);

    /* Reinit the Image Shaders */
    m_ImageRenderer->ReinitShaderProgram();
    m_ImageComparisonRenderer->ReinitShaderProgram();

    /* Once the Shaders are initialized with the Current OCIO Display -> Redraw */
    update();
}

void VoidRenderer::CalculateModelViewProjection()
{
    /**
     * No Image Data to Proceed
     */
    if (!m_ImageA || m_ImageA->Empty())
        return;

    /**
     * To ensure the image is of the correct aspect while render
     * Calculate the aspect of the current view (Renderer Width / Renderer Height)
     * And the aspect of the image being rendered
     */
    float viewAspect = (width() / WidthDivisor()) / (height() / HeightDivisor());
    float imageAspect = float(m_ImageA->Width()) / float((m_ImageA->Height() ? m_ImageA->Height() : 1));

    /* Find the overall scale of the image */
    glm::vec2 scale = (imageAspect > viewAspect) ? glm::vec2(1.f, viewAspect / imageAspect) : glm::vec2(imageAspect / viewAspect, 1.f);

    /**
     * Get the Model matrix,
     * This is how our image/model looks like as a 4x4 matrix
     */
    glm::mat4 model = glm::translate(glm::mat4(1.f), glm::vec3(m_Pan, 0.f));

    /* Update the model with the aspect and the zoom scale */
    model = glm::scale(model, glm::vec3(scale.x * m_ZoomFactor, scale.y * m_ZoomFactor, 1.f));

    /**
     * And the projection matrix of how it's supposed to be projected on the viewport
     * Holds the scaling and aspect
     */
    glm::mat4 projection = glm::ortho(-1.f, 1.f, -1.f, 1.f);

    /**
     * Calculate the model view prohection matrix
     * For any 2D Texture/image the transform from the camera is unity
     */
    m_VProjection = model * projection; // * unity view i.e. glm::mat4(1.f)

    /* Calculate the inverse projection as well */
    m_InverseProjection = glm::inverse(m_VProjection);
}

void VoidRenderer::ReloadTextures()
{
    /* Based on the Image Data available -> Load the Textures */
    /* Update Image Render Buffer */
    m_ImageRenderer->SetImage(m_ImageA);

    /* Set The Image Buffer with the Images */
    m_ImageComparisonRenderer->SetImageA(m_ImageA);
    m_ImageComparisonRenderer->SetImageB(m_ImageB);
}

void VoidRenderer::ToggleAnnotation(bool t)
{
    /* Update Annotation State */
    m_Annotating = t;
    /* Accordinly set the Mouse Pointer */
    ResetAnnotationPointer();
}

void VoidRenderer::SetAnnotationColor(const glm::vec3& color)
{
    /* Update the current color on the Annotation */
    m_AnnotationsRenderer->SetColor(color);
    /* Reset The Mouse Pointer */
    ResetAnnotationPointer();
}
void VoidRenderer::SetAnnotationColor(const QColor& color)
{
    /* Update the current color on the Annotation */
    m_AnnotationsRenderer->SetColor(color);
    /* Reset The Mouse Pointer */
    ResetAnnotationPointer();
}

void VoidRenderer::SetAnnotationSize(const float size)
{
    /* Update the Brush Size */
    m_AnnotationsRenderer->SetBrushSize(size);

    /* Ensure we set a valid size fot the text */
    if (size > 0)
        m_AnnotationsRenderer->SetFontSize(static_cast<size_t>(size));

    /* Reset the Mouse Pointer to reflect the brush size */
    ResetAnnotationPointer();
}

void VoidRenderer::SetAnnotationDrawType(const int type)
{
    /* Update the Draw Component */
    m_AnnotationsRenderer->SetDrawType(static_cast<Renderer::DrawType>(type));
    /* Update the mouse Pointer */
    ResetAnnotationPointer();
}

void VoidRenderer::ClearAnnotations()
{
    /* Clear the Annotation */
    m_AnnotationsRenderer->DeleteAnnotation();
    /* Redraw */
    update();

    /* Emit that the annotation has now been removed */
    emit annotationDeleted();
}

void VoidRenderer::ResetAnnotationPointer()
{
    /* Setup the Pointer for Annotation */
    if (!m_Annotating)
    {
        /* Reset the Mouse Pointer */
        unsetCursor();
        return;
    }

    /* Size to be Used for Brush and Eraser */
    int diameter = (m_AnnotationsRenderer->BrushSize() * 500) + 4; // 4 units for boundaries

    /* For each of the Annotation Tool -> Set the Mouse Pointer */
    if (m_AnnotationsRenderer->DrawType() == Renderer::DrawType::BRUSH)
    {
        /* The Pixmap holding the Brush */
        QPixmap p(diameter, diameter);
        p.fill(Qt::transparent);

        QPainter painter(&p);
        painter.setRenderHint(QPainter::Antialiasing);

        /* Fetch the current color from the Renderer */
        glm::vec3 color = m_AnnotationsRenderer->Color();
        /* Normalize to QColor RGB space (0 - 255) */
        painter.setBrush(QColor(color[0] * 255, color[1] * 255, color[2] * 255));
        painter.drawEllipse(0, 0, diameter, diameter);

        /* Hotspot at the center */
        int hotspot = diameter / 2;
        setCursor(QCursor(p, hotspot, hotspot));
    }
    else if (m_AnnotationsRenderer->DrawType() == Renderer::DrawType::ERASER)
    {
        /* The Pixmap holding the Brush */
        QPixmap p(diameter + 4, diameter + 4);
        p.fill(Qt::transparent);

        QPainter painter(&p);
        painter.setRenderHint(QPainter::Antialiasing);

        /* Fetch the current color from the Renderer */
        glm::vec3 color = m_AnnotationsRenderer->Color();
        painter.setPen(QPen(Qt::white, 2));
        painter.drawEllipse(2, 2, diameter, diameter);

        /* Hotspot at the center */
        int hotspot = diameter / 2;
        setCursor(QCursor(p, hotspot, hotspot));
    }
    else if (m_AnnotationsRenderer->DrawType() == Renderer::DrawType::TEXT)
    {
        /* Set as Text Cursor */
        setCursor(Qt::IBeamCursor);
    }
    else
    {
        unsetCursor();
    }
}

void VoidRenderer::HandleAnnotationTyping(QKeyEvent* event)
{
    /* Handle Key Switches */
    switch (event->key())
    {
        case Qt::Key_Backspace:
            m_AnnotationsRenderer->Backspace();
            break;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            m_AnnotationsRenderer->CommitText();
            /* Clear Focus from the Widget */
            clearFocus();
            releaseKeyboard();
            break;
        case Qt::Key_Escape:
            /* Discard anything that was being typed on screen */
            m_AnnotationsRenderer->DiscardText();
            /* Clear the Focus from the widget */
            clearFocus();
            releaseKeyboard();
            break;
        default:
            /* Add the Text on the Renderer Draft */
            m_AnnotationsRenderer->Type(event->text().toStdString());
    }

    /* Redraw */
    update();
}

void VoidRenderer::EnsureHasAnnotation()
{
    /* If we're not able to create a point -> that could be because the annotation isn't created yet */
    if (!m_AnnotationsRenderer->HasAnnotation())
    {
        /* Create a new Annotation for drawing over */
        SharedAnnotation annotation = m_AnnotationsRenderer->NewAnnotation();

        /* Emit the Created Annotation */
        emit annotationCreated(annotation);
    }
}

VOID_NAMESPACE_CLOSE
