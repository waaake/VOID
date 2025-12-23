// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "ProjectBridge.h"
#include "VoidCore/Logging.h"
#include "VoidUi/Media/Browser.h"
#include "VoidUi/Project/Browser.h"
#include "VoidUi/QExtensions/MessageBox.h"
#include "VoidUi/Preferences/Preferences.h"

VOID_NAMESPACE_OPEN

ProjectBridge::ProjectBridge()
    : m_Bridge(MBridge::Instance())
{
}

ProjectBridge& ProjectBridge::Instance()
{
    static ProjectBridge instance;
    return instance;
}

SharedMediaClip ProjectBridge::OpenMedia()
{
    VoidMediaBrowser mediaBrowser;

    if (!mediaBrowser.Browse())
    {
        VOID_LOG_INFO("User Cancelled Opening Media");
        return nullptr;
    }

    m_Bridge.AddMedia(mediaBrowser.GetSelectedFile());
    return m_Bridge.LastMedia();
}

SharedMediaClip ProjectBridge::OpenMedia(const std::string& path)
{
    m_Bridge.AddMedia(path);
    return m_Bridge.LastMedia();
}

void ProjectBridge::ImportMedia()
{
    VoidMediaBrowser mediaBrowser;

    /* In case the dialog was not accepted */
    if (!mediaBrowser.Browse())
    {
        VOID_LOG_INFO("User Cancelled Browsing.");
        return;
    }

    /* Read the File from the FileDialog */
    m_Bridge.AddMedia(mediaBrowser.GetSelectedFile());
}

void ProjectBridge::ImportDirectory()
{
    VoidMediaBrowser mediaBrowser;

    if (!mediaBrowser.BrowseDirectory())
    {
        VOID_LOG_INFO("User Cancelled Importing");
        return;
    }

    /* Import the Media from the directory */
    m_Bridge.ImportDirectory(mediaBrowser.SelectedDirectory());
}

void ProjectBridge::ImportMedia(Project* project)
{
    VoidMediaBrowser mediaBrowser;

    /* In case the dialog was not accepted */
    if (!mediaBrowser.Browse())
    {
        VOID_LOG_INFO("User Cancelled Browsing.");
        return;
    }

    m_Bridge.SetCurrentProject(project);
    m_Bridge.AddMedia(mediaBrowser.GetSelectedFile());
}

void ProjectBridge::ImportDirectory(Project* project)
{
    VoidMediaBrowser mediaBrowser;

    if (!mediaBrowser.BrowseDirectory())
    {
        VOID_LOG_INFO("User Cancelled Importing");
        return;
    }

    m_Bridge.SetCurrentProject(project);
    m_Bridge.ImportDirectory(mediaBrowser.SelectedDirectory());
}

void ProjectBridge::New()
{
    /**
     * TODO: This needs to be implemented here as opposed to the MediaBridge
     */
    m_Bridge.NewProject();
}

void ProjectBridge::Open()
{
    VoidProjectBrowser browser;

    if (!browser.Browse())
    {
        VOID_LOG_INFO("User Cancelled Opening.");
        return;
    }

    VoidFileDescriptor d = browser.File();
    m_Bridge.Load(d.path);
}

void ProjectBridge::Open(const std::string& path)
{
    m_Bridge.Load(path);
}

void ProjectBridge::Close()
{
    /* Any modifications in the project which needs to be saved */
    if (!m_Bridge.CloseProject())
    {
        SaveMessageBox box;
        QMessageBox::StandardButton ret = box.Prompt();

        if (ret == QMessageBox::Save)
            Save();
        else if (ret == QMessageBox::Cancel)
            return;

        /* Force Close the project*/
        m_Bridge.CloseProject(true);
    }
}

void ProjectBridge::Close(Project* project)
{
    /* Any modifications in the project which needs to be saved */
    if (!m_Bridge.CloseProject(project))
    {
        /* Make this the current project before trying to prompt user for Saving */
        m_Bridge.SetCurrentProject(project);

        SaveMessageBox box;
        QMessageBox::StandardButton ret = box.Prompt();

        if (ret == QMessageBox::Save)
            Save();
        else if (ret == QMessageBox::Cancel)
            return;

        /* Force Close the project*/
        m_Bridge.CloseProject(project, true);
    }
}

void ProjectBridge::Save()
{
    if (!m_Bridge.Save())
    {
        VOID_LOG_INFO("Not able to save project. Opening Save As Dialog...");

        VoidProjectBrowser browser;

        if (!browser.Save())
        {
            VOID_LOG_INFO("User Cancelled Saving.");
            return;
        }

        VoidFileDescriptor d = browser.File();
        m_Bridge.Save(d.path, d.name, d.type);
    }
}

void ProjectBridge::SaveAs()
{
    VoidProjectBrowser browser;

    if (!browser.Save())
    {
        VOID_LOG_INFO("User Cancelled Saving.");
        return;
    }

    VoidFileDescriptor d = browser.File();
    m_Bridge.Save(d.path, d.name, d.type);
}

Project* ProjectBridge::ProjectAt(int row)
{
    QModelIndex index = m_Bridge.m_Projects->index(row, 0);
    return dynamic_cast<Project*>(m_Bridge.m_Projects->ProjectAt(index));
}

Project* ProjectBridge::ProjectAt(const QModelIndex& index)
{
    return dynamic_cast<Project*>(m_Bridge.m_Projects->ProjectAt(index));
}

VOID_NAMESPACE_CLOSE
