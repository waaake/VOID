// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_APPLICATION_BRIDGE_H
#define _VOID_APPLICATION_BRIDGE_H

/* Qt */
#include <QObject>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Core/Task.h"

VOID_NAMESPACE_OPEN

class VOID_API VoidEngineBridge : public QObject
{
    Q_OBJECT
    VoidEngineBridge() = default;
public:
    static VoidEngineBridge& Instance();

    void QueueTask(Task* task) { emit taskQueued(task); }
    float Framerate() const { return m_Framerate; }
    void SetFramerate(float framerate) { m_Framerate = framerate; }
    std::string FramerateString() const;

    bool AudioEnabled() const { return m_Audio; }
    void ToggleAudio(bool enable) { m_Audio = enable; }

    bool IsDarkTheme() const { return m_Dark; }
    void SetLuminance(bool isdark) { m_Dark = isdark; }

signals:
    void taskQueued(Task*);

private:
    float m_Framerate { 24.f };
    bool m_Audio { false };
    bool m_Dark { false };
};

#define _EngineBridge VoidEngineBridge::Instance()

VOID_NAMESPACE_CLOSE

#endif // _VOID_APPLICATION_BRIDGE_H
