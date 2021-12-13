#ifndef ADMININTERFACE_H
#define ADMININTERFACE_H
#include <QMainWindow>
#include <QStackedWidget>
#include "usersdata.h"
#include <QWidget>
#include <QCheckBox>
#include <QVector>
#include<QTextStream>


using namespace std;
class QLineEdit;
class QTableWidget;
class QGroupBox;
class QVBoxLayout;
class QPushButton;


class AdminInterface: public QMainWindow
{
    Q_OBJECT


public:
    AdminInterface(QWidget* parent = nullptr);
    ~AdminInterface() = default;

    void setUpMenus();
    void setTableHeaders();
    void addUserIntoTable(QString name);
    UsersData* getUsersData();
    void setNewUserPW(QString login, QString password);

signals:
    void exitAdmin();

private slots:
    void AddUserWidget();
    void ChangePW();
    void AddUser();
    void ChangePWDialog();
    bool SaveFile();
    void Foo();
    void Exit();

private:
    QByteArray _saveQByteStr;
    QLineEdit* _name = nullptr;
    QTableWidget* _table = nullptr;
    QVector<QCheckBox*> _checkBoxesBan;
    QVector<QCheckBox*> _checkBoxesControl;
    QLineEdit* _previousPW = nullptr;
    QLineEdit* _newPW = nullptr;
    QLineEdit* _newPW2 = nullptr;
    UsersData* _data;
    QString _tempFileDirectory;
    QString _cryptFileDirectory;
    QByteArray _encryptedKey;

    bool saveIntoFile(const QString& fileName);
    void writeToFile(QTextStream &out, QString& str);
    bool checkFileEmptyness();
    bool readFile(const QString& fileName);
    void setTableValues();
    void updateTable();
    void setTable();
    //void saveIntoCryptFile();
    void encryptKey();
    void encryptPhrase();
    //std::string readTempFileIntoStr(const std::string& fileName);

    bool writeIntoCryptoFile(QString text);
    bool readCryptoFile(const QString& fileName,QString& text);

    int strCryptSize;
    QString _pwPhrase;
};

#endif // ADMININTERFACE_H
