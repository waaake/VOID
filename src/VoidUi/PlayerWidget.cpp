/* Internal */
#include "PlayerWidget.h"

/* Qt */
#include <QLayout>

VOID_NAMESPACE_OPEN

Player::Player(QWidget* parent)
    : QWidget(parent)
    , m_MFrameHandler(MissingFrameHandler::BLACK_FRAME)
{
    /* Init the Viewer Buffer */
    m_ViewBufferA = new ViewerBuffer("A");
    m_ViewBufferB = new ViewerBuffer("B");

    /* Set the Associated Color for Viewer Buffer B */
    m_ViewBufferB->SetColor(QColor(70, 180, 220));      // Blue

    /* The default buffer is Viewer Buffer A */
    m_ActiveViewBuffer = m_ViewBufferA;
    m_ViewBufferA->SetActive(true);

    /* Build the layout */
    Build();

    /* Connect Signals */
    Connect();
}

Player::~Player()
{
    /* Cleanup the buffers */
    if (m_ViewBufferA)
    {
        delete m_ViewBufferA;
        m_ViewBufferA = nullptr;
    }
    if (m_ViewBufferB)
    {
        delete m_ViewBufferB;
        m_ViewBufferB = nullptr;
    }

    m_ActiveViewBuffer = nullptr;
}

void Player::Connect()
{
    /* Timeslider - TimeChange -> Player - SetFrame */
    connect(m_Timeline, &Timeline::TimeChanged, this, &Player::SetFrame);

    /* ControlBar - ZoomChange -> Renderer - UpdateZoom */
    connect(m_ControlBar, &ControlBar::zoomChanged, m_Renderer, &VoidRenderer::UpdateZoom);
    /* ControlBar - MissingFrameHandler Change -> Player - SetMissingFrameHandler */
    connect(m_ControlBar, &ControlBar::missingFrameHandlerChanged, this, &Player::SetMissingFrameHandler);
    /* ControlBar - Viewer Buffer Switched -> Player - Set View Buffer */
    connect(m_ControlBar, &ControlBar::viewerBufferSwitched, this, &Player::SetViewBuffer);
}

void Player::Build()
{
    /* Base layout for the widget */
    QVBoxLayout* layout = new QVBoxLayout(this);

    /* Instantiate widgets */
    m_ControlBar = new ControlBar(m_ViewBufferA, m_ViewBufferB, this);
    m_Renderer = new VoidRenderer(this);
    m_Timeline = new Timeline(this);

    /**
     * The way how this renderer will be setup in UI is
     * First Row will have any controls related to the viewport and anything which can show any information
     * The second Row/section will be the one which holds the Renderer
     * Then comes the Timeslider which holds any controls for playback
     */
    layout->addWidget(m_ControlBar);
    layout->addWidget(m_Renderer);
    layout->addWidget(m_Timeline);

    /* Spacing */
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 2);
}

void Player::Clear()
{
    /**
     * Update the time range to be 0-1 ??
     * Clear the data from the player
     */
    m_Timeline->SetRange(0, 0);
    m_Renderer->Clear();
}

void Player::Load(const SharedMediaClip& media)
{
    /* Update what's currently being played on the viewer buffer */
    m_ActiveViewBuffer->Set(media);

    /* Viewer Buffer - Clip -> Player - Add Cache Frame */
    ConnectMediaClipToTimeline(media);

    /**
     * Once we have the image sequence,
     * First update the timeslider range,
     */
    m_Timeline->SetRange(media->FirstFrame(), media->LastFrame());
    /* Clear any cached frame markings from the timeline */
    m_Timeline->ClearCachedFrames();
    /* Then set the First Image on the Player */
    m_Renderer->Render(media->FirstImage());
}

void Player::Load(const SharedPlaybackTrack& track)
{
    /* Update what's currently being played on the viewer buffer */
    m_ActiveViewBuffer->Set(track);

    /**
     * Once we have the image sequence,
     * First update the timeslider range,
     */
    m_Timeline->SetRange(track->StartFrame(), track->EndFrame());
    /* Clear any cached frame markings from the timeline */
    m_Timeline->ClearCachedFrames();
    /* Then set the First Image on the Player */
    SetTrackFrame(m_Timeline->Frame());
}

void Player::Load(const SharedPlaybackSequence& sequence)
{
    /* Update what is being played on the Active Viewer Buffer */
    m_ActiveViewBuffer->Set(sequence);

    /**
     * Once we have the image sequence,
     * First update the timeslider range,
     */
    m_Timeline->SetRange(sequence->StartFrame(), sequence->EndFrame());
    /* Clear any cached frame markings from the timeline */
    m_Timeline->ClearCachedFrames();

    /* Render the current frame from the seqeunce */
    SetSequenceFrame(m_Timeline->Frame());

    VOID_LOG_INFO("SEQUENCE SET: Range: {0}--{1}", sequence->StartFrame(), sequence->EndFrame());
}

void Player::SetFrame(int frame)
{
    /**
     * Check what do we want to play
     * if currently playing component on the Active ViewerBuffer is Sequence, meaning that was latest set on it
     */
    if (m_ActiveViewBuffer->PlayingComponent() == ViewerBuffer::PlayableComponent::Sequence)
        return SetSequenceFrame(frame);
    else if (m_ActiveViewBuffer->PlayingComponent() == ViewerBuffer::PlayableComponent::Track)
        return SetTrackFrame(frame);

    return SetMediaFrame(frame);
}

void Player::SetSequenceFrame(int frame)
{
    /* Ensure We have Valid sequence with media to process before setting the frame */
    if (m_ActiveViewBuffer->GetSequence()->IsEmpty())
        return;

    /* Try to get the trackItem at a given frame in the current sequence */
    /**
     * Instead of fetching the image data directly from the sequence, fetch the trackItem at any given point
     * This for the time being (till we have a best case O(1) for finding a track item at any given available)
     * is a better approach as the nearest frame logic causes a recursion which could end up looping over the same entity
     * Multiple times in order to find the media first, then it's nearest frame and then the image data for that frame which is very costly
     */
    SharedTrackItem item = m_ActiveViewBuffer->TrackItem(frame);

    if (item)
        SetTrackItemFrame(item, frame);
}

void Player::SetTrackFrame(int frame)
{
    /* Ensure We have Valid sequence with media to process before setting the frame */
    if (m_ActiveViewBuffer->GetTrack()->IsEmpty())
        return;

    /* Try to get the trackItem at a given frame in the current sequence */
    /**
     * Instead of fetching the image data directly from the sequence, fetch the trackItem at any given point
     * This for the time being (till we have a best case O(1) for finding a track item at any given available)
     * is a better approach as the nearest frame logic causes a recursion which could end up looping over the same entity
     * Multiple times in order to find the media first, then it's nearest frame and then the image data for that frame which is very costly
     */
    SharedTrackItem item = m_ActiveViewBuffer->TrackItem(frame);

    if (item)
        SetTrackItemFrame(item, frame);
}

void Player::SetTrackItemFrame(SharedTrackItem item, const int frame)
{
    /**
     * This GetImage itself runs a check if Media.Contains(frame) as it has to offset values internally
     * based on where the trackitem is present in the sequence
     */
    VoidImageData* data = item->GetImage(frame);

    /* A standard frame which is available for any trackitem/media */
    if (data)
    {
        m_Renderer->Render(data);
    }
    else
    {
        /**
         * This maybe a case where the given frame does not exist for the track item
         * What happens next is based on how the missing frame handler is set
         */
        switch(m_MFrameHandler)
        {
            /* Show only black frame on the viewer */
            case MissingFrameHandler::BLACK_FRAME:
                m_Renderer->Clear();
                break;
            case MissingFrameHandler::ERROR_FRAME:
                m_Renderer->Clear();
                m_Renderer->SetMessage("Frame " + std::to_string(frame) + " not available.");
                break;
            case MissingFrameHandler::NEAREST:
                /**
                 * Recursively call the method but with the nearest available frame
                 * For any given frame, this recursion should happen only once as the nearest frame is a valid frame
                 * to read and render on the renderer
                 */
                SetTrackItemFrame(item, item->NearestFrame(frame));
                break;
        }
    }
}

void Player::SetMediaFrame(int frame)
{
    /* Ensure we have a valid media to process before setting the frame */
    if (m_ActiveViewBuffer->GetMediaClip()->Empty())
        return;

    /**
     * If the frame does not have any data, this could mean that the frame is missing
     * if the provided frame is in range of the Media
     * How such a case is handled is based on the MissingFrameHandler
     * This determines what to do when a frame data is not available
     *
     * ERROR: Display an error on the Viewport stating the frame is not available.
     * BLACK_FRAME: Display a black frame instead of anything else. No error is displayed.
     * NEAREST: Don't do anything here, as we continue to show the last frame which was rendered.
     */
    if (m_ActiveViewBuffer->GetMediaClip()->Contains(frame))
    {
        /* Read the image for the frame from the sequence and set it on the player */
        m_Renderer->Render(m_ActiveViewBuffer->GetMediaClip()->Image(frame));
    }
    else
    {
        switch(m_MFrameHandler)
        {
            case MissingFrameHandler::BLACK_FRAME:
                m_Renderer->Clear();
                break;
            case MissingFrameHandler::ERROR_FRAME:
                m_Renderer->Clear();
                m_Renderer->SetMessage("Frame " + std::to_string(frame) + " not available.");
                break;
            case MissingFrameHandler::NEAREST:
                /**
                 * Recursively call the method but with the nearest available frame
                 * For any given frame, this recursion should happen only once as the nearest frame is a valid frame
                 * to read and render on the renderer
                 */
                SetMediaFrame(m_ActiveViewBuffer->GetMediaClip()->NearestFrame(frame));
                break;
        }
    }
}

void Player::SetViewBuffer(const PlayerViewBuffer& buffer)
{
    /* Update the active buffer based on the provided buffer enum */
    if (buffer == PlayerViewBuffer::A)
    {
        m_ActiveViewBuffer = m_ViewBufferA;
        /* Set its Active state */
        m_ViewBufferA->SetActive(true);
        /* Reset the active state for the other buffer */
        m_ViewBufferB->SetActive(false);
    }        
    else
    {
        m_ActiveViewBuffer = m_ViewBufferB;
        /* Set its Active state */
        m_ViewBufferB->SetActive(true);
        /* Reset the active state for the other buffer */
        m_ViewBufferA->SetActive(false);
    }

    /* Clear the viewport */
    m_Renderer->Clear();

    /* Refresh the player with the updated content from the Buffer */
    Refresh();

    /* Update the frame range */
    SetRange(m_ActiveViewBuffer->StartFrame(), m_ActiveViewBuffer->EndFrame());
}

VOID_NAMESPACE_CLOSE
