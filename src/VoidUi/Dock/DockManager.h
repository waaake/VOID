#ifndef _VOID_DOCK_MANAGER_H
#define _VOID_DOCK_MANAGER_H

/* STD */
#include <unordered_map>

/* Qt */
#include <QObject>
#include <QWidget>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

struct DockStruct
{
    /* Id of the Widget in the current Docks */
    int id;

    /* The Registered Widget */
    QWidget* widget;

    /* Name of the Widget to appear on the dock and also in the menu */
    std::string name;
};

class DockManager : public QObject
{
    Q_OBJECT

private:
    DockManager(QObject* parent = nullptr) : QObject(parent) {}

public:
    static DockManager& Instance();

    ~DockManager();

    /* Add a widget to the Dock Manager to be able to Dock */
    int RegisterDock(QWidget* widget, const std::string& text);

    /**
     * Returns the Dock widget information at any given index
     * If the index is not available, then an invalid DockStruct Entry is returned
     */
    DockStruct Dock(int index) const;

    const std::unordered_map<int, DockStruct>& AvailableDocks() const { return m_Docks; }

    /* Disable Copy */
    DockManager(const DockManager&) = delete;
    DockManager& operator=(const DockManager&) = delete;

    /* Disable Move */
    DockManager(DockManager&&) = delete;
    DockManager& operator=(DockManager&&) = delete;

signals:
    void updated();

private: /* Members */
    std::unordered_map<int, DockStruct> m_Docks;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_DOCK_MANAGER_H
