// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Entity.h"
#include "VoidObjects/Project/Project.h"

VOID_NAMESPACE_OPEN

ProjectEntity::ProjectEntity(Core::Project* project)
    : ProjectEntity(Qt::black, project)
{
}

ProjectEntity::ProjectEntity(const QColor& color, Core::Project* project)
    : VoidObject(project)
    , m_Color(color)
    , m_Project(project)
{
}

void ProjectEntity::SetColor(const QColor& color)
{
    m_Color = color;
    emit updated();
}

QPixmap ProjectEntity::DefaultThumbnail()
{
    QPixmap pix = QPixmap(QSize(400, 225));
    pix.fill(Qt::black);
    return pix;
}

VOID_NAMESPACE_CLOSE
