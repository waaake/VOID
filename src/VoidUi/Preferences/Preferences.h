// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_PREFERENCES_H
#define _VOID_PREFERENCES_H

/* Qt */
#include <QSettings>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

namespace Settings
{
    constexpr auto MissingFramesHandler = "player/missingFramesHandler";
}

class VoidPreferences : public QObject
{
    Q_OBJECT
public:
    static VoidPreferences& Instance()
    {
        static VoidPreferences instance;
        return instance;
    }

    VoidPreferences(QObject* parent = nullptr);
    ~VoidPreferences();

    inline QVariant GetSetting(const std::string key) const { return settings.value(key.c_str()); }
    inline void Set(const std::string key, const QVariant& value)
    {
        /* Setup values */
        settings.setValue(key.c_str(), value);
        /* and emit that something was changed */
        emit updated();
    }

    /* Helpers -> Exposing Setting Value natively */
    inline int GetMissingFrameHandler() const { return GetSetting(Settings::MissingFramesHandler).toInt(); }

signals:
    /**
     * The updated signal is invoked when any setting is changed
     * this ensures any component listening to any values can get notified when something has changed
     * for it to requery it's data
     *
     * Adding individual signal for a setting could be quite messy in the long run
     */
    void updated();

private:
    QSettings settings;

};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PREFERENCES_H
