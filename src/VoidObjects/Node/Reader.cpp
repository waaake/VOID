// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Reader.h"

VOID_NAMESPACE_OPEN

ReadNode::ReadNode(const SharedMediaClip& media)
    : Core::Node(media->Name())
    , m_Media(media)
{
}

ReadNode::~ReadNode()
{
}

BufferData ReadNode::Evaluate(v_frame_t frame)
{
    SharedPixels image = m_Media->Image(frame);
    return BufferData(image->Pixels(), image->FrameSize(), image->GLType(), image->Channels(), image->Width(), image->Height());
}

VOID_NAMESPACE_CLOSE
