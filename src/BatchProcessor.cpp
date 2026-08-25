#include "BatchProcessor.h"

#include <QDir>
#include <QFileInfo>

BatchProcessor::BatchProcessor(QObject *parent)
    : QObject(parent)
{
}

void BatchProcessor::registerLoader(ThermalImageLoader *loader)
{
    if (loader && !m_loaders.contains(loader))
        m_loaders.append(loader);
}

QString BatchProcessor::loadPaths(const QStringList &paths)
{
    int success = 0;
    int failed = 0;
    QString lastError;

    QStringList files;
    for (const QString &path : paths) {
        QFileInfo fi(path);
        if (fi.isDir()) {
            QDir dir(path);
            for (const QFileInfo &child : dir.entryInfoList(QDir::Files))
                files.append(child.absoluteFilePath());
        } else if (fi.isFile()) {
            files.append(path);
        }
    }

    for (const QString &file : std::as_const(files)) {
        ThermalDataModel model;
        QString err = tryLoadFile(file, model);
        if (err.isEmpty()) {
            m_models.append(model);
            emit fileLoaded(m_models.size() - 1, model);
            ++success;
        } else {
            lastError = err;
            emit fileFailed(file, err);
            ++failed;
        }
    }

    emit finished(success, failed);

    if (success == 0 && !files.isEmpty()) {
        return QObject::tr("No files could be loaded. Last error: %1").arg(lastError);
    }
    return QObject::tr("Loaded %1 file(s), %2 failed.").arg(success).arg(failed);
}

void BatchProcessor::clear()
{
    m_models.clear();
}

QString BatchProcessor::tryLoadFile(const QString &filePath, ThermalDataModel &outData)
{
    QFileInfo fi(filePath);
    QString ext = fi.suffix().toLower();

    for (ThermalImageLoader *loader : std::as_const(m_loaders)) {
        if (loader->supportedExtensions().contains(ext)) {
            return loader->load(filePath, outData);
        }
    }

    return QObject::tr("No loader registered for extension '.%1'").arg(ext);
}
