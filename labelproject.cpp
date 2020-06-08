#include "labelproject.h"

#include <QDebug>
#include <QSqlQuery>
#include <QFileInfo>
#include <QMessageBox>
#include <QSqlRecord>

LabelProject::LabelProject(QObject *parent) : QObject(parent)
{
}



LabelProject::~LabelProject()
{
    /*!
     * Destructor, closes the database if it's open.
     */

//    if(db.isOpen()){
//        qDebug() << "Closing database.";
//        db.close();
//        QSqlDatabase::removeDatabase(connection_name);
//    }
//    emit finished();
}

bool LabelProject::createDatabase(QString fileName){
    bool res;

//    loadDatabase(fileName);
    {
        QSqlQuery query(db_);
        /*
         * The label database is a simple relational db - image locations, classes
         * and label bounding boxes.
         */
        res = query.exec("CREATE table images (image_id INTEGER PRIMARY KEY ASC, "
                   "path varchar(256))");
        res &= query.exec("CREATE table classes (class_id INTEGER PRIMARY KEY ASC, "
                   "name varchar(32))");
        res &= query.exec("CREATE table labels (label_id INTEGER PRIMARY KEY ASC, "
                   "image_id int, class_id int, x int, y int, width int, height int, automatic int)");
        if(!res){
            qDebug() << "query Error: " ;
        }
    }
    return checkDatabase();

}
bool LabelProject::loadDatabase(QString fileName){
    if(db_.isOpen()){
        db_.close();
    }
    connection_name_ = fileName;
    db_ = QSqlDatabase::addDatabase("QSQLITE", connection_name_);
    db_.setDatabaseName(fileName);

    QFileInfo check_file(fileName);

    if (!db_.open()) {
        QMessageBox::critical(nullptr, QObject::tr("Cannot open database"),
            QObject::tr("Unable to establish a database connection.\n"
                        "This example needs SQLite support. Please read "
                        "the Qt SQL driver documentation for information how "
                        "to build it.\n\n"
                        "Click Cancel to exit."), QMessageBox::Cancel);
        return false;
    }
    else if ( check_file.exists() && check_file.isFile() ){
        return checkDatabase();
    }else{
        return true;
    }
}


bool LabelProject::checkDatabase(){

    /*!
     * Check that the newly loaded database contains the correct tables/fields. Returns
     * true if the database is valid, false if not.
     */

    QSqlRecord image_record = db_.record("images");
    QSqlRecord label_record = db_.record("labels");
    QSqlRecord class_record = db_.record("classes");

    if(image_record.isEmpty()) return false;
    if(label_record.isEmpty()) return false;
    if(class_record.isEmpty()) return false;

    if(!image_record.contains("image_id")) return false;
    if(!image_record.contains("path")) return false;

    if(!label_record.contains("image_id")) return false;
    if(!label_record.contains("label_id")) return false;
    if(!label_record.contains("class_id")) return false;
    if(!label_record.contains("x")) return false;
    if(!label_record.contains("y")) return false;
    if(!label_record.contains("width")) return false;
    if(!label_record.contains("height")) return false;

    if(!class_record.contains("class_id")) return false;
    if(!class_record.contains("name")) return false;

    qDebug() << "Database structure looks good";

    return true;
}
