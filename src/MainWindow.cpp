#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QAction>
#include <QActionGroup>
#include <QCheckBox>
#include <QDateTime>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QListWidget>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QModelIndex>
#include <QStandardItemModel>
#include <QVBoxLayout>

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

    // Default spin boxes disabled until data loaded
    ui->minSpin->setEnabled(false);
    ui->maxSpin->setEnabled(false);
    ui->autoRangeButton->setEnabled(false);

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

    connect(&m_batchProcessor, &BatchProcessor::fileLoaded, this, &MainWindow::on_batchFileLoaded);
    connect(&m_batchProcessor, &BatchProcessor::fileFailed, this, &MainWindow::on_batchFileFailed);
    connect(&m_batchProcessor, &BatchProcessor::finished, this, &MainWindow::on_batchFinished);
}

void MainWindow::loadFile(const QString &filePath)
{
    loadFiles(QStringList() << filePath);
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
    const QString filter = tr("Thermal Images (*.png *.tif *.tiff *.jpg *.jpeg);;All Files (*)");
    const QStringList files = QFileDialog::getOpenFileNames(
        this, tr("Open Images"), QString(), filter);
    if (files.isEmpty())
        return;

    loadFiles(files);
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

        QString err = exportModel(m_models[idx], m_imagePoints[idx], outputPath,
                                  OverlayExporter::AllOverlays);
        if (err.isEmpty()) {
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
            const QString outputPath = dir + "/" + QFileInfo(model.fileName()).completeBaseName() + "_overlay.png";
            QString err = exportModel(model, m_imagePoints[idx], outputPath,
                                      OverlayExporter::AllOverlays);
            if (err.isEmpty()) {
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
    const QList<QPoint> points = m_imageView->userPoints();

    int idx = ui->fileList->currentRow();
    const bool haveModel = (idx >= 0 && idx < m_models.size() && m_models[idx].isValid());
    if (!haveModel)
        return;

    for (int i = 0; i < points.size(); ++i) {
        const QPoint p = points[i];
        float t = m_models[idx].temperatureAt(p.x(), p.y());
        ui->pointsList->addItem(tr("P%1: (%2, %3) %4 C")
                                    .arg(i + 1)
                                    .arg(p.x())
                                    .arg(p.y())
                                    .arg(t, 0, 'f', 2));
    }
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
    m_currentModelIndex = -1;
    invalidateThumbnailCache();
    ui->fileList->clear();
    m_batchProcessor.clear();

    m_models.append(model);
    m_imagePoints.append(QList<QPoint>());

    QListWidgetItem *item = new QListWidgetItem(model.fileName());
    item->setIcon(thumbnailIconForIndex(0));
    ui->fileList->addItem(item);
    ui->fileList->setCurrentRow(0);

    addLogMessage(tr("Generated synthetic %1x%2 thermal image.").arg(w).arg(h));
}

QString MainWindow::exportModel(const ThermalDataModel &model, const QList<QPoint> &points,
                                const QString &outputPath, OverlayExporter::OverlayFlagSet flags)
{
    ColorMap::Palette palette = m_rawMode ? ColorMap::RawGrayscale : m_currentPalette;

    QImage baseImage;
    if (m_rawMode && !model.previewImage().isNull()) {
        // Export the original image with overlays in grayscale mode.
        baseImage = model.previewImage();
    }

    OverlayExporter::Result result = OverlayExporter::render(
        model, palette, points, flags, m_manualMin, m_manualMax, baseImage);
    return OverlayExporter::save(result, outputPath);
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

void MainWindow::removeCurrentImage()
{
    int idx = ui->fileList->currentRow();
    if (idx < 0 || idx >= m_models.size())
        return;

    // Remove from data model, points, and cache.
    m_models.removeAt(idx);
    m_imagePoints.removeAt(idx);
    m_thumbnailCache.clear();   // simple invalidation; indices changed
    m_currentModelIndex = -1;

    delete ui->fileList->takeItem(idx);

    if (m_models.isEmpty()) {
        ui->fileList->setCurrentRow(-1);
        m_imageView->clear();
        ui->metadataTree->model()->removeRows(0, ui->metadataTree->model()->rowCount());
        ui->pointsList->clear();
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
