#ifndef USERINTERFACE_H
#define USERINTERFACE_H
#include <QMainWindow>
#include <QStackedWidget>
#include <QWidget>

class QLineEdit;

class UserInterface: public QMainWindow
{
    Q_OBJECT

public:
    UserInterface(QWidget* parent = nullptr);
    ~UserInterface() = default;

    void setData(QString login, QString password, bool isRestricted);

signals:
    void exitUser();
    void newUserPW(QString login, QString password);

public slots:
     void ChangePWDialog();
private slots:   
    void ChangePW();
    void Foo();

private:
    void setUpMenus();
    bool isUnderRestriction(const std::string& password);

    QString _login;
    QString _password;
    QLineEdit* _previousPW = nullptr;
    QLineEdit* _newPW = nullptr;
    QLineEdit* _newPW2 = nullptr;
    bool _isRestricted;

};

#endif // USERINTERFACE_H
