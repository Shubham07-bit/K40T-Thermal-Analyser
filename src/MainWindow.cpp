#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QAction>
#include <QActionGroup>
#include <QCheckBox>
#include <QDateTime>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDir>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QListWidget>
#include <QMenu>
#include <QMessageBox>
#include <QMimeData>
#include <QPushButton>
#include <QModelIndex>
#include <QSet>
#include <QStandardItemModel>
#include <QTextStream>
#include <QUrl>
#include <QVBoxLayout>

#include <functional>
#include <limits>

static const QStringList &supportedImageExtensions()
{
    static const QStringList extensions = QStringList()
        << "jpg" << "jpeg" << "png" << "tif" << "tiff";
    return extensions;
}

#include "BatchProcessor.h"
#ifdef WITH_BLST_SDK
#include "BlstIrLoader.h"
#endif
#include "ColorMap.h"
#include "DjiRjpgLoader.h"
#include "ImageView.h"
#include "MatrixDialog.h"
#include "OverlayExporter.h"
#include "PngTiffLoader.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_batchProcessor(this)
{
    ui->setupUi(this);

    // Register loaders. Order matters: BlstIrLoader must come before DjiRjpgLoader
    // because both claim .jpg/.jpeg. We want real BLST IR files decoded by the SDK.
    auto *pngLoader = new PngTiffLoader();
    auto *djiLoader = new DjiRjpgLoader();
    m_ownedLoaders << pngLoader << djiLoader;
    m_batchProcessor.registerLoader(pngLoader);

#ifdef WITH_BLST_SDK
    auto *blstIrLoader = new BlstIrLoader();
    m_ownedLoaders << blstIrLoader;
    m_batchProcessor.registerLoader(blstIrLoader);
#endif

    m_batchProcessor.registerLoader(djiLoader);

    // Image viewer lives in the central frame
    m_imageView = new ImageView(this);
    ui->imageFrameLayout->addWidget(m_imageView);

    setAcceptDrops(true);

    setupUi();
    connectSignals();
}

MainWindow::~MainWindow()
{
    delete ui;
    for (ThermalImageLoader *loader : std::as_const(m_ownedLoaders))
        delete loader;
    m_ownedLoaders.clear();
}

void MainWindow::setupUi()
{
    ui->paletteCombo->addItems(ColorMap::paletteNames());
    ui->paletteCombo->setCurrentIndex(static_cast<int>(ColorMap::IronBow));

    // Default to original-image mode.
    ui->rawCheckBox->setChecked(true);
    m_rawMode = true;

    QStandardItemModel *metaModel = new QStandardItemModel(this);
    metaModel->setHorizontalHeaderLabels(QStringList() << tr("Key") << tr("Value"));
    ui->metadataTree->setModel(metaModel);

    ui->fileList->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->fileList->setIconSize(QSize(96, 72));
    ui->fileList->setSpacing(4);
    ui->fileList->setContextMenuPolicy(Qt::CustomContextMenu);

    ui->pointsList->setSelectionMode(QAbstractItemView::SingleSelection);

    // Hide measurement panels until there is something to show.
    ui->pointsGroup->setVisible(false);
    ui->boxesGroup->setVisible(false);

    // Default spin boxes disabled until data loaded
    ui->minSpin->setEnabled(false);
    ui->maxSpin->setEnabled(false);
    ui->autoRangeButton->setEnabled(false);

    // Limit zoom range.
    m_imageView->setZoomRange(0.05, 20.0);

    // Delete-key / context-menu action to remove the selected image.
    QAction *removeImageAction = new QAction(tr("Remove Image"), this);
    removeImageAction->setShortcut(QKeySequence::Delete);
    removeImageAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    ui->fileList->addAction(removeImageAction);
    connect(removeImageAction, &QAction::triggered, this, &MainWindow::removeCurrentImage);

    // Make view-mode actions mutually exclusive.
    QActionGroup *viewGroup = new QActionGroup(this);
    viewGroup->addAction(ui->actionViewList);
    viewGroup->addAction(ui->actionViewGrid);
    viewGroup->addAction(ui->actionViewThumbnails);
    viewGroup->setExclusive(true);
    ui->actionViewList->setChecked(true);

    addLogMessage(tr("K40T Thermal Analyser started."));
    addLogMessage(tr("Supported formats: K40T IR JPEG, 16-bit grayscale PNG/TIFF."));
    addLogMessage(tr("Left click on the image to place measurement points."));
}

void MainWindow::connectSignals()
{
    /*
     * Qt's connectSlotsByName() (called by setupUi()) automatically connects
     * slots named on_<objectName>_<signalName> to the matching UI objects.
     * Therefore we must NOT explicitly connect action/widget slots here;
     * doing so would fire each slot twice.
     *
     * Only explicitly connect objects created in code (m_imageView,
     * m_batchProcessor) whose slots do not follow the auto-connect naming.
     */

    connect(m_imageView, &ImageView::pixelHovered, this, &MainWindow::on_pixelHovered);
    connect(m_imageView, &ImageView::pixelClicked, this, &MainWindow::on_pixelClicked);
    connect(m_imageView, &ImageView::minMaxChanged, this, &MainWindow::on_minMaxChanged);
    connect(m_imageView, &ImageView::userPointsChanged, this, &MainWindow::on_userPointsChanged);
    connect(m_imageView, &ImageView::boxDrawn, this, &MainWindow::on_boxDrawn);

    connect(&m_batchProcessor, &BatchProcessor::fileLoaded, this, &MainWindow::on_batchFileLoaded);
    connect(&m_batchProcessor, &BatchProcessor::fileFailed, this, &MainWindow::on_batchFileFailed);
    connect(&m_batchProcessor, &BatchProcessor::finished, this, &MainWindow::on_batchFinished);
}

void MainWindow::loadFile(const QString &filePath)
{
    loadFiles(QStringList() << filePath);
}

QStringList MainWindow::collectImageFiles(const QStringList &paths) const
{
    QStringList result;
    QSet<QString> seen;

    std::function<void(const QString&)> scan = [&](const QString &path) {
        QFileInfo fi(path);
        if (!fi.exists())
            return;

        if (fi.isFile()) {
            const QString ext = fi.suffix().toLower();
            if (supportedImageExtensions().contains(ext)) {
                const QString abs = fi.absoluteFilePath();
                if (!seen.contains(abs)) {
                    seen.insert(abs);
                    result << abs;
                }
            }
        } else if (fi.isDir()) {
            QDir dir(path);
            for (const QFileInfo &child : dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot)) {
                if (child.isDir()) {
                    scan(child.absoluteFilePath());
                } else {
                    scan(child.absoluteFilePath());
                }
            }
        }
    };

    for (const QString &path : paths)
        scan(path);

    return result;
}

void MainWindow::loadFiles(const QStringList &filePaths)
{
    m_models.clear();
    m_imagePoints.clear();
    m_currentModelIndex = -1;
    invalidateThumbnailCache();
    ui->fileList->clear();
    m_batchProcessor.clear();
    QString result = m_batchProcessor.loadPaths(filePaths);
    addLogMessage(result);
}

void MainWindow::on_actionOpen_triggered()
{
    QDialog dialog(this);
    dialog.setWindowTitle(tr("Open"));
    dialog.setMinimumWidth(300);
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    layout->addWidget(new QLabel(tr("Open thermal images or a folder containing images:"), &dialog));

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *filesButton = new QPushButton(tr("Open Image(s)..."), &dialog);
    QPushButton *folderButton = new QPushButton(tr("Open Folder..."), &dialog);
    QPushButton *cancelButton = new QPushButton(tr("Cancel"), &dialog);
    btnLayout->addWidget(filesButton);
    btnLayout->addWidget(folderButton);
    btnLayout->addStretch();
    btnLayout->addWidget(cancelButton);
    layout->addLayout(btnLayout);

    QStringList paths;
    connect(filesButton, &QPushButton::clicked, &dialog, [&]() {
        const QString filter = tr("Thermal Images (*.png *.tif *.tiff *.jpg *.jpeg);;All Files (*)");
        const QStringList files = QFileDialog::getOpenFileNames(
            &dialog, tr("Open Images"), QString(), filter);
        if (!files.isEmpty()) {
            paths = files;
            dialog.accept();
        }
    });
    connect(folderButton, &QPushButton::clicked, &dialog, [&]() {
        const QString dir = QFileDialog::getExistingDirectory(
            &dialog, tr("Open Folder with Images"));
        if (!dir.isEmpty()) {
            paths = QStringList() << dir;
            dialog.accept();
        }
    });
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted || paths.isEmpty())
        return;

    loadFiles(collectImageFiles(paths));
}

void MainWindow::on_actionGenerateTestImage_triggered()
{
    generateSyntheticImage();
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("About K40T Thermal Analyser"),
        tr("<h2>K40T Thermal Analyser</h2>"
           "<p>A Qt 6 based thermal image visualization tool for the K40T camera.</p>"
           "<p>Features: multi-image loading, color palettes, grayscale view, "
           "min/max overlay, clickable measurement points, export with overlays, "
           "pixel temperature readout, metadata panel.</p>"
           "<p>Copyright &copy; Indrones Solution Pvt. Ltd.<br>"
           "<a href=\"https://indrones.com/\">https://indrones.com/</a></p>"));
}

void MainWindow::on_actionZoomIn_triggered()
{
    m_imageView->scale(1.2, 1.2);
}

void MainWindow::on_actionZoomOut_triggered()
{
    m_imageView->scale(1.0 / 1.2, 1.0 / 1.2);
}

void MainWindow::on_actionResetZoom_triggered()
{
    m_imageView->resetTransform();
}

void MainWindow::on_actionToggleMinMax_triggered()
{
    m_imageView->setShowMinMax(ui->actionToggleMinMax->isChecked());
}

void MainWindow::on_actionToggleCrosshair_triggered()
{
    m_imageView->setShowCrosshair(ui->actionToggleCrosshair->isChecked());
}

void MainWindow::on_actionRemoveLastBox_triggered()
{
    on_removeLastBoxButton_clicked();
}

void MainWindow::on_actionClearBoxes_triggered()
{
    on_clearBoxesButton_clicked();
}

void MainWindow::on_actionViewList_triggered()
{
    setFileListViewMode(QListWidget::ListMode, false);
}

void MainWindow::on_actionViewGrid_triggered()
{
    setFileListViewMode(QListWidget::IconMode, false);
}

void MainWindow::on_actionViewThumbnails_triggered()
{
    setFileListViewMode(QListWidget::IconMode, true);
}

void MainWindow::on_actionExport_triggered()
{
    if (m_models.isEmpty()) {
        QMessageBox::warning(this, tr("Export"), tr("No images loaded."));
        return;
    }

    QList<int> selectedIndices = selectImagesForExport();
    if (selectedIndices.isEmpty())
        return;

    if (selectedIndices.size() == 1) {
        // Single image: use a save-file dialog.
        const int idx = selectedIndices.first();
        const QString outputPath = QFileDialog::getSaveFileName(
            this, tr("Export Image with Overlays"),
            suggestExportFileName(m_models[idx]),
            tr("Images (*.png *.jpg *.jpeg *.bmp *.tif);;All Files (*)"));
        if (outputPath.isEmpty())
            return;

        QString err = exportModel(m_models[idx], m_imagePoints[idx], m_imageBoxes[idx], outputPath,
                                  OverlayExporter::AllOverlays);
        if (err.isEmpty()) {
            const QString csvPath = QFileInfo(outputPath).path() + "/" + QFileInfo(outputPath).completeBaseName() + ".csv";
            QString csvErr = writeCsv(m_models[idx], m_imagePoints[idx], m_imageBoxes[idx], csvPath);
            if (!csvErr.isEmpty()) {
                addLogMessage(tr("CSV export failed: %1").arg(csvErr));
            }
            addLogMessage(tr("Exported image to %1").arg(outputPath));
            ui->statusLabel->setText(tr("Exported to %1").arg(outputPath));
        } else {
            QMessageBox::critical(this, tr("Export Failed"), err);
            addLogMessage(tr("Export failed: %1").arg(err));
        }
    } else {
        // Multiple images: choose an output directory.
        const QString dir = QFileDialog::getExistingDirectory(
            this, tr("Select Output Directory for Export"));
        if (dir.isEmpty())
            return;

        int success = 0;
        int failed = 0;
        QString lastError;

        for (int idx : selectedIndices) {
            const ThermalDataModel &model = m_models[idx];
            const QString baseName = QFileInfo(model.fileName()).completeBaseName();
            const QString outputPath = dir + "/" + baseName + "_overlay.png";
            QString err = exportModel(model, m_imagePoints[idx], m_imageBoxes[idx], outputPath,
                                      OverlayExporter::AllOverlays);
            if (err.isEmpty()) {
                const QString csvPath = dir + "/" + baseName + "_overlay.csv";
                writeCsv(model, m_imagePoints[idx], m_imageBoxes[idx], csvPath);
                ++success;
            } else {
                ++failed;
                lastError = err;
            }
        }

        addLogMessage(tr("Export: %1 succeeded, %2 failed.").arg(success).arg(failed));
        ui->statusLabel->setText(tr("Export: %1 succeeded, %2 failed.").arg(success).arg(failed));

        if (failed > 0) {
            QMessageBox::critical(this, tr("Export"),
                tr("%1 image(s) exported successfully. %2 failed.\nLast error: %3")
                    .arg(success).arg(failed).arg(lastError));
        }
    }
}

void MainWindow::on_actionRemoveLastPoint_triggered()
{
    m_imageView->removeLastUserPoint();
}

void MainWindow::on_actionClearPoints_triggered()
{
    m_imageView->clearUserPoints();
}

void MainWindow::on_paletteCombo_currentIndexChanged(int index)
{
    m_currentPalette = static_cast<ColorMap::Palette>(index);
    refreshImage();
    refreshAllThumbnails();
}

void MainWindow::on_rawCheckBox_stateChanged(int state)
{
    m_rawMode = (state == Qt::Checked);
    refreshImage();
    refreshAllThumbnails();
}

void MainWindow::on_minSpin_valueChanged(double value)
{
    m_manualMin = static_cast<float>(value);
    refreshImage();
}

void MainWindow::on_maxSpin_valueChanged(double value)
{
    m_manualMax = static_cast<float>(value);
    refreshImage();
}

void MainWindow::on_autoRangeButton_clicked()
{
    m_manualMin = std::nullopt;
    m_manualMax = std::nullopt;
    refreshImage();
}

void MainWindow::on_clearPointsButton_clicked()
{
    m_imageView->clearUserPoints();
}

void MainWindow::on_removeLastPointButton_clicked()
{
    m_imageView->removeLastUserPoint();
}

void MainWindow::on_clearBoxesButton_clicked()
{
    int idx = ui->fileList->currentRow();
    if (idx >= 0 && idx < m_imageBoxes.size()) {
        m_imageBoxes[idx].clear();
        applyBoxesToView(idx);
        updateBoxesList();
    }
}

void MainWindow::on_removeLastBoxButton_clicked()
{
    int idx = ui->fileList->currentRow();
    if (idx >= 0 && idx < m_imageBoxes.size() && !m_imageBoxes[idx].isEmpty()) {
        m_imageBoxes[idx].removeLast();
        applyBoxesToView(idx);
        updateBoxesList();
    }
}

void MainWindow::on_viewMatrixButton_clicked()
{
    int idx = ui->fileList->currentRow();
    if (idx < 0 || idx >= m_models.size())
        return;

    MatrixDialog dialog(m_models[idx], this);
    dialog.exec();
}

void MainWindow::on_fileList_customContextMenuRequested(const QPoint &pos)
{
    int idx = ui->fileList->row(ui->fileList->itemAt(pos));
    if (idx < 0 || idx >= m_models.size())
        return;

    QMenu menu(this);
    QAction *removeAction = menu.addAction(tr("Remove Image"));
    if (menu.exec(ui->fileList->mapToGlobal(pos)) == removeAction) {
        ui->fileList->setCurrentRow(idx);
        removeCurrentImage();
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QList<QUrl> urls = event->mimeData()->urls();
    QStringList paths;
    for (const QUrl &url : urls) {
        if (url.isLocalFile())
            paths << url.toLocalFile();
    }

    if (!paths.isEmpty()) {
        loadFiles(collectImageFiles(paths));
    }
    event->acceptProposedAction();
}

void MainWindow::on_fileList_currentRowChanged(int currentRow)
{
    if (currentRow >= 0 && currentRow < m_models.size()) {
        displayModel(currentRow);
    }
}

void MainWindow::on_pixelHovered(int x, int y, float temperature)
{
    ui->statusLabel->setText(tr("Pixel (%1, %2): %3 C  |  %4 F")
                                 .arg(x)
                                 .arg(y)
                                 .arg(temperature, 0, 'f', 2)
                                 .arg(ThermalDataModel::celsiusToFahrenheit(temperature), 0, 'f', 2));
}

void MainWindow::on_pixelClicked(int x, int y, float temperature)
{
    Q_UNUSED(x)
    Q_UNUSED(y)
    Q_UNUSED(temperature)
    // Point is already added by ImageView; we just refresh the list here.
    updatePointsList();
}

void MainWindow::on_minMaxChanged(float min, float max)
{
    addLogMessage(tr("Image range: %1 C to %2 C").arg(min, 0, 'f', 2).arg(max, 0, 'f', 2));
}

void MainWindow::on_userPointsChanged(const QList<QPoint> &points)
{
    int idx = ui->fileList->currentRow();
    if (idx >= 0 && idx < m_imagePoints.size()) {
        m_imagePoints[idx] = points;
    }
    updatePointsList();
}

void MainWindow::on_batchFileLoaded(int index, const ThermalDataModel &model)
{
    Q_UNUSED(index)
    m_models.append(model);
    m_imagePoints.append(QList<QPoint>());
    m_imageBoxes.append(QList<ThermalBox>());

    const int newIndex = m_models.size() - 1;
    QListWidgetItem *item = new QListWidgetItem(model.fileName());
    item->setIcon(thumbnailIconForIndex(newIndex));
    ui->fileList->addItem(item);

    if (m_models.size() == 1) {
        // setCurrentRow already triggers currentRowChanged -> displayModel().
        ui->fileList->setCurrentRow(0);
    }
}

void MainWindow::on_batchFileFailed(const QString &path, const QString &reason)
{
    addLogMessage(tr("Failed to load %1: %2").arg(path, reason));
}

void MainWindow::on_batchFinished(int successCount, int failCount)
{
    Q_UNUSED(successCount)
    Q_UNUSED(failCount)
    ui->statusLabel->setText(tr("Finished. %1 loaded, %2 failed.").arg(successCount).arg(failCount));
}

void MainWindow::refreshImage()
{
    int idx = ui->fileList->currentRow();
    if (idx < 0 || idx >= m_models.size())
        return;

    const ThermalDataModel &model = m_models[idx];
    if (!model.isValid())
        return;

    QImage rendered;
    if (m_rawMode && !model.previewImage().isNull()) {
        // Grayscale mode shows the original embedded image unchanged.
        rendered = model.previewImage();
    } else {
        ColorMap::Palette palette = m_rawMode ? ColorMap::RawGrayscale : m_currentPalette;
        rendered = ColorMap::render(model, palette, m_manualMin, m_manualMax);
    }

    m_imageView->setRenderedImage(rendered);
    m_imageView->setThermalData(model);

    // Update thumbnail for the current item.
    if (QListWidgetItem *item = ui->fileList->item(idx)) {
        item->setIcon(thumbnailIconForIndex(idx));
    }

    // Update spin boxes to reflect current range
    ui->minSpin->blockSignals(true);
    ui->maxSpin->blockSignals(true);
    ui->minSpin->setValue(m_manualMin.value_or(model.minTemperature()));
    ui->maxSpin->setValue(m_manualMax.value_or(model.maxTemperature()));
    ui->minSpin->blockSignals(false);
    ui->maxSpin->blockSignals(false);
}

void MainWindow::displayModel(int index)
{
    if (index < 0 || index >= m_models.size())
        return;

    if (index == m_currentModelIndex)
        return;

    m_currentModelIndex = index;

    const ThermalDataModel &model = m_models[index];
    updateMetadataPanel(model);

    ui->minSpin->setEnabled(true);
    ui->maxSpin->setEnabled(true);
    ui->autoRangeButton->setEnabled(true);

    refreshImage();
    applyPointsToView(index);
    applyBoxesToView(index);
    updateBoxesList();
}

void MainWindow::updateMetadataPanel(const ThermalDataModel &data)
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel *>(ui->metadataTree->model());
    if (!model)
        return;

    model->removeRows(0, model->rowCount());

    const QMap<QString, QVariant> meta = data.metadata();
    for (auto it = meta.cbegin(); it != meta.cend(); ++it) {
        QList<QStandardItem *> row;
        row << new QStandardItem(it.key());
        row << new QStandardItem(it.value().toString());
        model->appendRow(row);
    }

    ui->metadataTree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void MainWindow::updatePointsList()
{
    ui->pointsList->clear();
    int idx = ui->fileList->currentRow();
    const bool haveModel = (idx >= 0 && idx < m_models.size() && m_models[idx].isValid());
    if (!haveModel) {
        ui->pointsGroup->setVisible(false);
        return;
    }

    const QList<QPoint> &points = m_imagePoints[idx];
    for (int i = 0; i < points.size(); ++i) {
        const QPoint p = points[i];
        float t = m_models[idx].temperatureAt(p.x(), p.y());
        ui->pointsList->addItem(tr("P%1: (%2, %3) %4 C")
                                    .arg(i + 1)
                                    .arg(p.x())
                                    .arg(p.y())
                                    .arg(t, 0, 'f', 2));
    }
    ui->pointsGroup->setVisible(!points.isEmpty());
}

void MainWindow::addLogMessage(const QString &msg)
{
    const QString line = QString("[%1] %2")
                             .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                             .arg(msg);
    ui->logEdit->appendPlainText(line);
}

void MainWindow::generateSyntheticImage()
{
    const int w = 640;
    const int h = 512;
    ThermalDataModel model;
    model.setSize(w, h);
    model.setFileName(tr("synthetic_test_image.png"));
    model.setFilePath(tr("(generated)"));

    const float cx = w / 2.0f;
    const float cy = h / 2.0f;
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            const float dx = (x - cx) / (w * 0.35f);
            const float dy = (y - cy) / (h * 0.35f);
            const float dist = std::sqrt(dx * dx + dy * dy);
            const float base = 25.0f + 30.0f * std::exp(-dist * dist);
            const float noise = (std::rand() % 100) / 100.0f * 1.5f;
            model.setTemperature(x, y, base + noise);
        }
    }

    model.recomputeMinMax();
    model.setMetadataValue(tr("Width"), w);
    model.setMetadataValue(tr("Height"), h);
    model.setMetadataValue(tr("Source"), tr("Synthetic test data"));
    model.setMetadataValue(tr("Min Temperature"), model.minTemperature());
    model.setMetadataValue(tr("Max Temperature"), model.maxTemperature());

    m_models.clear();
    m_imagePoints.clear();
    m_imageBoxes.clear();
    m_currentModelIndex = -1;
    invalidateThumbnailCache();
    ui->fileList->clear();
    m_batchProcessor.clear();

    m_models.append(model);
    m_imagePoints.append(QList<QPoint>());
    m_imageBoxes.append(QList<ThermalBox>());

    QListWidgetItem *item = new QListWidgetItem(model.fileName());
    item->setIcon(thumbnailIconForIndex(0));
    ui->fileList->addItem(item);
    ui->fileList->setCurrentRow(0);

    addLogMessage(tr("Generated synthetic %1x%2 thermal image.").arg(w).arg(h));
}

QString MainWindow::exportModel(const ThermalDataModel &model,
                                const QList<QPoint> &points,
                                const QList<ThermalBox> &boxes,
                                const QString &outputPath,
                                OverlayExporter::OverlayFlagSet flags)
{
    ColorMap::Palette palette = m_rawMode ? ColorMap::RawGrayscale : m_currentPalette;

    QImage baseImage;
    if (m_rawMode && !model.previewImage().isNull()) {
        // Export the original image with overlays in grayscale mode.
        baseImage = model.previewImage();
    }

    OverlayExporter::Result result = OverlayExporter::render(
        model, palette, points, boxes, flags, m_manualMin, m_manualMax, baseImage);
    return OverlayExporter::save(result, outputPath);
}

QString MainWindow::writeCsv(const ThermalDataModel &model,
                             const QList<QPoint> &points,
                             const QList<ThermalBox> &boxes,
                             const QString &csvPath)
{
    QFile file(csvPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return tr("Could not open %1 for writing").arg(csvPath);

    QTextStream out(&file);
    out << "# For boxes, X,Y is the top-left corner of the box.\n";
    out << "# For points, X,Y is the pixel coordinate.\n";
    out << "Type,ID,X,Y,Width,Height,Avg_C,Min_C,Max_C\n";

    for (int i = 0; i < points.size(); ++i) {
        const QPoint p = points[i];
        const float t = model.temperatureAt(p.x(), p.y());
        out << "Point,P" << (i + 1) << ","
            << p.x() << "," << p.y() << ",,,"
            << QString::number(t, 'f', 2) << ",,\n";
    }

    for (int i = 0; i < boxes.size(); ++i) {
        const ThermalBox &b = boxes[i];
        out << "Box,B" << (i + 1) << ","
            << b.rect.x() << "," << b.rect.y() << ","
            << b.rect.width() << "," << b.rect.height() << ","
            << QString::number(b.avgTemperature, 'f', 2) << ","
            << QString::number(b.minTemperature, 'f', 2) << ","
            << QString::number(b.maxTemperature, 'f', 2) << "\n";
    }

    file.close();
    return QString();
}

ThermalBox MainWindow::computeBoxStats(const QRect &rect) const
{
    ThermalBox box;
    box.rect = rect;

    int idx = ui->fileList->currentRow();
    if (idx < 0 || idx >= m_models.size() || !m_models[idx].isValid())
        return box;

    const ThermalDataModel &model = m_models[idx];
    const QRect r = rect.intersected(QRect(0, 0, model.width(), model.height()));
    if (r.isEmpty())
        return box;

    double sum = 0.0;
    float minT = std::numeric_limits<float>::max();
    float maxT = std::numeric_limits<float>::lowest();
    QPoint minP;
    QPoint maxP;
    int count = 0;

    for (int y = r.top(); y <= r.bottom(); ++y) {
        for (int x = r.left(); x <= r.right(); ++x) {
            const float t = model.temperatureAt(x, y);
            sum += t;
            if (t < minT) { minT = t; minP = QPoint(x, y); }
            if (t > maxT) { maxT = t; maxP = QPoint(x, y); }
            ++count;
        }
    }

    if (count > 0) {
        box.avgTemperature = static_cast<float>(sum / count);
        box.minTemperature = minT;
        box.maxTemperature = maxT;
        box.minPixel = minP;
        box.maxPixel = maxP;
    }

    return box;
}

void MainWindow::on_boxDrawn(const QRect &rect)
{
    int idx = ui->fileList->currentRow();
    if (idx < 0 || idx >= m_imageBoxes.size())
        return;

    ThermalBox box = computeBoxStats(rect);
    if (box.rect.width() > 0 && box.rect.height() > 0) {
        m_imageBoxes[idx].append(box);
        applyBoxesToView(idx);
        updateBoxesList();
    }
}

void MainWindow::updateBoxesList()
{
    ui->boxesList->clear();
    int idx = ui->fileList->currentRow();
    if (idx < 0 || idx >= m_imageBoxes.size()) {
        ui->boxesGroup->setVisible(false);
        return;
    }

    for (int i = 0; i < m_imageBoxes[idx].size(); ++i) {
        const ThermalBox &b = m_imageBoxes[idx][i];
        ui->boxesList->addItem(tr("B%1: (%2,%3 %4x%5) avg %6C min %7C max %8C")
                                   .arg(i + 1)
                                   .arg(b.rect.x())
                                   .arg(b.rect.y())
                                   .arg(b.rect.width())
                                   .arg(b.rect.height())
                                   .arg(b.avgTemperature, 0, 'f', 1)
                                   .arg(b.minTemperature, 0, 'f', 1)
                                   .arg(b.maxTemperature, 0, 'f', 1));
    }
    ui->boxesGroup->setVisible(!m_imageBoxes[idx].isEmpty());
}

void MainWindow::applyBoxesToView(int index)
{
    if (index < 0 || index >= m_imageBoxes.size())
        return;
    m_imageView->setBoxes(m_imageBoxes[index]);
}

QString MainWindow::suggestExportFileName(const ThermalDataModel &model) const
{
    const QString base = QFileInfo(model.filePath()).completeBaseName();
    return QDir::currentPath() + "/" + base + "_overlay.png";
}

QList<int> MainWindow::selectImagesForExport()
{
    QList<int> result;

    if (m_models.size() == 1) {
        result.append(0);
        return result;
    }

    QDialog dialog(this);
    dialog.setWindowTitle(tr("Select Images to Export"));
    dialog.setMinimumWidth(450);
    dialog.setMinimumHeight(400);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    // View-mode buttons for the export selection list.
    QHBoxLayout *viewLayout = new QHBoxLayout();
    QPushButton *listButton = new QPushButton(tr("List"), &dialog);
    QPushButton *gridButton = new QPushButton(tr("Grid"), &dialog);
    QPushButton *thumbButton = new QPushButton(tr("Thumbnails"), &dialog);
    viewLayout->addWidget(listButton);
    viewLayout->addWidget(gridButton);
    viewLayout->addWidget(thumbButton);
    viewLayout->addStretch();
    layout->addLayout(viewLayout);

    QListWidget *listWidget = new QListWidget(&dialog);
    listWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    listWidget->setSpacing(4);
    layout->addWidget(listWidget);

    auto applyViewMode = [&](QListWidget::ViewMode mode, bool withThumbnails) {
        listWidget->setViewMode(mode);
        if (mode == QListWidget::ListMode) {
            listWidget->setIconSize(QSize(0, 0));
            listWidget->setFlow(QListWidget::TopToBottom);
            listWidget->setWrapping(false);
        } else {
            listWidget->setIconSize(withThumbnails ? QSize(96, 72) : QSize(48, 48));
            listWidget->setFlow(QListWidget::LeftToRight);
            listWidget->setWrapping(true);
            listWidget->setResizeMode(QListWidget::Adjust);

            for (int i = 0; i < listWidget->count(); ++i) {
                int idx = listWidget->item(i)->data(Qt::UserRole).toInt();
                if (withThumbnails && idx >= 0 && idx < m_models.size()) {
                    listWidget->item(i)->setIcon(thumbnailIconForIndex(idx));
                } else {
                    QPixmap generic(48, 48);
                    generic.fill(Qt::lightGray);
                    listWidget->item(i)->setIcon(QIcon(generic));
                }
            }
        }
    };

    connect(listButton, &QPushButton::clicked, &dialog, [&](){ applyViewMode(QListWidget::ListMode, false); });
    connect(gridButton, &QPushButton::clicked, &dialog, [&](){ applyViewMode(QListWidget::IconMode, false); });
    connect(thumbButton, &QPushButton::clicked, &dialog, [&](){ applyViewMode(QListWidget::IconMode, true); });

    // Pre-select images that have overlays (user points) and the current image.
    bool anyOverlays = false;
    for (int i = 0; i < m_imagePoints.size(); ++i) {
        if (!m_imagePoints[i].isEmpty()) {
            anyOverlays = true;
            break;
        }
    }
    const int currentRow = ui->fileList->currentRow();

    for (int i = 0; i < m_models.size(); ++i) {
        QListWidgetItem *item = new QListWidgetItem(m_models[i].fileName(), listWidget);
        item->setData(Qt::UserRole, i);

        bool selected = (i == currentRow);
        if (anyOverlays && !m_imagePoints[i].isEmpty())
            selected = true;
        item->setSelected(selected);
    }

    // Default to thumbnail view so the user sees what they are selecting.
    applyViewMode(QListWidget::IconMode, true);

    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttons);

    if (dialog.exec() != QDialog::Accepted)
        return result;

    for (QListWidgetItem *item : listWidget->selectedItems()) {
        result.append(item->data(Qt::UserRole).toInt());
    }

    std::sort(result.begin(), result.end());
    return result;
}

void MainWindow::applyPointsToView(int index)
{
    if (index < 0 || index >= m_imagePoints.size())
        return;

    m_imageView->blockSignals(true);
    m_imageView->clearUserPoints();
    for (const QPoint &p : std::as_const(m_imagePoints[index])) {
        m_imageView->addUserPoint(p);
    }
    m_imageView->blockSignals(false);

    updatePointsList();
}

void MainWindow::saveCurrentViewPoints()
{
    int idx = ui->fileList->currentRow();
    if (idx >= 0 && idx < m_imagePoints.size()) {
        m_imagePoints[idx] = m_imageView->userPoints();
    }
}

void MainWindow::saveCurrentViewBoxes()
{
    int idx = ui->fileList->currentRow();
    if (idx >= 0 && idx < m_imageBoxes.size()) {
        m_imageBoxes[idx] = m_imageView->boxes();
    }
}

void MainWindow::removeCurrentImage()
{
    int idx = ui->fileList->currentRow();
    if (idx < 0 || idx >= m_models.size())
        return;

    // Remove from data model, points, boxes, and cache.
    m_models.removeAt(idx);
    m_imagePoints.removeAt(idx);
    m_imageBoxes.removeAt(idx);
    m_thumbnailCache.clear();   // simple invalidation; indices changed
    m_currentModelIndex = -1;

    delete ui->fileList->takeItem(idx);

    if (m_models.isEmpty()) {
        ui->fileList->setCurrentRow(-1);
        m_imageView->clear();
        ui->metadataTree->model()->removeRows(0, ui->metadataTree->model()->rowCount());
        ui->pointsList->clear();
        ui->boxesList->clear();
        ui->minSpin->setEnabled(false);
        ui->maxSpin->setEnabled(false);
        ui->autoRangeButton->setEnabled(false);
        ui->statusLabel->setText(tr("No images loaded."));
    } else {
        int newRow = qBound(0, idx, m_models.size() - 1);
        ui->fileList->setCurrentRow(newRow);
    }
}

QIcon MainWindow::createThumbnailIcon(const ThermalDataModel &model)
{
    if (!model.isValid())
        return QIcon();

    QImage thumb = ColorMap::render(model, ColorMap::IronBow, std::nullopt, std::nullopt);
    if (thumb.isNull())
        return QIcon();

    thumb = thumb.scaled(96, 72, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    return QIcon(QPixmap::fromImage(thumb));
}

QString MainWindow::thumbnailCacheKey(int index, ColorMap::Palette palette, bool grayscale) const
{
    return QString::number(index) + QLatin1Char(':')
           + QString::number(static_cast<int>(palette)) + QLatin1Char(':')
           + QString::number(grayscale ? 1 : 0);
}

QIcon MainWindow::thumbnailIconForIndex(int index)
{
    if (index < 0 || index >= m_models.size())
        return QIcon();

    const QString key = thumbnailCacheKey(index, m_currentPalette, m_rawMode);
    auto it = m_thumbnailCache.find(key);
    if (it != m_thumbnailCache.end())
        return it.value();

    const ThermalDataModel &model = m_models[index];
    if (!model.isValid())
        return QIcon();

    QImage thumb;
    if (m_rawMode && !model.previewImage().isNull()) {
        thumb = model.previewImage();
    } else {
        ColorMap::Palette palette = m_rawMode ? ColorMap::RawGrayscale : m_currentPalette;
        thumb = ColorMap::render(model, palette, std::nullopt, std::nullopt);
    }

    if (thumb.isNull())
        return QIcon();

    thumb = thumb.scaled(96, 72, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QIcon icon(QPixmap::fromImage(thumb));
    m_thumbnailCache.insert(key, icon);
    return icon;
}

void MainWindow::invalidateThumbnailCache()
{
    m_thumbnailCache.clear();
}

void MainWindow::refreshAllThumbnails()
{
    for (int i = 0; i < ui->fileList->count(); ++i) {
        if (i < m_models.size()) {
            ui->fileList->item(i)->setIcon(thumbnailIconForIndex(i));
        }
    }
}

void MainWindow::setFileListViewMode(QListWidget::ViewMode mode, bool withThumbnails)
{
    ui->fileList->setViewMode(mode);

    if (mode == QListWidget::ListMode) {
        ui->fileList->setIconSize(QSize(0, 0));
        ui->fileList->setFlow(QListWidget::TopToBottom);
        ui->fileList->setWrapping(false);
    } else {
        // Grid / thumbnail view
        ui->fileList->setIconSize(withThumbnails ? QSize(96, 72) : QSize(48, 48));
        ui->fileList->setFlow(QListWidget::LeftToRight);
        ui->fileList->setWrapping(true);
        ui->fileList->setResizeMode(QListWidget::Adjust);

        // Re-generate icons if thumbnail mode is requested.
        if (withThumbnails) {
            for (int i = 0; i < ui->fileList->count(); ++i) {
                if (i < m_models.size()) {
                    ui->fileList->item(i)->setIcon(thumbnailIconForIndex(i));
                }
            }
        } else {
            // Generic file icon for grid mode without thumbnails.
            QPixmap generic(48, 48);
            generic.fill(Qt::lightGray);
            QIcon icon(generic);
            for (int i = 0; i < ui->fileList->count(); ++i) {
                ui->fileList->item(i)->setIcon(icon);
            }
        }
    }
}
