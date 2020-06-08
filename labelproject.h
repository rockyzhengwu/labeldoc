#ifndef LABELPROJECT_H
#define LABELPROJECT_H

#include <QObject>
#include <QSqlDatabase>
#include <QString>


class LabelProject: public QObject
{
public:
    explicit LabelProject(QObject *parent = nullptr);
    ~LabelProject();

    bool createDatabase(QString fileName);
    bool loadDatabase(QString fileName);


private:
    QSqlDatabase db_;
    QString connection_name_;

    bool checkDatabase();
};

#endif // LABELPROJECT_H
