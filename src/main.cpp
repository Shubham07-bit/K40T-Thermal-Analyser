#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QStringList>

#include "BatchProcessor.h"
#ifdef WITH_BLST_SDK
#include "BlstIrLoader.h"
#endif
#include "ColorMap.h"
#include "DjiRjpgLoader.h"
#include "MainWindow.h"
#include "OverlayExporter.h"
#include "PngTiffLoader.h"
#include "ThermalBox.h"

static int runHeadlessTest(const QString &path)
{
    BatchProcessor processor;
    processor.registerLoader(new PngTiffLoader());
#ifdef WITH_BLST_SDK
    processor.registerLoader(new BlstIrLoader());
#endif
    processor.registerLoader(new DjiRjpgLoader());

    QList<ThermalDataModel> models;
    QObject::connect(&processor, &BatchProcessor::fileLoaded,
                     [&](int, const ThermalDataModel &m) { models.append(m); });

    QString result = processor.loadPaths(QStringList() << path);
    qDebug().noquote() << result;

    if (models.isEmpty()) {
        qDebug() << "No thermal images could be loaded.";
        return 1;
    }

    for (const ThermalDataModel &m : std::as_const(models)) {
        qDebug().noquote() << m.fileName()
                           << "size=" << m.width() << "x" << m.height()
                           << "min=" << m.minTemperature()
                           << "max=" << m.maxTemperature();
    }
    return 0;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QObject::tr("K40T Thermal Analyser"));
    app.setOrganizationName(QObject::tr("Indrones"));

    QCommandLineParser parser;
    parser.setApplicationDescription(QObject::tr("Thermal image visualization and analysis tool."));
    parser.addHelpOption();

    QCommandLineOption testLoadOption(QStringList() << "t" << "test-load",
        QObject::tr("Load image(s) without GUI and print statistics."),
        QObject::tr("path"));
    parser.addOption(testLoadOption);

    QCommandLineOption exportBatchTestOption(QStringList() << "export-batch-test",
        QObject::tr("Load image(s) and export each with min/max overlays to the given directory (no GUI)."),
        QObject::tr("outdir"));
    parser.addOption(exportBatchTestOption);

    parser.process(app);

    if (parser.isSet(testLoadOption)) {
        return runHeadlessTest(parser.value(testLoadOption));
    }

    if (parser.isSet(exportBatchTestOption)) {
        const QStringList positional = parser.positionalArguments();
        if (positional.isEmpty()) {
            qDebug() << "Usage: --export-batch-test <outdir> <image_or_directory>";
            return 1;
        }

        BatchProcessor processor;
        processor.registerLoader(new PngTiffLoader());
#ifdef WITH_BLST_SDK
        processor.registerLoader(new BlstIrLoader());
#endif
        processor.registerLoader(new DjiRjpgLoader());

        QList<ThermalDataModel> models;
        QObject::connect(&processor, &BatchProcessor::fileLoaded,
                         [&](int, const ThermalDataModel &m) { models.append(m); });

        QString loadResult = processor.loadPaths(positional);
        qDebug().noquote() << loadResult;

        if (models.isEmpty()) {
            qDebug() << "No images to export.";
            return 1;
        }

        QDir outDir(parser.value(exportBatchTestOption));
        if (!outDir.exists() && !outDir.mkpath(".")) {
            qDebug() << "Failed to create output directory.";
            return 1;
        }

        int ok = 0;
        for (const ThermalDataModel &m : std::as_const(models)) {
            QString outPath = outDir.filePath(QFileInfo(m.fileName()).completeBaseName() + "_overlay.png");
            OverlayExporter::Result result = OverlayExporter::render(
                m, ColorMap::IronBow, QList<QPoint>(), QList<ThermalBox>(), OverlayExporter::MinMaxMarkers);
            QString err = OverlayExporter::save(result, outPath);
            if (err.isEmpty()) {
                qDebug().noquote() << "Exported:" << outPath;
                ++ok;
            } else {
                qDebug().noquote() << "Export failed:" << err;
            }
        }
        qDebug().noquote() << QString("Exported %1/%2 images.").arg(ok).arg(models.size());
        return ok == models.size() ? 0 : 1;
    }

    MainWindow window;

    const QStringList args = app.arguments();
    QStringList positional;
    for (int i = 1; i < args.size(); ++i) {
        if (!args.at(i).startsWith("-"))
            positional.append(args.at(i));
    }
    if (!positional.isEmpty()) {
        window.loadFiles(positional);
    }

    window.show();
    return app.exec();
}
