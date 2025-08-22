// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_UI_PROJECT_H
#define _VOID_UI_PROJECT_H

/* Qt */
#include <QUndoStack>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Project/Project.h"

VOID_NAMESPACE_OPEN

/**
 * A Project is essentially, just the store for Media and Media entities or collection
 * like Sequence and/or Playlist, it does not do anything apart from holding references
 * to the data via the MediaModel
 */
class Project : public Core::Project
{
    Q_OBJECT

public:
    Project(bool active = true, QObject* parent = nullptr);
    Project(const std::string& name, bool active = true, QObject* parent = nullptr);

    virtual ~Project();

    inline void PushCommand(QUndoCommand* command) { m_UndoStack->push(command); }
    inline QUndoStack* UndoStack() const { return m_UndoStack; }

private: /* Members */
    QUndoStack* m_UndoStack;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_UI_PROJECT_H
