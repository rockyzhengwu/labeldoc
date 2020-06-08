#ifndef LABELWIDGET_H
#define LABELWIDGET_H


#include <QWidget>
#include <QGroupBox>
#include <QDockWidget>
#include <QGridLayout>
#include <QLabel>
#include <QProgressBar>
#include <QVector>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QString>
#include <QListWidget>
#include <QRadioButton>

class LabelWidget : public QWidget
{
        Q_OBJECT


public:
    explicit LabelWidget(QWidget *parent=nullptr);
//    QString getCurrentLabel();
    void addLabel(QString labelName, QString color);

signals:

protected slots:
    void addLabel(QString labelName );
    void deleteLabel();
    void changeCurrentLabel();

private:
    QVBoxLayout *mainLayout_;
    QVector<QString> uniqueLabels_;
    QVector<QString> uniqueLabelColors_;
    QVBoxLayout *uniqueLabelLayout_;

    QVector<QRadioButton*> allButtons_;
    QHBoxLayout *addLayout_;
    QLabel *addLabel_;
    QLineEdit *addLabelEdit_ ;
    QPushButton *addButton_;
    QPushButton *deleteButton_;

    int currentLabelIndex = -1;
    void createUI();
    QString getOneColor();

};

#endif // LABELWIDGET_H
