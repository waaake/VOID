// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _PROJECT_ENTITY_H
#define _PROJECT_ENTITY_H

/* Qt */
#include <QColor>
#include <QPixmap>

/* Internal */
#include "Definition.h"
#include "FrameRange.h"
#include "VoidObjects/VoidObject.h"

VOID_NAMESPACE_OPEN

namespace Core {
    class Project;
} // namespace Core

class VOID_API ProjectEntity : public VoidObject
{
    Q_OBJECT
public:
    enum class EntityType
    {
        MEDIA_CLIP,
        MEDIA_SEQUENCE
    };

public:
    ProjectEntity(Core::Project* project = nullptr);
    Core::Project* Project() const { return m_Project; }

    // Common properties which needs to be present
    virtual std::string Name() const = 0;
    virtual std::string Extension() const = 0;
    virtual QPixmap Thumbnail() = 0;
    virtual double Framerate() const = 0;
    virtual MFrameRange Framerange() const = 0;
    virtual bool HasAudio() const = 0;
    virtual bool HasTags() const = 0;
    virtual int Channels() const = 0;
    QColor Color() const { return m_Color; }

    void SetColor(const QColor& color);

    virtual EntityType Type() const = 0;

signals:
    void updated();

protected:
    QColor m_Color;
    Core::Project* m_Project;

protected: /* Methods */
    QPixmap DefaultThumbnail() const;
};

VOID_NAMESPACE_CLOSE

#endif // _PROJECT_ENTITY_H
