 #ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMap>
#include <QMainWindow>
#include <QStackedWidget>
class AdminInterface;
class UserInterface;
class UsersData;
class QLineEdit;

struct User;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void closeDialog();

private:
    AdminInterface* _adminInterface;
    UserInterface* _userInterface;

    QLineEdit* _login;
    QLineEdit* _password;
    QStackedWidget* _stackedWidget;

    int _checkValidEnter = 0;

private slots:
    void OpenEnterWindow();
    void CheckUserType();
    void SwitchToMainWindow();
    void ChangeUserPW(QString login, QString password);
    void InfoDialog();


};
#endif // MAINWINDOW_H
