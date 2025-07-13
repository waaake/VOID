/* GLM */
#include <glm/gtc/type_ptr.hpp> // for glm::value_ptr

/* Internal */
#include "AnnotationRenderer.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

VoidAnnotationsRenderer::VoidAnnotationsRenderer()
    : m_Drawing(false)
    , m_Typing(false)
    , m_Color(1.f, 1.f, 1.f)
    , m_Size(0.004f)
    , m_DrawType(Renderer::DrawType::NONE)
{
    m_Annotation = nullptr;

    /* Update the Annotation Data */
    m_AnnotationData = new Renderer::AnnotationRenderData;
}

VoidAnnotationsRenderer::~VoidAnnotationsRenderer()
{
    if (m_StrokeRenderer)
    {
        delete m_StrokeRenderer;
        m_StrokeRenderer = nullptr;
    }
    if (m_TextRenderer)
    {
        delete m_TextRenderer;
        m_TextRenderer = nullptr;
    }
    if (m_AnnotationData)
    {
        delete m_AnnotationData;
        m_AnnotationData = nullptr;
    }
}

Renderer::SharedAnnotation VoidAnnotationsRenderer::NewAnnotation()
{
    /* Create a new Annotation and return it */
    m_Annotation = std::make_shared<Renderer::Annotation>();

    /* Update the Annotation data with this Annotation */
    m_AnnotationData->annotation = m_Annotation;

    return m_Annotation;
}

bool VoidAnnotationsRenderer::DrawPoint(const glm::vec2& point)
{
    if (!m_Annotation)
        return false;

    /* The first point to be inserted for the annotation */
    if (!m_Drawing)
    {
        m_LastPoint = point;
        m_Drawing = true;

        /* Initialize the Stroke with the Color and Size */
        m_Annotation->current.color = m_Color;
        m_Annotation->current.thickness = m_Size;

        return true;
    }

    /* Next Set of Points */
    glm::vec2 direction = glm::normalize(point - m_LastPoint);
    glm::vec2 normal = glm::vec2(-direction.y, direction.x);

    float halfThickness = m_Size * 0.5f;

    /* The Annotated Vertex */
    Renderer::AnnotatedVertex v1 = { m_LastPoint + normal * halfThickness, normal};
    Renderer::AnnotatedVertex v2 = { m_LastPoint - normal * halfThickness, -normal};
    Renderer::AnnotatedVertex v3 = { point + normal * halfThickness, normal};
    Renderer::AnnotatedVertex v4 = { point - normal * halfThickness, -normal};

    /* Add to the Set of Vertices */
    m_Annotation->current.vertices.push_back(v1);
    m_Annotation->current.vertices.push_back(v2);
    m_Annotation->current.vertices.push_back(v3);
    m_Annotation->current.vertices.push_back(v4);

    /* Update the Last point */
    m_LastPoint = point;

    return true;
}

void VoidAnnotationsRenderer::EraseStroke(const glm::vec2& point)
{
    /* No Active Annotation */
    if (!m_Annotation)
        return;

    auto colliding = [&](const Renderer::Stroke& stroke) -> bool 
    {
        for (const Renderer::AnnotatedVertex& vertex: stroke.vertices)
        {
            if (glm::distance(vertex.position, point) < 0.05)   // If the point is closer than the threshold
                return true;
        }

        /* Not Colliding */
        return false;
    };

    /* Erase if the point collides on the stroke */
    m_Annotation->strokes.erase(std::remove_if(m_Annotation->strokes.begin(), m_Annotation->strokes.end(), colliding), m_Annotation->strokes.end());
}

void VoidAnnotationsRenderer::CommitStroke()
{
    /* If the Annotation hasn't been set */
    if (!m_Annotation)
        return;

    /* If the current Annotation is empty -> Don't add anything */
    if (m_Annotation->current.Empty())
        return;

    /* Move the Current annotation to the collection of strokes */
    m_Annotation->strokes.push_back(std::move(m_Annotation->current));

    /* Clear the Current Annotation */
    m_Annotation->current.Clear();
    m_Drawing = false;
}

void VoidAnnotationsRenderer::AddDemoText(const glm::vec2& point, const std::string& text)
{
    /* Don't have annotation ready */
    if (!m_Annotation)
        return;
    
    Renderer::RenderText r;
    r.color = m_Color;
    r.face = m_FontFace;
    r.position = point;
    r.text = text;

    m_Annotation->texts.push_back(std::move(r));
}

void VoidAnnotationsRenderer::Initialize()
{
    /* Create the Render Components */
    m_StrokeRenderer = new StrokeRenderGear;
    m_TextRenderer = new TextRenderGear;

    /* Initialize */
    m_StrokeRenderer->Initialize();
    m_TextRenderer->Initialize();

    /* Initialize Freetype */
    if (FT_Init_FreeType(&m_FtLib))
    {
        VOID_LOG_ERROR("Unable to Initialize FreeType.");
    }

    /* Setup Default Font */
    SetFontFace("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 40);
}

void VoidAnnotationsRenderer::SetFontFace(const std::string& path, const int size)
{
    /* Load the Font Face */
    if (FT_New_Face(m_FtLib, path.c_str(), 0, &m_FontFace))
    {
        VOID_LOG_ERROR("Not Able to load Font from file: {0}", path);
    }

    /* Set the Font Size */
    FT_Set_Pixel_Sizes(m_FontFace, 0, size);
}

void VoidAnnotationsRenderer::Render(const glm::mat4& projection)
{
    /* Don't have anything to draw yet */
    if (!m_Annotation)
        return;

    /* Update the projection before Drawing */
    m_AnnotationData->projection = projection;

    /* Annotation data to be sent to Render Gears */
    const void* data = static_cast<const void*>(m_AnnotationData);

    /* Render the Strokes */
    m_StrokeRenderer->Render(data);

    /* Render the text */
    m_TextRenderer->Render(data);
}

VOID_NAMESPACE_CLOSE
