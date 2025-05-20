/* Internal */
#include "PlayerWidget.h"

/* Qt */
#include <QLayout>

VOID_NAMESPACE_OPEN

Player::Player(QWidget* parent)
    : QWidget(parent)
    , m_Media()
{
    /* Build the layout */
    Build();

    /* Connect Signals */
    Connect();
}

Player::~Player()
{
}

void Player::Connect()
{
    /* Timeslider - TimeChange -> Player - SetFrame */
    connect(m_Timeslider, &Timeslider::TimeChanged, this, &Player::SetFrame);
}

void Player::Build()
{
    /* Base layout for the widget */
    QVBoxLayout* layout = new QVBoxLayout(this);

    /* Instantiate widgets */
    m_Renderer = new VoidRenderer(this);
    m_Timeslider = new Timeslider(this);

    /*
     * The way how this renderer will be setup in UI is
     * First Row will have any controls related to the viewport and anything which can show any information
     * The second Row/section will be the one which holds the Renderer
     * Then comes the Timeslider which holds any controls for playback
     */
    layout->addWidget(m_Renderer);
    layout->addWidget(m_Timeslider);

    /* Spacing */
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
}

void Player::Clear()
{
    /* Clear the sequence */
    m_Media = Media();

    /*
     * Update the time range to be 0-1 ??
     * Clear the data from the player
     */
    m_Timeslider->SetRange(0, 0);
    m_Renderer->Clear();
}

void Player::Load(const Media& media)
{
    m_Media = media;

    /* Once we have the image sequence,
     * First update the timeslider range,
     */
    m_Timeslider->SetRange(m_Media.FirstFrame(), m_Media.LastFrame());
    /* Then set the First Image on the Player */
    m_Renderer->Render(m_Media.FirstImage());
}

void Player::SetFrame(int frame)
{
    /* Ensure we have a valid sequence to process before setting the frame */
    if (m_Media.Empty())
        return;

    /* Read the image for the frame from the sequence and set it on the player */
    // m_Renderer->Render(m_Sequence.GetImage(frame).GetImageData());
    m_Renderer->Render(m_Media.Image(frame));
}

VOID_NAMESPACE_CLOSE
