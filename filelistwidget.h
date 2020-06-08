#ifndef FILELISTWIDGET_H
#define FILELISTWIDGET_H

#include <QWidget>
class FileListWidget:public QWidget
{
public:
    explicit FileListWidget(QWidget *parent=nullptr);

private:
    QStringList images_;

};

#endif // FILELISTWIDGET_H
