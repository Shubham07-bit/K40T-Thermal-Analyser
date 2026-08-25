#include "MatrixDialog.h"

#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QHeaderView>
#include <QTableView>
#include <QVBoxLayout>

TemperatureMatrixModel::TemperatureMatrixModel(const ThermalDataModel &data, QObject *parent)
    : QAbstractTableModel(parent)
    , m_data(data)
{
}

int TemperatureMatrixModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_data.isValid() ? m_data.height() : 0;
}

int TemperatureMatrixModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_data.isValid() ? m_data.width() : 0;
}

QVariant TemperatureMatrixModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !m_data.isValid())
        return QVariant();

    const int x = index.column();
    const int y = index.row();

    if (x < 0 || x >= m_data.width() || y < 0 || y >= m_data.height())
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        const float t = m_data.temperatureAt(x, y);
        return QString::number(t, 'f', 2);
    }

    if (role == Qt::TextAlignmentRole)
        return QVariant(Qt::AlignRight | Qt::AlignVCenter);

    return QVariant();
}

QVariant TemperatureMatrixModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        return QString("X%1").arg(section);
    else
        return QString("Y%1").arg(section);
}

MatrixDialog::MatrixDialog(const ThermalDataModel &data, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Temperature Matrix - %1").arg(data.fileName()));
    setMinimumSize(700, 500);

    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *infoLabel = new QLabel(tr("Showing %1 x %2 temperature values in Celsius.").arg(data.width()).arg(data.height()));
    layout->addWidget(infoLabel);

    m_tableView = new QTableView(this);
    m_tableView->setModel(new TemperatureMatrixModel(data, m_tableView));
    m_tableView->setAlternatingRowColors(true);
    m_tableView->horizontalHeader()->setDefaultSectionSize(64);
    m_tableView->verticalHeader()->setDefaultSectionSize(22);
    layout->addWidget(m_tableView);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}
