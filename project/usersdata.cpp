#include "usersdata.h"
#include <QMessageBox>
#define ADMIN "ADMIN"
UsersData::UsersData()
{
    _usersPW[ADMIN] = {ADMIN, "", false, false};
}


bool UsersData::addUser(const QString& name)
{
    if (_usersPW.find(name) == _usersPW.end())
    {
        _usersPW[name] = {name, "", false, false};
        return true;
    }
    else
    {
        QMessageBox::warning(this, tr("Неверный ввод"), tr("Пользователь с таким именем существует"), QMessageBox::Close);
        return false;
    }
}
void UsersData::changePW(const QString& login, const QString& newPW)
{
    _usersPW[login].password = newPW;
}
void UsersData::banUser() {}
void UsersData::Update() {}

QMap<QString, User>* UsersData::getData()
{
    return &_usersPW;
}

void UsersData::setData(const QMap<QString, User>& map)
{
    _usersPW.clear();
    _usersPW = map;
}

void UsersData::setNewUserPW(QString login, QString password)
{
    _usersPW[login].password = password;
}

void UsersData::clear()
{
    _usersPW.clear();
}
