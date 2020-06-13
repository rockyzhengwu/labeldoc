#include "labelwidget.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QRadioButton>
#include <QMessageBox>
#include <QDebug>
#include <QLayoutItem>
#include <QColor>
#include <QStringList>

#include <QDebug>

#include "statestorage.h"
LabelWidget::LabelWidget(QWidget *parent):QWidget (parent),
    uniqueLabels_(),
    allButtons_()
{
    createUI();
}

void LabelWidget::createUI(){
    for(QString s: uniqueLabels_){
        addLabel(s);
    }

    uniqueLabelLayout_ = new QVBoxLayout();
    uniqueLabelLayout_->setSpacing(5);
    uniqueLabelLayout_->setSizeConstraint(QLayout::SetFixedSize);
    mainLayout_ = new QVBoxLayout(this);
    mainLayout_->setAlignment(Qt::AlignTop);
    addLabel_ = new QLabel("add new Label");
    addLabelEdit_ = new QLineEdit();
    addButton_ = new QPushButton("Add Label");
    addLayout_ = new QHBoxLayout();
    addLayout_->addWidget(addLabelEdit_);
    addLayout_->addWidget(addButton_);;
    mainLayout_->addLayout(uniqueLabelLayout_);
    mainLayout_->addLayout(addLayout_);
    deleteButton_ = new QPushButton("delete current label");
    mainLayout_->addWidget(deleteButton_);
    connect(addButton_, &QPushButton::clicked, this, [this]{this->addLabel(addLabelEdit_->text());} );
    connect(deleteButton_, &QPushButton::clicked, this, [this]{this->deleteLabel();});
}


void LabelWidget::addLabel(QString labelName){
    if(labelName.isEmpty()){
        return;
    }
    if(uniqueLabels_.contains(labelName)){
        QMessageBox msgBox;
        msgBox.setText("label is exists");
        msgBox.setInformativeText("this label "+ labelName + " is exists");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        return;
    }

    QString color = getOneColor();
    addLabel(labelName, color);
}

void LabelWidget::addLabel(QString labelName, QString color){
    if(uniqueLabels_.contains(labelName)){
        return;
    }
    uniqueLabels_.append(labelName);
    QRadioButton *b = new QRadioButton(labelName);
    uniqueLabelColors_.append(color);
    b->setStyleSheet("color:"+color);
    uniqueLabelLayout_->addWidget(b, Qt::AlignRight);
    connect(b, SIGNAL(toggled(bool)), this, SLOT(changeCurrentLabel()));
    allButtons_.append(b);
    addLabelEdit_->setText("");
    update();
}


void LabelWidget::changeCurrentLabel(){
    QRadioButton *checkBtn = nullptr;
    int checkIndex = -1;
    for(int i=0; i<allButtons_.size() ;i++){
        if(allButtons_[i]->isChecked()){
            checkBtn = allButtons_[i];
            checkIndex = i;
            break;
        }
    }
    if(checkIndex<0){
        currentLabelIndex = -1;
        return;
    }
    currentLabelIndex = checkIndex;
    StateStorage::labelName = uniqueLabels_[currentLabelIndex];
    StateStorage::labelColor = uniqueLabelColors_[currentLabelIndex];
}

void LabelWidget::deleteLabel(){
    if(currentLabelIndex< 0){
        // todo 没选择任何label 不能删除
        return;
    }
    QMessageBox msgBox;
    msgBox.setText("Delete Label in Project.");
    msgBox.setInformativeText("Do you whant to delete this label in this project ? all the image in this project will delete this label");

    msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    if(ret == QMessageBox::Cancel){
        return;
    }
    uniqueLabels_.removeAt(currentLabelIndex);
    uniqueLabelColors_.removeAt(currentLabelIndex);
    allButtons_.removeAt(currentLabelIndex);

    QLayoutItem *labelItem = uniqueLabelLayout_->takeAt(currentLabelIndex);
    currentLabelIndex = -1;
    if(labelItem->widget()!=nullptr){
        delete labelItem->widget();
    }
    delete labelItem;
    StateStorage::labelName = "";
    StateStorage::labelColor="";
}

QString LabelWidget::getOneColor(){
    if(uniqueLabels_.size() > StateStorage::colorNames.size()){
        QStringList colors = QColor::colorNames();
        while(true){
            int indx = rand()% colors.size();
            QString name = colors[indx];
            if (!StateStorage::colorNames.contains(name) && !uniqueLabelColors_.contains(name)){
                return  name;
            }
        }
    }else{
        for(int i=0; i<StateStorage::colorNames.size(); i++){
            QString name = QColor(StateStorage::colorNames[i]).name();
            if(!uniqueLabelColors_.contains(name)){
                return name;
            }
        }
    }
    return "red";
}

bool LabelWidget::hasLabel(QString labelName){
    return uniqueLabels_.contains(labelName);
}
QString LabelWidget::getColor(QString labelName){
    if(uniqueLabels_.contains(labelName)){
        return uniqueLabelColors_[uniqueLabels_.indexOf(labelName)];
    }
    return "black";
}
