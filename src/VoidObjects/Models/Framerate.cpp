// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <algorithm>
#include <vector>

/* Internal */
#include "Framerate.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

namespace {

static inline std::vector<double> s_Framerates = {
    8.0,
    10.0,
    12.0,
    15.0,
    23.976,
    24.0,
    25.0,
    29.97,
    30.0,
    48.0,
    50.0,
    59.94,
    60.0,
};

} // namespace

FramerateModel::FramerateModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int FramerateModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(s_Framerates.size());
}

QVariant FramerateModel::data(const QModelIndex& index, int role) const
{
    if (index.isValid() && index.row() < static_cast<int>(s_Framerates.size()))
    {
        if (role == Qt::DisplayRole || role == Qt::EditRole)
            return QString::number(s_Framerates[index.row()], 'g', 6);
        if (role == static_cast<int>(Roles::Float))
            return s_Framerates[index.row()];
    }

    return QVariant();
}

void FramerateModel::Add(const QString& fr)
{
    Add(fr.toDouble());
}

void FramerateModel::Add(double fr)
{
    bool exists = false;
    auto _comp = [&exists] (const double& _v, const double& _fr) -> bool
    {
        // Only if the framerate matches, we'll need this to not add same one again
        if (std::abs(_v - _fr) < 0.001)
            exists = true;
        return _v < _fr;
    };

    const auto it = std::lower_bound(s_Framerates.begin(), s_Framerates.end(), fr, _comp);

    if (exists) return;
    int d = std::distance(s_Framerates.begin(), it);

    beginInsertRows(QModelIndex(), d, d);
    s_Framerates.insert(it, fr);
    VOID_LOG_INFO("Added {} to global framerates", fr);
    endInsertRows();
}

void FramerateModel::Remove(int row)
{
    beginRemoveRows(QModelIndex(), row, row);
    s_Framerates.erase(s_Framerates.begin() + row);
    endRemoveRows();
}

double FramerateModel::Framerate(int index) const
{
    if (index < static_cast<int>(s_Framerates.size()))
        return s_Framerates[index];
    return 0.0;
}

VOID_NAMESPACE_CLOSE
