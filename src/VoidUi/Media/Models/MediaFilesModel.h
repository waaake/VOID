// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_MEDIA_FILES_MODEL_H
#define _VOID_MEDIA_FILES_MODEL_H

/* STD */
#include <set>

/* Qt */
#include <QSortFilterProxyModel>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class MediaFilesProxyModel : public QSortFilterProxyModel
{
public:
    MediaFilesProxyModel(QObject* parent = nullptr);

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
    mutable std::set<QString> m_Media;
    bool m_SequenceView;

private: /* Methods */
    QString TemplatedName(const QString& name) const;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_FILES_MODEL_H
