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
    explicit ProjectEntity(Core::Project* project = nullptr);
    explicit ProjectEntity(const QColor& color, Core::Project* project = nullptr);
    virtual ~ProjectEntity() = default;

    Core::Project* Project() const { return m_Project; }

    virtual std::string Name() const = 0;
    virtual MFrameRange FrameRange() const = 0;
    virtual double Framerate() const = 0;
    virtual int Channels() const = 0;
    virtual QPixmap Thumbnail() = 0;

    virtual bool HasTags() const { return false; }
    virtual bool HasAudio() const { return false; }

    const QColor& Color() const { return m_Color; }
    void SetColor(const QColor& color);

signals:
    void updated();

protected:
    QPixmap DefaultThumbnail() const;

protected:
    QColor m_Color;

private:
    Core::Project* m_Project;
};

VOID_NAMESPACE_CLOSE

#endif // _PROJECT_ENTITY_H
