/* Internal */
#include "DockManager.h"

VOID_NAMESPACE_OPEN

DockManager& DockManager::Instance()
{
    static DockManager d;
    return d;
}

DockManager::~DockManager()
{
}

int DockManager::RegisterDock(QWidget* widget, const std::string& text)
{
    /* Current size determines the id of the next widget in place */
    int id = m_Docks.size();

    /* Add the Dock to the underlying struct */
    m_Docks[id] = {id, widget, text};

    /* Emit that the DockManager has received an update */
    emit updated();

    return id;
}

DockStruct DockManager::Dock(int index) const
{
    if (m_Docks.find(index) != m_Docks.end())
        return m_Docks.at(index);

    /* Invalid Dock Struct */
    return {-1, nullptr, ""};
}

VOID_NAMESPACE_CLOSE
