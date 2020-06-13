#include "mainwindow.h"
#include "statestorage.h"
#include "models/pageitem.h"
#include "shape.h"

#include <QMenu>
#include <QMenuBar>
#include <QPixmap>
#include <QToolBar>
#include <QFileDialog>
#include <QImageReader>
#include <QSignalMapper>
#include <QGridLayout>
#include <QMessageBox>
#include <QScrollArea>
#include <QImageReader>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    settings_ = new QSettings("OcrLabel", "OcrLabel");

    createActions();
    createMeanu();
    createToolBar();
    QScrollArea *canvasArea = new QScrollArea(this);
    canvas_ = new Canvas(this);
    canvas_->setEnabled(false);
    canvas_->setContextMenuPolicy(Qt::CustomContextMenu);
    canvasArea->setWidget(canvas_);
    canvasArea->setWidgetResizable(true);
    setCentralWidget(canvasArea);

    labelWidget_ = new LabelWidget(this);
    rightLabelDockWidget_ = new QDockWidget("labels in project");
    rightLabelDockWidget_->setWidget(labelWidget_);
    rightLabelDockWidget_->setFeatures(QDockWidget::NoDockWidgetFeatures);
    addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, rightLabelDockWidget_);
    currentDir_ = "";

    imageNameListWidget_ = new QListWidget(this);
    connect(imageNameListWidget_, SIGNAL(itemSelectionChanged()), this, SLOT(fileSelectionChange()));
    QDockWidget *imageDockWidget = new QDockWidget(this);
    imageDockWidget->setWidget(imageNameListWidget_);
    addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, imageDockWidget);

    connect(canvas_, SIGNAL(addNewShape(Shape*)), this, SLOT(addNewShape(Shape *)));
//    mainlayout->addWidget(canvas_, 0, 0);
//    mainlayout->addWidget(labelWidget_, 0, 1);


}


MainWindow::~MainWindow()
{

}

// 新建所有 action
void MainWindow::createActions(){
    openFileAction_ = new QAction(QPixmap(":/icons/icons/open.png"), "Open");
    openDirAction_ = new QAction(QPixmap(":/icons/icons/open.png"), "OpenDir");
    zoomIn_ = new QAction(QPixmap(":/icons/icons/zoom-in.png"), "ZoomIn");
    zoomOut_ = new QAction(QPixmap(":/icons/icons/zoom-out.png"), "ZoomOut");


    connect(openFileAction_, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(openDirAction_ , SIGNAL(triggered()), this, SLOT(openDir()));
    connect(zoomIn_, &QAction::triggered, this, [this]{this->addZoom(1.1);});
    connect(zoomOut_, &QAction::triggered, this, [this]{this->addZoom(0.9);});


    createMode_ = new QAction(QPixmap(":/icons/icons/polygon.png"), "Create Polygons");
    createRectangleMode_ = new QAction(QPixmap(":/icons/icons/rectangular.png"), "Create Rectangle");
    createCircleMode_ = new QAction(QPixmap(":/icons/icons/circle.png"), "Create Circle");
    createLineMode_ = new QAction(QPixmap(":/icons/icons/line.png"), "Create Line");
    createPointMode_ = new QAction(QPixmap(":/icons/icons/point.png"), "Create Point");
    editMode_ = new QAction(QPixmap(":/icons/icons/edit.png"), "Edit");
    deleteShape_ = new QAction(QPixmap(":/icons/icons/delete.png"), "Delete Polygons");

    saveAction_ = new QAction(QPixmap(":/icons/icons/save.png"), "Save");

    connect(createMode_, &QAction::triggered, this, [this]{this->updateMode(false, Shape::POLYGON);});
    connect(createRectangleMode_, &QAction::triggered, this, [this]{this->updateMode(false, Shape::RECTANGLE);});
    connect(createCircleMode_, &QAction::triggered, this, [this]{this->updateMode(false, Shape::CIRCLE);});
    connect(createLineMode_, &QAction::triggered, this, [this]{this->updateMode(false, Shape::LINE);});
    connect(createPointMode_, &QAction::triggered, this, [this]{this->updateMode(false, Shape::POINT);});
    connect(editMode_, &QAction::triggered, this, [this]{this->updateMode(true, Shape::NONE);});
    connect(deleteShape_, &QAction::triggered, this, [this]{this->deleteSelectShape();});
    connect(saveAction_, &QAction::triggered, this, [this]{this->saveLabeFile();});

    // project
//    newProject_ = new QAction(QPixmap(":/icons/icons/new-project.png"), "New Project");
//    connect(newProject_, &QAction::triggered, this, [this]{this->newProject();});

    binaryAction_ = new QAction(QPixmap(":/icons/icons/analysis.png"), "Auto Label");
    connect(binaryAction_, &QAction::triggered, this, [this]{this->startBinaryDialog();});
}


// 新建菜单
void MainWindow::createMeanu(){
    QMenu *file = menuBar()->addMenu(tr("&File"));
    file->addAction(openFileAction_);
    file->addAction(openDirAction_);
    file->addSeparator();

    QMenu *edit = menuBar()->addMenu(tr("&Edit"));

    edit->addAction(createMode_);
    edit->addAction(createRectangleMode_);
    edit->addAction(createCircleMode_);
    edit->addAction(createLineMode_);
    edit->addAction(createPointMode_);
}


// 新建 ToolBar
void MainWindow::createToolBar(){
    QToolBar *toolbar = new QToolBar("tools");
    toolbar->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextUnderIcon);
//    toolbar->addAction(newProject_);
    toolbar->addSeparator();

    toolbar->addAction(openFileAction_);
    toolbar->addSeparator();
    toolbar->addAction(openDirAction_);
    toolbar->addSeparator();


    toolbar->addAction(saveAction_);
    toolbar->addSeparator();

    toolbar->addAction(zoomIn_);
    zoomSpinBox_ = new QSpinBox();
    zoomSpinBox_->setRange(1, 1000);
    zoomSpinBox_->setToolTip("Zoom Level");
    zoomSpinBox_->setSuffix("%");
    zoomSpinBox_->setValue(100);
    zoomSpinBox_->setAlignment(Qt::AlignCenter);
    connect(zoomSpinBox_, SIGNAL(valueChanged(int)), this, SLOT(resizeCanvas()));

    toolbar->addWidget(zoomSpinBox_);
    toolbar->addAction(zoomOut_);

    addToolBar(Qt::ToolBarArea::LeftToolBarArea, toolbar);
    QToolBar *topToolbar = new QToolBar("toptool");
    topToolbar->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextUnderIcon);
    topToolbar->addAction(createMode_);
    topToolbar->addSeparator();
    topToolbar->addAction(createRectangleMode_);
    topToolbar->addSeparator();
    topToolbar->addAction(createCircleMode_);
    topToolbar->addSeparator();
    topToolbar->addAction(createLineMode_);
    topToolbar->addSeparator();
    topToolbar->addAction(createPointMode_);
    topToolbar->addSeparator();
    topToolbar->addAction(editMode_);
    topToolbar->addSeparator();
    topToolbar->addAction(deleteShape_);
    topToolbar->addSeparator();
    topToolbar->addAction(binaryAction_);

    addToolBar(Qt::TopToolBarArea, topToolbar);
}


// 打开文件对话框，读取所有图片文件
void MainWindow::openDir(){
    qDebug() << "opendir" ;
    QString defaultDir = QDir::homePath();
    QString dirName = QFileDialog::getExistingDirectory(
                this,
                "Open image dir",
                defaultDir,
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
                );
    qDebug() <<"opendir: " << dirName;
    if(dirName.isEmpty()){
        return;
    }
    currentDir_ = dirName;
    importDirImages(dirName);
}


void MainWindow::resizeCanvas(){
    StateStorage::scale = 0.01 * zoomSpinBox_->value();
    qDebug() << "resize canvas: " << zoomSpinBox_->value();
    canvas_->adjustSize();
    canvas_->update();
}

// 打开一个文件对话框
void MainWindow::openFile(){
    qDebug()<< "OpenFile";
    QString currPath = QDir::currentPath();
    QImageReader::supportedImageFormats();
    QFileDialog dialog;
    QList<QByteArray>  formats = QImageReader::supportedImageFormats();
    QString filter = "Image Files (";
    for(int i=0; i<formats.size(); i++){
        filter +=  "*." + formats[i] + " ";
    }
    filter += ")";
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), currPath, filter);
    QFileInfo fInfo = QFileInfo(fileName);
    currentDir_ = fInfo.dir().makeAbsolute();
    qDebug() <<  currentDir_;
    loadImage(fileName);
}

// 读取图片文件
void MainWindow::loadImage(QString imagePath){
    currImage_ = QImage(imagePath);
    double scale = adjustWindowScale();
    StateStorage::scale = scale;
    qDebug() << scale << " scale";
    zoomSpinBox_->setValue(100 * scale);
    currentImagePath_ = imagePath;
    paintCanvas();
}


double MainWindow::adjustWindowScale(){
    int windowWidth = centralWidget()->width();
    int windowHeight = centralWidget()->height();
    int imageWidth = currImage_.width();
    int imageHeight = currImage_.height();
    double rw = 1.0 * windowWidth / windowHeight;
    double ri = 1.0 * imageWidth / imageHeight;
    if (rw >  ri){
        return 1.0* windowHeight/imageHeight;
    }
    return 1.0 * windowWidth / imageWidth;
}


void MainWindow::paintCanvas(){
    canvas_->setEnabled(true);
    canvas_->setPixImage(QPixmap::fromImage(currImage_));
}

void MainWindow::addZoom(double increment){
    double zv = zoomSpinBox_->value() * increment;
    zoomSpinBox_->setValue(zv);
}

void MainWindow::deleteSelectShape(){
    if(deleteShape_->isEnabled() && canvas_->hasSelected()){
        QMessageBox msgBox;
        msgBox.setText("Delete Shape");
        msgBox.setInformativeText("Do you whant to delete this Shape");
        msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();
        if(ret == QMessageBox::Cancel){
            return;
        }
        canvas_->deleteSelectedShape();
    }
}


void MainWindow::updateMode(bool isEdit, Shape::ShapeType shapeType) {
    if(isEdit){
        StateStorage::canvasMode = Canvas::EDIT;
    }else{
        StateStorage::canvasMode = Canvas::CREATE;
    }
    qDebug()<<"update mode" << StateStorage::labelName;
    if(!isEdit && StateStorage::labelName.isEmpty()){
        QMessageBox msgBox ;
        msgBox.setText("no label selected");
        msgBox.setInformativeText("please create or select a label name in this project");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }
    editMode_->setEnabled(!isEdit);
    StateStorage::drawingShapeType = shapeType;
    if(isEdit){
        createMode_->setEnabled(true);
        createPointMode_->setEnabled(true);
        createLineMode_->setEnabled(true);
        createRectangleMode_->setEnabled(true);
        createCircleMode_->setEnabled(true);
    } else {
        if(StateStorage::drawingShapeType==Shape::POINT){
          createPointMode_->setEnabled(false);
          createMode_->setEnabled(true);
          createLineMode_->setEnabled(true);
          createRectangleMode_->setEnabled(true);
          createCircleMode_->setEnabled(true);
        }
        else if(StateStorage::drawingShapeType==Shape::LINE){
          createPointMode_->setEnabled(true);
          createLineMode_->setEnabled(false);
          createMode_->setEnabled(true);
          createRectangleMode_->setEnabled(true);
          createCircleMode_->setEnabled(true);
        }
        else if(StateStorage::drawingShapeType == Shape::RECTANGLE){
          createPointMode_->setEnabled(true);
          createLineMode_->setEnabled(true);
          createRectangleMode_->setEnabled(false);
          createMode_->setEnabled(true);
          createCircleMode_->setEnabled(true);
        }else if(StateStorage::drawingShapeType == Shape::POLYGON){
          createPointMode_->setEnabled(true);
          createLineMode_->setEnabled(true);
          createRectangleMode_->setEnabled(true);
          createMode_->setEnabled(false);
          createCircleMode_->setEnabled(true);
        }
        else if(StateStorage::drawingShapeType== Shape::CIRCLE){
          createPointMode_->setEnabled(true);
          createLineMode_->setEnabled(true);
          createRectangleMode_->setEnabled(true);
          createMode_->setEnabled(true);
          createCircleMode_->setEnabled(false);
        }
    }
}

//void MainWindow::openProject(QString fileName){
//}

void MainWindow::importDirImages(QString dirName){
    QList<QByteArray>  formats = QImageReader::supportedImageFormats();
    qDebug() << formats ;
    QDir directory(dirName);

    QStringList filters;
    for(int i=0; i<formats.size() ; i++){
        filters<< "*." + formats[i];
    }
    QStringList images = directory.entryList(filters, QDir::Files);
    imageNames_.clear();
    foreach(QString filename, images){
        QString absPath = QDir::cleanPath(currentDir_ + QDir::separator() + filename);
        if(imageNames_.contains(absPath)){
            continue;
        }
        imageNames_.append(absPath);
        QListWidgetItem *item = new QListWidgetItem(absPath);
        // todo 判断是否存在标签文件
        QString labelFilePath = getLabelFileName(absPath);
        QFileInfo labelFileInfo = QFileInfo(labelFilePath);
        if(labelFileInfo.exists()){
            item->setCheckState(Qt::Checked);
        }else {
            item->setCheckState(Qt::Unchecked);
        }
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        imageNameListWidget_->addItem(item);
    }
}

void MainWindow::fileSelectionChange(){
     QList<QListWidgetItem*> items  = imageNameListWidget_->selectedItems();
     if(items.size()<=0) {
         return;
     }
     if(canvas_->isNeedSave()){
         // TODO save
         QMessageBox msgBox;
         msgBox.setText("save !");
         msgBox.setInformativeText("Do you whant to save !");
         msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
         msgBox.setDefaultButton(QMessageBox::Ok);
         int res  = msgBox.exec();
         if(res==QMessageBox::Cancel){
             return;
         }
     }
     if(canvas_->shapeSize()>0){
         saveLabeFile();
     }
     QListWidgetItem *item = items[0];
     QString fileName = item->text();
     QString labelFileName = getLabelFileName(fileName);
     loadImage(fileName);
     loadLabelFile(labelFileName);

}

void MainWindow::saveLabeFile(){
    if(currentImagePath_.isEmpty()){
        return;
    }
    QString saveFilePath = getLabelFileName(currentImagePath_);
    QJsonArray shapeArray = canvas_->getShapeAsJson();
    QJsonObject outObj ;
    outObj["shapes"] = shapeArray;
    outObj["imagePath"] = currentImagePath_;
    outObj["imageWidth"] = currImage_.width();
    outObj["imageHeight"] = currImage_.height();
    QFile outf(saveFilePath);
    outf.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QJsonDocument outDoc(outObj);
    if(outf.isOpen()){
        outf.write(outDoc.toJson());
    }
    outf.close();
    qDebug() << saveFilePath;
    int index = imageNames_.indexOf(currentImagePath_);
    if(index >= 0){
       qDebug() << "current image index: " << index;
       imageNameListWidget_->item(index)->setCheckState(Qt::Checked);
    }
    canvas_->resetState();
    // TODO change file list checked state

}



QString MainWindow::getLabelFileName(QString imagePath){
    if(imagePath.isEmpty()){
        return "";
    }
    QFileInfo info(imagePath);
    QString baseName = info.baseName();
    QString absPath = QDir::cleanPath(info.path() + QDir::separator() + baseName+".json");
    return absPath;
}

void MainWindow::loadLabelFile(QString fileName){
    QFile loadFile(fileName);
    if(!loadFile.exists()){
        return;
    }
    loadFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray json_data = loadFile.readAll();
    auto jsonDoc = QJsonDocument::fromJson(json_data);
    auto shapes = jsonDoc["shapes"];
    QString imagePath = jsonDoc["imagePath"].toString();
    canvas_->restoreShapeFromJson(shapes.toArray());

}

void MainWindow::addNewShape(Shape *shape){
    labelWidget_->addLabel(shape->getLabel(), shape->getLineColor());
}

void MainWindow::startBinaryDialog(){
    if(currImage_.isNull()){
        return;
    }
    if(autoLabelDialog_==nullptr){
       autoLabelDialog_ = new AutoLabelDialog(currImage_, this);
       connect(autoLabelDialog_, SIGNAL(finishAndSave()), this, SLOT(saveAutoLabelResult()));
    }else {
     autoLabelDialog_->setSourceImage(currImage_);
    }
    autoLabelDialog_->show();
}

void MainWindow::saveAutoLabelResult(){
    qDebug() << "save autto label result";
    std::vector<ocrmodel::PageItem> autoResult = autoLabelDialog_->getResult();

    if(!labelWidget_->hasLabel("textline")){
       labelWidget_->addLabel("textline");
    }

    for(ocrmodel::PageItem pit:autoResult){
        Shape *shape = new Shape("textline", Shape::POLYGON);
        shape->setShapeState(Shape::ShapeState::NoState);
        for(ocrmodel::Point p: pit.getPoints()){
            shape->addPoint(QPointF(p.x, p.y));
        }
        shape->setLineColor(QColor(labelWidget_->getColor("textline")));
       canvas_->addShape(shape);
    }
    qDebug() << "canvas: shapes";
    canvas_->repaint();
}

