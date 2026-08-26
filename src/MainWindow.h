#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QHash>
#include <QIcon>
#include <QListWidget>
#include <QMainWindow>
#include <QStandardItemModel>

#include "BatchProcessor.h"
#include "ColorMap.h"
#include "OverlayExporter.h"
#include "ThermalBox.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class ImageView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void loadFile(const QString &filePath);
    void loadFiles(const QStringList &filePaths);
    QStringList collectImageFiles(const QStringList &paths) const;

private slots:
    void on_actionOpen_triggered();
    void on_actionGenerateTestImage_triggered();
    void on_actionExit_triggered();
    void on_actionAbout_triggered();

    void on_actionZoomIn_triggered();
    void on_actionZoomOut_triggered();
    void on_actionResetZoom_triggered();
    void on_actionToggleMinMax_triggered();
    void on_actionToggleCrosshair_triggered();
    void on_actionExport_triggered();
    void on_actionRemoveLastPoint_triggered();
    void on_actionClearPoints_triggered();
    void on_actionRemoveLastBox_triggered();
    void on_actionClearBoxes_triggered();
    void on_actionViewList_triggered();
    void on_actionViewGrid_triggered();
    void on_actionViewThumbnails_triggered();

    void on_paletteCombo_currentIndexChanged(int index);
    void on_rawCheckBox_stateChanged(int state);
    void on_minSpin_valueChanged(double value);
    void on_maxSpin_valueChanged(double value);
    void on_autoRangeButton_clicked();
    void on_clearPointsButton_clicked();
    void on_removeLastPointButton_clicked();
    void on_clearBoxesButton_clicked();
    void on_removeLastBoxButton_clicked();
    void on_viewMatrixButton_clicked();
    void on_fileList_customContextMenuRequested(const QPoint &pos);

    void on_fileList_currentRowChanged(int currentRow);

    void on_pixelHovered(int x, int y, float temperature);
    void on_pixelClicked(int x, int y, float temperature);
    void on_minMaxChanged(float min, float max);
    void on_userPointsChanged(const QList<QPoint> &points);
    void on_boxDrawn(const QRect &rect);
    void on_batchFileLoaded(int index, const ThermalDataModel &model);
    void on_batchFileFailed(const QString &path, const QString &reason);
    void on_batchFinished(int successCount, int failCount);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    void setupUi();
    void connectSignals();
    void refreshImage();
    void displayModel(int index);
    void updateMetadataPanel(const ThermalDataModel &data);
    void updatePointsList();
    void updateBoxesList();
    void addLogMessage(const QString &msg);
    void generateSyntheticImage();

    ThermalBox computeBoxStats(const QRect &rect) const;
    QString exportModel(const ThermalDataModel &model,
                        const QList<QPoint> &points,
                        const QList<ThermalBox> &boxes,
                        const QString &outputPath,
                        OverlayExporter::OverlayFlagSet flags);
    QString writeCsv(const ThermalDataModel &model,
                     const QList<QPoint> &points,
                     const QList<ThermalBox> &boxes,
                     const QString &csvPath);
    QString suggestExportFileName(const ThermalDataModel &model) const;
    QList<int> selectImagesForExport();
    void applyPointsToView(int index);
    void applyBoxesToView(int index);
    void saveCurrentViewPoints();
    void saveCurrentViewBoxes();
    void removeCurrentImage();
    QIcon createThumbnailIcon(const ThermalDataModel &model);
    QIcon thumbnailIconForIndex(int index);
    QString thumbnailCacheKey(int index, ColorMap::Palette palette, bool grayscale) const;
    void invalidateThumbnailCache();
    void refreshAllThumbnails();
    void setFileListViewMode(QListWidget::ViewMode mode, bool withThumbnails);

    Ui::MainWindow *ui;
    ImageView *m_imageView = nullptr;
    BatchProcessor m_batchProcessor;
    QList<ThermalImageLoader *> m_ownedLoaders;
    QList<ThermalDataModel> m_models;
    QVector<QList<QPoint>> m_imagePoints;
    QVector<QList<ThermalBox>> m_imageBoxes;
    int m_currentModelIndex = -1;
    QHash<QString, QIcon> m_thumbnailCache;

    ColorMap::Palette m_currentPalette = ColorMap::IronBow;
    bool m_rawMode = false;
    std::optional<float> m_manualMin;
    std::optional<float> m_manualMax;
};

#endif // MAINWINDOW_H
