// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Entity.h"

VOID_NAMESPACE_OPEN

ProjectEntity::ProjectEntity(Core::Project* project)
    : VoidObject()
    , m_Color(93, 150, 163)
    , m_Project(project)
{
}

void ProjectEntity::SetColor(const QColor& color)
{
    m_Color = color;
    emit updated();
}

QPixmap ProjectEntity::DefaultThumbnail() const
{
    QPixmap pix = QPixmap(QSize(400, 225));
    pix.fill(Qt::black);

    return pix;
}

VOID_NAMESPACE_CLOSE
