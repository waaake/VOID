/* Internal */
#include "PlayerWidget.h"

/* Qt */
#include <QLayout>

VOID_NAMESPACE_OPEN

Player::Player(QWidget* parent)
    : QWidget(parent)
    , m_Media()
    , m_PlaySequence(false)
{
    /* Init the Sequence and empty ImageData */
    m_Sequence = std::make_shared<PlaybackSequence>();
    m_Image = new VoidImageData;

    /* Build the layout */
    Build();

    /* Connect Signals */
    Connect();
}

Player::~Player()
{
    if (m_Image && !m_Image->Empty())
    {
        m_Image->Free();
        /* 
         * TODO: Check why can't we delete this pointer ?? 
         * Mostly due to memcpy
         * Could use copy constructor or copy assignment ?
         */
        // delete m_Image;
    }
}

void Player::Connect()
{
    /* Timeslider - TimeChange -> Player - SetFrame */
    connect(m_Timeline, &Timeline::TimeChanged, this, &Player::SetFrame);

    /* ControlBar - ZoomChange -> Renderer - UpdateZoom */
    connect(m_ControlBar, &ControlBar::zoomChanged, m_Renderer, &VoidRenderer::UpdateZoom);
}

void Player::Build()
{
    /* Base layout for the widget */
    QVBoxLayout* layout = new QVBoxLayout(this);

    /* Instantiate widgets */
    m_ControlBar = new ControlBar(this);
    m_Renderer = new VoidRenderer(this);
    m_Timeline = new Timeline(this);

    /*
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
    /* Clear the sequence */
    m_Media = Media();

    /*
     * Update the time range to be 0-1 ??
     * Clear the data from the player
     */
    m_Timeline->SetRange(0, 0);
    m_Renderer->Clear();
}

void Player::Load(const Media& media)
{
    m_Media = media;

    /* 
     * Media is set on the Player
     * The play state should represent that
     */
    m_PlaySequence = false;

    /* Once we have the image sequence,
     * First update the timeslider range,
     */
    m_Timeline->SetRange(m_Media.FirstFrame(), m_Media.LastFrame());
    /* Clear any cached frame markings from the timeline */
    m_Timeline->ClearCachedFrames();
    /* Then set the First Image on the Player */
    m_Renderer->Render(m_Media.FirstImage());
}

void Player::Load(const SharedPlaybackSequence& sequence)
{
    /* 
     * Now that we have loaded a Sequence, the very first thing
     * is to update the play state to now play the Sequence
     */
    m_PlaySequence = true;

    /* Update the sequence to point at the provided sequence shared pointer */
    m_Sequence = sequence;

    /* Once we have the image sequence,
     * First update the timeslider range,
     */
    m_Timeline->SetRange(m_Sequence->StartFrame(), m_Sequence->EndFrame());
    /* Clear any cached frame markings from the timeline */
    m_Timeline->ClearCachedFrames();
    
    /* Render the current frame from the seqeunce */
    SetSequenceFrame(m_Timeline->Frame());

    VOID_LOG_INFO("SEQUENCE SET: Range: {0}--{1}", m_Sequence->StartFrame(), m_Sequence->EndFrame());
}

void Player::SetFrame(int frame)
{
    /* 
     * Check what do we want to play
     * if play sequence is not set to true, which means the media was set latest on the player
     */
    if (m_PlaySequence)
        return SetSequenceFrame(frame);
    
    return SetMediaFrame(frame);
}

void Player::SetSequenceFrame(int frame)
{
    /* Ensure We have Valid sequence with media to process before setting the frame */
    if (m_Sequence->IsEmpty())
        return;

    /* 
     * Update the internal pointer with the frame data from the media of the sequence
     * The method returns a boolean value indicating whether the frame had data or not
     * Accordingly, the frame gets set if it has some data
     */
    if (m_Sequence->GetImage(frame, m_Image))
        m_Renderer->Render(m_Image);
}

void Player::SetMediaFrame(int frame)
{
    /* Ensure we have a valid media to process before setting the frame */
    if (m_Media.Empty())
        return;

    /* Read the image for the frame from the sequence and set it on the player */
    m_Renderer->Render(m_Media.Image(frame));
}

VOID_NAMESPACE_CLOSE
