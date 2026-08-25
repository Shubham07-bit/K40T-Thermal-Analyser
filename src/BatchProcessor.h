#ifndef BATCHPROCESSOR_H
#define BATCHPROCESSOR_H

#include <QList>
#include <QObject>
#include <QStringList>

#include "ThermalDataModel.h"
#include "ThermalImageLoader.h"

/**
 * @brief Loads a list of thermal image files and collects statistics.
 */
class BatchProcessor : public QObject
{
    Q_OBJECT

public:
    explicit BatchProcessor(QObject *parent = nullptr);

    void registerLoader(ThermalImageLoader *loader);

    /**
     * @brief Load all supported files in the given paths (files or directories).
     * @return Summary string; details are emitted through signals.
     */
    QString loadPaths(const QStringList &paths);

    const QList<ThermalDataModel> &models() const { return m_models; }
    int count() const { return m_models.size(); }
    bool isEmpty() const { return m_models.isEmpty(); }

    void clear();

signals:
    void fileLoaded(int index, const ThermalDataModel &model);
    void fileFailed(const QString &path, const QString &reason);
    void finished(int successCount, int failCount);

private:
    QString tryLoadFile(const QString &filePath, ThermalDataModel &outData);

    QList<ThermalImageLoader *> m_loaders;
    QList<ThermalDataModel> m_models;
};

#endif // BATCHPROCESSOR_H
