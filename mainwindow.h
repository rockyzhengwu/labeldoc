#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QImage>
#include <QString>
#include <QSpinBox>
#include <QSettings>
#include <QDockWidget>

#include  "canvas.h"
#include "labelwidget.h"
#include "filelistwidget.h"
#include "shape.h"
#include "autolabeldialog.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void createMeanu();
    void createToolBar();
    void createActions();

    void newProject();
    void saveLabeFile();
    void loadLabelFile(QString fileName);

    ~MainWindow();

protected slots:
    // create shape
    void openFile();
    void openDir();
    void updateMode(bool isEdit, Shape::ShapeType shapeType);
    void addZoom(double increment);
    void resizeCanvas();
    void deleteSelectShape();

    // project
//    void openProject(QString filename = "");
    void fileSelectionChange();
    void addNewShape(Shape *shape);
    void startAutoLavelDialog();

    // auto label
    void saveAutoLabelResult();



private:
    QAction *openFileAction_;
    QAction *openDirAction_;

    QAction *zoomIn_;
    QAction *zoomOut_;

    QAction *createMode_;
    QAction *createRectangleMode_;
    QAction *createCircleMode_;
    QAction *createLineMode_;
    QAction *createPointMode_;
    QAction *editMode_;

    QAction *deleteShape_;

    QAction *binaryAction_;

//    QAction *newProject_;
    QAction *saveAction_;

    QImage currImage_;
    Canvas *canvas_;

    QSpinBox *zoomSpinBox_;

    void loadImage(QString imagePath);
    void paintCanvas();
    double adjustWindowScale();
    void addZoom(float ratio);


    QSettings* settings_;


    LabelWidget *labelWidget_;

    QDockWidget *rightLabelDockWidget_;

    void importDirImages(QString dirName);
    QString currentDir_;
    QString currentImagePath_;

    QStringList imageNames_;
    QListWidget *imageNameListWidget_;

    QString getLabelFileName(QString imagePath);

    AutoLabelDialog *autoLabelDialog_;

};

#endif // MAINWINDOW_H
