#include "idatabase.h"
#include<QUuid>

void IDatabase::ininDatabase()
{
    database = QSqlDatabase::addDatabase("QSQLITE"); //添加 SQL LITE数据库驱动
    QString aFile = "D:/QTLab/Lab3a.db";
    database.setDatabaseName(aFile); //设置数据库名称

    if (!database.open()) { //打开数据库
        qDebug() << "failed to open database";
    } else
        qDebug() << "open database is ok";
}

bool IDatabase::initPatientModel()
{
    patientTabModel = new QSqlTableModel(this, database);
    patientTabModel->setTable("patient");
    patientTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    patientTabModel->setSort(patientTabModel->fieldIndex("name"), Qt::AscendingOrder); //排序
    if (!(patientTabModel->select()))//查询数据
        return false;

    thePatientSelection = new QItemSelectionModel(patientTabModel);
    return true;
}

int IDatabase::addNewPatient()
{
    patientTabModel->insertRow(patientTabModel->rowCount(),
                               QModelIndex()); //在末尾添加一个记录
    QModelIndex curIndex = patientTabModel->index(patientTabModel->rowCount() - 1,
                                                  1); //创建最后一行的ModelIndex
int curRecNo = curIndex.row();
QSqlRecord curRec = patientTabModel->record(curRecNo); //获取当前记录
curRec.setValue("CREATEDTIMESTAMP", QDateTime::currentDateTime().toString("yyyy-MM-dd"));
curRec.setValue("ID", QUuid::createUuid().toString(QUuid::WithoutBraces));
patientTabModel->setRecord(curRecNo, curRec);

return curIndex.row();

    return curIndex.row();
}
bool IDatabase::searvhPatient(QString filter)
{
    patientTabModel->setFilter(filter);
    return patientTabModel->select();
}

bool IDatabase::deleteCurrentPatient()
{
    QModelIndex curIndex = thePatientSelection->currentIndex();
    // 先判断索引是否有效（避免无效行操作）
    if (!curIndex.isValid()) {
        return false;
    }
    // 尝试删除行
    if (!patientTabModel->removeRow(curIndex.row())) {
        return false;
    }
    // 提交修改（返回提交结果）
    bool submitOk = patientTabModel->submitAll();
    // 重新查询数据（刷新模型）
    patientTabModel->select();
    return submitOk;
}

bool IDatabase::submitPatientEdit()
{
    return patientTabModel->submitAll();
}

void IDatabase::revertPatientEdit()
{
    patientTabModel->revertAll();
}


    QString IDatabase::userLogin(QString userName, QString password)
    {
      //  return "loginOK";
          QSqlQuery query; //查询出当前记录的所有字段
        query.prepare("select username,password from user where username = :USER");
        query.bindValue(":USER", userName);
        query.exec();
        if (query.first() && query.value("username").isValid()) {
            QString paswd = query.value("password").toString();
            if (paswd == password) {
                return "loginOk";
            } else {
                return "wrongPassword";
            }
        } else {
            qDebug() << "no such user";
            return "wrongUsername";
        }
    }


IDatabase::IDatabase(QObject *parent) : QObject(parent)
{
    ininDatabase();
}
