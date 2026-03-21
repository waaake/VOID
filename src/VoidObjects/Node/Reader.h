// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Definition.h"
#include "Node.h"
#include "VoidObjects/Media/MediaClip.h"

VOID_NAMESPACE_OPEN

class ReadNode : public Core::Node
{
public:
    ReadNode(const SharedMediaClip& media);
    virtual ~ReadNode();

    std::string Class() const noexcept override { return "Read"; }
    BufferData Evaluate(v_frame_t frame);

private: /* Members */
    SharedMediaClip m_Media;
};

VOID_NAMESPACE_CLOSE
