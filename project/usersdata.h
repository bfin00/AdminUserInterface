#ifndef USERSDATA_H
#define USERSDATA_H

#include <QMainWindow>
#include <QMap>
#include <QFile>
struct User
{
    QString login;
    QString password;
    bool isRestricted;
    bool isBanned;
};

class UsersData: public QMainWindow
{
    Q_OBJECT

public:
    UsersData();
    bool addUser(const QString& name);
    void changePW(const QString& login, const QString& newPW);
    void banUser();
    QMap<QString, User>* getData();
    void setData(const QMap<QString, User>& map);
    void setNewUserPW(QString login, QString password);
    void clear();

private slots:
    void Update();

private:
    QMap<QString, User> _usersPW;
};




#endif // USERSDATA_H
