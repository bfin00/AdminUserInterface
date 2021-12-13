#include "mainwindow.h"
#include "admininterface.h"
#include "userinterface.h"
#include "usersdata.h"
#include <QPushButton>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QCoreApplication>
#include <QMenuBar>
#include <QLabel>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    _adminInterface = new AdminInterface();
    _userInterface = new UserInterface();
    _stackedWidget = new QStackedWidget(this);


    auto enterBut = new QPushButton(tr("Вход в систему"));
    enterBut->setMaximumSize(200, 40);
    auto groupbox = new QGroupBox(this);
    auto mainLayout = new QHBoxLayout;
    mainLayout->addWidget(enterBut);
    groupbox->setLayout(mainLayout);

    _stackedWidget->addWidget(groupbox);
    _stackedWidget->addWidget(_adminInterface);
    _stackedWidget->addWidget(_userInterface);
    _stackedWidget->setCurrentIndex(0);
    setCentralWidget(_stackedWidget);

    showFullScreen();
    auto fileMenu = menuBar()->addMenu(tr("Справка"));

    auto *info = new QAction(tr("О программе"), this);
    fileMenu->addAction(info);
    connect(info, &QAction::triggered, this, &MainWindow::InfoDialog);

    connect(enterBut, &QPushButton::clicked, this, &MainWindow::OpenEnterWindow);
    connect(_adminInterface, &AdminInterface::exitAdmin, this, &MainWindow::SwitchToMainWindow);
    connect(_userInterface, &UserInterface::exitUser, this, &MainWindow::SwitchToMainWindow);
}

MainWindow::~MainWindow() {}

void MainWindow::OpenEnterWindow()
{
    auto dialog = new QDialog(this);
    auto box = new QGroupBox(dialog);
    auto layout = new QVBoxLayout;
    box->setLayout(layout);
    _login = new QLineEdit;
    _password = new QLineEdit;
    _password->setEchoMode(QLineEdit::Password);
    _login->setMinimumSize(350, 50);
    _password->setMinimumSize(350, 50);
    auto enterBut = new QPushButton(tr("Войти"));
    layout->addWidget(_login);
    layout->addWidget(_password);
    layout->addWidget(enterBut);
    dialog->setMinimumSize(400, 400);

    connect(enterBut, &QPushButton::clicked, this, &MainWindow::CheckUserType);
    connect(this, &MainWindow::closeDialog, this, [dialog]() { dialog->close();});
    dialog->exec();
}

void MainWindow::CheckUserType()
{
    if (_checkValidEnter >= 2)
    {
        QMessageBox::warning(this, tr(""), tr("Пароль введен неверно больше 2-х раз"), QMessageBox::Close);
        QCoreApplication::quit();
    }
    if (_login->text() == "ADMIN" && _password->text() == (*_adminInterface->getUsersData()->getData())["ADMIN"].password)
    {
        _stackedWidget->setCurrentIndex(1);
        emit closeDialog();
    }
    else if (_adminInterface->getUsersData()->getData()->find(_login->text()) != _adminInterface->getUsersData()->getData()->end()
             && _password->text() == (*_adminInterface->getUsersData()->getData())[_login->text()].password
             )
    {
        if ((*_adminInterface->getUsersData()->getData())[_login->text()].isBanned == false)
        {
//            if ((*_adminInterface->getUsersData()->getData())[_login->text()].isRestricted == true)
//            {

//                emit closeDialog();
//            }
            _stackedWidget->setCurrentIndex(2);

            _userInterface->setData(_login->text(), (*_adminInterface->getUsersData()->getData())[_login->text()].password,
                                   (*_adminInterface->getUsersData()->getData())[_login->text()].isRestricted);

            connect(_userInterface, &UserInterface::newUserPW, this, &MainWindow::ChangeUserPW);
            if ((*_adminInterface->getUsersData()->getData())[_login->text()].isRestricted == true)
            {
                _userInterface->ChangePWDialog();
            }

            emit closeDialog();
        }

        else
            QMessageBox::warning(this, tr(""), tr("Пользователь забанен"), QMessageBox::Close);
    }
    else if ((*_adminInterface->getUsersData()->getData()).find(_login->text()) == (*_adminInterface->getUsersData()->getData()).end())
    {
        QMessageBox::warning(this, tr("Неверные данные"), tr("Неверный логин"), QMessageBox::Close);
        _password->clear();
        _login->clear();
    }
    else if (_password->text() != (*_adminInterface->getUsersData()->getData())[_login->text()].password)
    {
        ++_checkValidEnter;
        QMessageBox::warning(this, tr("Неверные данные"), tr("Неверный пароль"), QMessageBox::Close);
        _password->clear();
        _login->clear();
    }
    else
        QMessageBox::warning(this, tr("Неверные данные"), tr("Неверные данные"), QMessageBox::Close);
}

void MainWindow::SwitchToMainWindow()
{
    _stackedWidget->setCurrentIndex(0);
}

void MainWindow::ChangeUserPW(QString login, QString password)
{
    _adminInterface->setNewUserPW(login, password);
}

void MainWindow::InfoDialog()
{
    auto dialog = new QDialog(this);
    dialog->setMinimumSize(500, 500);
    auto box = new QGroupBox(dialog);
    auto layout = new QVBoxLayout;
    box->setLayout(layout);
    auto lbl = new QLabel(tr(""));
    auto lbl1 = new QLabel(tr("Вариант 14: Наличие строчных и прописных букв,"));
    auto lbl2 = new QLabel(tr("а также знаков арифметических операций"));
    layout->addWidget(lbl);
    layout->addWidget(lbl1);
    layout->addWidget(lbl2);
    dialog->exec();
}
