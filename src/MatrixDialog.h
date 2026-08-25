#ifndef MATRIXDIALOG_H
#define MATRIXDIALOG_H

#include <QAbstractTableModel>
#include <QDialog>

#include "ThermalDataModel.h"

class QTableView;

/**
 * @brief Read-only table model that exposes the thermal matrix.
 */
class TemperatureMatrixModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit TemperatureMatrixModel(const ThermalDataModel &data, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    const ThermalDataModel &m_data;
};

/**
 * @brief Dialog that displays the full temperature matrix in a table.
 */
class MatrixDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MatrixDialog(const ThermalDataModel &data, QWidget *parent = nullptr);

private:
    QTableView *m_tableView = nullptr;
};

#endif // MATRIXDIALOG_H
