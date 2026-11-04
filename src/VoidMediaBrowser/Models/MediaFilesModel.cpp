// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QFileSystemModel>

/* Internal */
#include "MediaFilesModel.h"
#include "VoidCore/Logging.h"
#include "VoidCore/Media/Filesystem.h"

VOID_NAMESPACE_OPEN

/**
 * Regex to compare with entity names as image.1234.exr and it returns the match as
 * image
 * 1234
 * exr
 */
static QRegularExpression s_SequenceRegex(R"(^(.*)\.(\d+)\.(\w+)$)");
/**
 * Regex to compare with entity names as image.1234.exr and it returns the match as
 * image.
 * 1234
 * .exr
 *
 * Useful when re-constructing the templated name from the original name as image.####.exr
 */
static QRegularExpression s_NamingRegex(R"(^(.*?)(\d+)(\.[^.]+)$)");

MediaFilesProxyModel::MediaFilesProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
    , m_SequenceView(false)
{
}

void MediaFilesProxyModel::EnableSequences(bool enable)
{
    m_SequenceView = enable;
    invalidateFilter();
}

QVariant MediaFilesProxyModel::data(const QModelIndex& index, int role) const
{
    if (m_SequenceView)
    {
        if (role == MediaFilesRoles::DisplayNameRole)
        {
            const QModelIndex& idx = this->index(index.row(), 0, index.parent());
            const QVariant& data = QSortFilterProxyModel::data(idx, Qt::DisplayRole);
            const QString& path = QSortFilterProxyModel::data(idx, QFileSystemModel::FilePathRole).toString();
            return m_Sequences.find(path) != m_Sequences.end() ? TemplatedName(data.toString()) : data;
        }

        const QVariant& data = QSortFilterProxyModel::data(index, role);

        if (role == Qt::DisplayRole)
        {
            if (index.column() == 0)
            {
                const QString& name = data.toString();
                const QString& path = QSortFilterProxyModel::data(index, QFileSystemModel::FilePathRole).toString();

                const auto& it = m_Sequences.find(path);
                if (it != m_Sequences.end())
                    return TemplatedName(name, it->second.Framerange());
            }
            else if (index.column() == 1)
            {
                const QString& path = QSortFilterProxyModel::data(
                                                    this->index(index.row(), 0, index.parent()), QFileSystemModel::FilePathRole
                                                ).toString();
                if (m_Sequences.find(path) != m_Sequences.end())
                    return QVariant();
            }
        }

        return data;
    }

    return QSortFilterProxyModel::data(index, role);
}

bool MediaFilesProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    if (m_SequenceView)
    {
        QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
        const QString& name = sourceModel()->data(index, QFileSystemModel::FileNameRole).toString();
        const QString& path = sourceModel()->data(index, QFileSystemModel::FilePathRole).toString();
        const QString& directory = sourceModel()->data(index.parent(), QFileSystemModel::FilePathRole).toString();

        /**
         * Already visited and accepted media entry,
         * this is likely going to be the first entry (whatever comes as the first entry based on the sorting)
         * not that we really care what is it, as long as it's part of a sequence
         *
         * This method only cares about ensuring only one of the entity from an entire sequence
         * for e.g. image.1001.png image.1002.png image.1003.png ... image.1010.png is a sequence and this method
         * ensures only one of the image entry, could be any is the only one that is returned
         * if for image.1001.png it returns true, the display of how the entity appears in the FileDialog
         * is then handled by the MediaFilesProxyModel::data method and any formatting to the hash or printf
         * representation is handled there
         */
        if (m_Sequences.find(path) != m_Sequences.end())
            return true;

        QRegularExpressionMatch match = s_SequenceRegex.match(name);
        if (match.hasMatch())
        {
            QString key = directory + match.captured(1) + match.captured(3);
            if (m_Visited.find(key) == m_Visited.end())
            {
                m_Visited.insert(key);
                m_Sequences[path] = std::move(MediaStruct::FromFile(path.toStdString()));
                return true;
            }
            else
            {
                return false;
            }
        }

        return true;
    }

    return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
}

QString MediaFilesProxyModel::TemplatedName(const QString& name) const
{
    QRegularExpressionMatch match = s_NamingRegex.match(name);
    if (match.hasMatch())
    {
        const QString hashes(match.captured(2).size(), '#');
        return match.captured(1) + hashes + match.captured(3);
    }

    return name;
}

QString MediaFilesProxyModel::TemplatedName(const QString& name, const MFrameRange& range) const
{
    QRegularExpressionMatch match = s_NamingRegex.match(name);
    if (match.hasMatch())
    {
        const QString hashes(match.captured(2).size(), '#');
        return QString("%1%2%3 %4-%5")
                    .arg(match.captured(1))
                    .arg(hashes)
                    .arg(match.captured(3))
                    .arg(range.startframe)
                    .arg(range.endframe);
    }

    return name;
}

void MediaFilesProxyModel::ResetSequences()
{
    m_Sequences.clear();
    m_Visited.clear();
}

VOID_NAMESPACE_CLOSE
