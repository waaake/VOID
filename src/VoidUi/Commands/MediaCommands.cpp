/* Internal */
#include "MediaCommands.h"

VOID_NAMESPACE_OPEN

/* Media Import Command {{{ */

MediaImportCommand::MediaImportCommand(const std::string& path, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Path(path)
{
    /* The current index on which the Media Will be inserted */
    m_InsertIndex = MBridge::Instance().DataModel()->rowCount();

    setText("Import Media");
}

void MediaImportCommand::undo()
{
    /* Index at which the Media was inserted and now needs removal */
    QModelIndex index = MBridge::Instance().DataModel()->index(m_InsertIndex, 0);

    /* Remove the Media at the index */
    MBridge::Instance().Remove(index);
}

bool MediaImportCommand::Redo()
{
    /* Construct the media struct from the file path */
    MediaStruct mstruct = MediaStruct::FromFile(m_Path);

    /* Validate before adding */
    if (mstruct.Empty())
    {
        VOID_LOG_INFO("Invalid Media");
        return false;
    }

    if (!mstruct.ValidMedia())
    {
        VOID_LOG_INFO("Invalid Media: {0}", mstruct.FirstPath());
        return false;
    }

    /* Add the Media to the Model */
    return MBridge::Instance().AddMedia(mstruct);
}

/* }}} */

/* Media Remove Command {{{ */

MediaRemoveCommand::MediaRemoveCommand(const std::vector<QModelIndex>& indexes, QUndoCommand* parent)
    : VoidUndoCommand(parent)
    , m_Indexes(indexes)
{
    setText("Remove Media(s)");
}

void MediaRemoveCommand::undo()
{
    /* Add each Item back from its original source */
    for (std::pair<int, std::string> item : m_Paths)
    {
        /* Construct the media struct from the file path */
        MediaStruct mstruct = MediaStruct::FromFile(item.second);

        /* Validate before adding */
        if (mstruct.Empty())
        {
            VOID_LOG_INFO("Invalid Media");
            return;
        }

        if (!mstruct.ValidMedia())
        {
            VOID_LOG_INFO("Invalid Media: {0}", mstruct.FirstPath());
            return;
        }

        /* Add the Media to the Model */
        MBridge::Instance().InsertMedia(mstruct, item.first);
    }
}

bool MediaRemoveCommand::Redo()
{
    /* Don't have anything to do */
    if (m_Indexes.empty())
        return false;

    /**
     * Loop over in a a reverse way as forward iteration would shift the model indexes and
     * result in wrong indexes being deleted, or a worst case scenario result in crashes as
     * the second model index doesn't even exist after the first has been deleted
     */
    for (int i = m_Indexes.size() - 1; i >= 0; --i)
    {

        QModelIndex index = m_Indexes.at(i);

        /* Update the internal struct so that we know where each clip was and it's path */
        const SharedMediaClip clip = MBridge::Instance().DataModel()->Media(index);

        /**
         * Basepath from the clip, which will be used to create the clip back
         */
        m_Paths[index.row()] = clip->FirstFrameData().Path();

        /* Now Set this media for being removed */
        MBridge::Instance().Remove(index);
    }

    return true;
}

/* }}} */

VOID_NAMESPACE_CLOSE
