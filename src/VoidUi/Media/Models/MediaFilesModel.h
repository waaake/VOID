// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_MEDIA_FILES_MODEL_H
#define _VOID_MEDIA_FILES_MODEL_H

/* STD */
#include <set>
#include <unordered_map>

/* Qt */
#include <QString>
#include <QSortFilterProxyModel>

/* Internal */
#include "Definition.h"
#include "VoidCore/Media/Filesystem.h"

namespace std {

    template <>
    struct hash<QString>
    {
        inline std::size_t operator()(const QString& key) const noexcept { return qHash(key); }
    };

} // namespace std

VOID_NAMESPACE_OPEN

enum MediaFilesRoles
{
    DisplayNameRole = Qt::UserRole + 1001
};

class MediaFilesProxyModel : public QSortFilterProxyModel
{
public:
    explicit MediaFilesProxyModel(QObject* parent = nullptr);

    /**
     * @brief Enables Sequence Display for Image/File sequences of the format name.####.ext
     * 
     * @param enable true to enable Sequence Display.
     * @param enable false to disable Sequence Display and show files normally.
     */
    void EnableSequences(bool enable);

    /**
     * @brief Clears any stored Sequences from the underlying structs.
     * 
     */
    void ResetSequences();

protected:
    QVariant data(const QModelIndex& index, int role) const override;
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

private: /* Members */
    mutable std::set<QString> m_Visited;
    mutable std::unordered_map<QString, MediaStruct> m_Sequences;
    bool m_SequenceView;

private: /* Methods */
    QString TemplatedName(const QString& name) const;
    QString TemplatedName(const QString& name, const MFrameRange& range) const;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_FILES_MODEL_H
