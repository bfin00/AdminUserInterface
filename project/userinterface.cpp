#include "userinterface.h"
#include <QtWidgets>
#include <QLineEdit>
#include <QLabel>
#include <set>


UserInterface::UserInterface(QWidget* parent): QMainWindow(parent)
{
    setUpMenus();
}

void UserInterface::setUpMenus()
{
    auto fileMenu = menuBar()->addMenu(tr("Файл"));

    auto *changePWAct = new QAction(tr("Изменить пароль"), this);
    fileMenu->addAction(changePWAct);
    connect(changePWAct, &QAction::triggered, this, &UserInterface::ChangePWDialog);


    auto *switchUser = new QAction(tr("Сменить пользователя"), this);
    fileMenu->addAction(switchUser);
    connect(switchUser, &QAction::triggered, this, &UserInterface::Foo);

    auto *exitAction = fileMenu->addAction(tr("Выйти"), qApp, &QCoreApplication::quit);
    exitAction->setShortcuts(QKeySequence::Quit);
}

void UserInterface::Foo() {emit exitUser();}

void UserInterface::ChangePWDialog()
{
    auto dialog = new QDialog(this);
    auto box = new QGroupBox(dialog);
    auto layout = new QVBoxLayout;
    auto lbl1 = new QLabel(tr("Введите старый пароль"));
    auto lbl2 = new QLabel(tr("Введите новый пароль"));
    auto lbl3 = new QLabel(tr("Подтвердите пароль"));
    box->setLayout(layout);
    _previousPW = new QLineEdit(dialog);
    _newPW = new QLineEdit(dialog);
    _newPW2 = new QLineEdit(dialog);
    _previousPW->setEchoMode(QLineEdit::Password);
    _newPW->setEchoMode(QLineEdit::Password);
    _newPW2->setEchoMode(QLineEdit::Password);
    _previousPW->setMinimumSize(350, 50);
    _newPW->setMinimumSize(350, 50);
    auto changeBut = new QPushButton(tr("Изменить"));
    layout->addWidget(lbl1);
    layout->addWidget(_previousPW);
    layout->addWidget(lbl2);
    layout->addWidget(_newPW);
    layout->addWidget(lbl3);
    layout->addWidget(_newPW2);
    layout->addWidget(changeBut);
    dialog->setMinimumSize(400, 400);

    connect(changeBut, &QPushButton::clicked, this, &UserInterface::ChangePW);

    dialog->exec();

}

void UserInterface::ChangePW()
{
    if (_password == _previousPW->text())
    {
        if (_newPW->text() == _newPW2->text())
        {
            if (_isRestricted == true)
            {
                if (isUnderRestriction(_newPW->text().toStdString()))
                {
                    emit newUserPW(_login, _newPW->text());
                    QMessageBox::information(this, tr("Изменение пароля"), tr("Пароль успешно изменен"), QMessageBox::Close);
                }
                else
                    QMessageBox::warning(this, tr("Неверные данные"), tr("Пароль должен содержать прописные, строчные буквы и знаки арифметических действий"), QMessageBox::Close);
            }
            else
            {
                emit newUserPW(_login, _newPW->text());
                QMessageBox::information(this, tr("Изменение пароля"), tr("Пароль успешно изменен"), QMessageBox::Close);
            }
        }
        else
            QMessageBox::warning(this, tr("Неверные данные"), tr("Новые пароли не совпадают"), QMessageBox::Close);
    }
    else
        QMessageBox::warning(this, tr("Неверные данные"), tr("Старый пароль не совпадает"), QMessageBox::Close);


}

void UserInterface::setData(QString login, QString password, bool isRestricted)
{
    _login = login;
    _password = password;
    _isRestricted = isRestricted;
}

bool UserInterface::isUnderRestriction(const std::string& password)
{
    const std::set<char> arithmeticOperators{'+', '-', '!', '*', '\\', '%', '^'};

    bool anyLowercase = std::any_of(password.cbegin(), password.cend(), [](char ch) { return islower(ch); });

    bool anyUppercase = std::any_of(password.cbegin(), password.cend(), [](char ch) { return isupper(ch); });

    bool anyArithmetic = std::any_of(password.cbegin(), password.cend(), [&arithmeticOperators](char ch) {
        return arithmeticOperators.count(ch);
    });

    return anyLowercase && anyUppercase && anyArithmetic;
}


