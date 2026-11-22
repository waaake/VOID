// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _FRAMERATE_MODEL_H
#define _FRAMERATE_MODEL_H

/* Qt */
#include <QAbstractListModel>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class VOID_API FramerateModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum class Roles { Float = Qt::UserRole + 1 };
    explicit FramerateModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    void Add(const QString& fr);
    void Add(double fr);
    void Remove(int row);
    double Framerate(int index) const;
};

VOID_NAMESPACE_CLOSE

#endif // _FRAMERATE_MODEL_H
