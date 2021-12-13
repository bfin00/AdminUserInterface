#include "admininterface.h"
#include "C_crc16.h"
#include <QtWidgets>
#include <QLineEdit>
#include <QTableWidget>
#include <wincrypt.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <QCryptographicHash>

AdminInterface::AdminInterface(QWidget* parent): QMainWindow(parent)
{
    encryptKey();
    encryptPhrase();

    _tempFileDirectory = QDir{qApp->applicationDirPath()}.filePath("users.txt");
    _cryptFileDirectory = QDir{qApp->applicationDirPath()}.filePath("crypt.txt");

    _data = new UsersData();
    setTableHeaders();
    setUpMenus();

    //прочитанный из файла текст
    QString text;

    if (checkFileEmptyness())
        SaveFile();
    else
    {
        if(!readCryptoFile(_cryptFileDirectory,text))
        {
            QMessageBox::warning(this, tr(""), tr("Файл поврежден"), QMessageBox::Close);
            exit(-1);
        }

        readFile(_tempFileDirectory);
        setTable();
    }


    _name = new QLineEdit();
    auto enterBut = new QPushButton(tr("Добавить пользователя"));

    enterBut->setMaximumSize(200, 40);
    auto groupbox = new QGroupBox(this);
    auto mainLayout = new QVBoxLayout;
    mainLayout->addWidget(_table);
    mainLayout->addWidget(_name);
    mainLayout->addWidget(enterBut);
    groupbox->setLayout(mainLayout);
    setCentralWidget(groupbox);
    setMinimumSize(1400, 1000);

    connect(enterBut, &QPushButton::clicked, this, &AdminInterface::AddUser);
}

void AdminInterface::setUpMenus()
{
    auto fileMenu = menuBar()->addMenu(tr("Файл"));

    auto *changePWAct = new QAction(tr("Изменить пароль"), this);
    fileMenu->addAction(changePWAct);
    connect(changePWAct, &QAction::triggered, this, &AdminInterface::ChangePWDialog);

    auto *saveFile = new QAction(tr("Сохранить"), this);
    fileMenu->addAction(saveFile);
    connect(saveFile, &QAction::triggered, this, &AdminInterface::SaveFile);

    auto *switchUser = new QAction(tr("Cменить пользователя"), this);
    fileMenu->addAction(switchUser);
    connect(switchUser, &QAction::triggered, this, &AdminInterface::Foo);

    auto *exitAction = fileMenu->addAction(tr("Выйти"), this, &AdminInterface::Exit);
    exitAction->setShortcuts(QKeySequence::Quit);
}

void AdminInterface::AddUser()
{
    bool check = _data->addUser(_name->text());
    if (check)
    {
        _checkBoxesBan.push_back(new QCheckBox(_table));
        _checkBoxesControl.push_back(new QCheckBox(_table));

        if ((*_data->getData())[_name->text()].isBanned)
            _checkBoxesBan.at(_checkBoxesBan.size() - 1)->setCheckState(Qt::Checked);
        else
            _checkBoxesBan.at(_checkBoxesBan.size() - 1)->setCheckState(Qt::Unchecked);

        if ((*_data->getData())[_name->text()].isRestricted)
            _checkBoxesControl.at(_checkBoxesControl.size() - 1)->setCheckState(Qt::Checked);
        else
            _checkBoxesControl.at(_checkBoxesControl.size() - 1)->setCheckState(Qt::Unchecked);


        addUserIntoTable(_name->text());
    }

}

void AdminInterface::AddUserWidget()
{

}

void AdminInterface::setTableHeaders()
{
    _table = new QTableWidget(0, 4);
    _table->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Имя полльзователя")));
    _table->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("Пароль")));
    _table->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Ограничения")));
    _table->setHorizontalHeaderItem(3, new QTableWidgetItem(tr("Бан")));
}

void AdminInterface::addUserIntoTable(QString name)
{
    _table->setRowCount(_table->rowCount() + 1);
    QVariant variant(name);
    auto item = new QTableWidgetItem();
    item->setData(Qt::DisplayRole, variant);
    _table->setItem(_table->rowCount() - 1, 0, item);

    QVariant variant2((*_data->getData())[name].password);
    auto item2 = new QTableWidgetItem();
    item2->setData(Qt::DisplayRole, variant2);
    _table->setItem(_table->rowCount() - 1, 1, item2);

    auto rowCount = _table->rowCount() - 1;
    _table->setCellWidget(rowCount, 3, _checkBoxesBan[rowCount]);
    _table->setCellWidget(rowCount, 2, _checkBoxesControl[rowCount]);


    auto ban = _checkBoxesBan[rowCount];
    auto control = _checkBoxesControl[rowCount];

    connect(ban, &QCheckBox::stateChanged, this, [ban, name, this](){
        (*this->_data->getData())[name].isBanned = ban->isChecked();
    });
    connect(control, &QCheckBox::stateChanged, this, [control, name, this](){
        (*this->_data->getData())[name].isRestricted = control->isChecked();
    });
}

void AdminInterface::ChangePWDialog()
{
    auto dialog = new QDialog(this);
    auto box = new QGroupBox(dialog);
    auto layout = new QVBoxLayout;
    box->setLayout(layout);
    auto lbl1 = new QLabel(tr("Введите старый пароль"));
    auto lbl2 = new QLabel(tr("Введите новый пароль"));
    auto lbl3 = new QLabel(tr("Подтвердите пароль"));
    box->setLayout(layout);
    _previousPW = new QLineEdit(dialog);
    _previousPW->setEchoMode(QLineEdit::Password);
    _newPW = new QLineEdit(dialog);
    _newPW->setEchoMode(QLineEdit::Password);
    _newPW2 = new QLineEdit(dialog);
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

    connect(changeBut, &QPushButton::clicked, this, &AdminInterface::ChangePW);

    dialog->exec();

}

bool AdminInterface::SaveFile()
{
    return saveIntoFile(_tempFileDirectory);
}

bool AdminInterface::saveIntoFile(const QString& fileName)
{
    QString errorMessage;

        QGuiApplication::setOverrideCursor(Qt::WaitCursor);
        QSaveFile file(fileName);
        if (file.open(QFile::WriteOnly | QFile::Text))
        {
                        QTextStream out(&file);
                        QString str;  // В str будет храниться строка, которую буду шифровать
                        out << tr("Имя пользователя  |  Пароль  |  Ограничения  |  Бан") << Qt::endl;
                        str.append(tr("Имя пользователя  |  Пароль  |  Ограничения  |  Бан\n"));
                        writeToFile(out, str);

                        // crypto

                        _saveQByteStr = str.toUtf8();
                        QByteArray array = _saveQByteStr;

                        QByteArray output;

                        for (int i = 0; i < array.size(); ++i)
                        {
                            output[i] = _saveQByteStr[i] ^ _encryptedKey[i % _encryptedKey.size()];
                        }

                        writeIntoCryptoFile(output);

                        // фрагмент для проверки правильности дешифрования
                        QByteArray decrypt;
                        for (int i = 0; i < array.size(); ++i)
                        {
                            decrypt[i] = output[i] ^ _encryptedKey[i % _encryptedKey.size()];
                        }

                        string ss = decrypt.toStdString();
                        //


                        //crypto

            if (!file.commit())
            {
                errorMessage = tr("Cannot write file %1:\n%2.")
                               .arg(QDir::toNativeSeparators(fileName), file.errorString());
            }
        }
        else
        {
            errorMessage = tr("Cannot open file %1 for writing:\n%2.")
                           .arg(QDir::toNativeSeparators(fileName), file.errorString());
        }
        QGuiApplication::restoreOverrideCursor();

        if (!errorMessage.isEmpty())
        {
            QMessageBox::warning(this, tr("Application"), errorMessage);
            return false;
        }

        return true;
}


void AdminInterface::Foo()
{
    emit exitAdmin();
}
void AdminInterface::writeToFile(QTextStream &out, QString& str)
{
    for (const auto& user: qAsConst(*_data->getData()))
    {
        out << user.login << tr("  |  ");
        str.append(user.login);
        str.append("  |  ");

        str.append(tr("  |  "));

        if (user.password == "")
        {
            out << tr("пароль отсутствует") << tr("  |  ");
            str.append(tr("пароль отсутствует  |  "));
        }
        else
        {
            out << user.password << tr("  |  ");
            str.append(user.password);
            str.append(tr("  |  "));
        }

        if (user.isRestricted == false)
        {
            out << tr("нет") << "  |  ";
            str.append(tr("нет  |  "));
        }
        else
        {
            out << tr("да") << "  |  ";
            str.append(tr("да  |  "));
        }

        if (user.isBanned == false)
        {
            out << tr("нет") << Qt::endl;
            str.append(tr("нет \n"));
        }
        else
        {
            out << tr("да") << Qt::endl;
            str.append(tr("да \n"));
        }
    }
};

void AdminInterface::ChangePW()
{
    if ((*_data->getData())["ADMIN"].password == _previousPW->text())
    {
        if (_newPW->text() == _newPW2->text())
        {
            _data->changePW("ADMIN", _newPW->text());
            QMessageBox::information(this, tr("Изменение пароля"), tr("Пароль успешно изменен"), QMessageBox::Close);
        }
        else
            QMessageBox::warning(this, tr("Неверные данные"), tr("Новые пароли не совпадают"), QMessageBox::Close);
    }
    else
        QMessageBox::warning(this, tr("Неверные данные"), tr("Старый пароль не совпадает"), QMessageBox::Close);
}

UsersData* AdminInterface::getUsersData()
{
    return _data;
}


bool AdminInterface::checkFileEmptyness()
{
    QFile file(_tempFileDirectory);
    file.open( QIODevice::WriteOnly|QIODevice::Append );
    if (file.pos() == 0)
        return true;
    else
        return false;
}

bool AdminInterface::readFile(const QString& fileName)
{
    //QVector<QString> strsFromFile;
    QMap<QString, User> map;
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return false;

    QTextStream in(&file);
    in.readLine();
    while (!in.atEnd())
    {
        QString str = in.readLine();

        if (str == "")
            break;

        QStringList strL = str.split("  |  ");

        if (strL.at(1) == tr("пароль отсутствует"))
            map[strL.at(0)].password = "";
        else
            map[strL.at(0)].password = strL.at(1);

        if (strL.at(2) == tr("нет"))
            map[strL.at(0)].isRestricted = false;
        else
            map[strL.at(0)].isRestricted = true;

        if (strL.at(3) == tr("нет"))
            map[strL.at(0)].isBanned = false;
        else
            map[strL.at(0)].isBanned = true;

        map[strL.at(0)].login = strL.at(0);
    }

    _data->setData(map);
    return true;
}

void AdminInterface::setNewUserPW(QString login, QString password)
{
    _data->setNewUserPW(login, password);
    SaveFile();
    updateTable();
}

void AdminInterface::updateTable()
{
    _table->clearContents();
    for (auto i = _table->rowCount() - 1; i >= 0; --i)
    {
        _table->removeRow(i);
    }

    setTable();

}

void AdminInterface::setTable()
{
    _checkBoxesBan.clear();
    _checkBoxesControl.clear();
    for (const auto& item: qAsConst(*_data->getData()))
    {
        if (item.login != "ADMIN")
        {
            _checkBoxesBan.push_back(new QCheckBox(_table));
            _checkBoxesControl.push_back(new QCheckBox(_table));

            if (item.isBanned)
                _checkBoxesBan.at(_checkBoxesBan.size() - 1)->setCheckState(Qt::Checked);
            else
                _checkBoxesBan.at(_checkBoxesBan.size() - 1)->setCheckState(Qt::Unchecked);

            if (item.isRestricted)
                _checkBoxesControl.at(_checkBoxesControl.size() - 1)->setCheckState(Qt::Checked);
            else
                _checkBoxesControl.at(_checkBoxesControl.size() - 1)->setCheckState(Qt::Unchecked);

            addUserIntoTable(item.login);
        }
    }
}


void AdminInterface::Exit()
{
    SaveFile();
    QCoreApplication::quit();
}



void AdminInterface::encryptKey()
{
    QByteArray key = "000";
    _encryptedKey = QCryptographicHash::hash(key, QCryptographicHash::Md5);

}

struct CCodec
{
	static uint8_t GetXorKey()
	{
		return 0x55;
	}

	static QByteArray EncodeString(const QString text)
	{
		QByteArray closedData;

		const char* beg =(const char*)text.data();
		const int   size=text.size()*sizeof(*text.data());
		closedData=CCodec::EncodeArray({beg,size});

		return closedData;
	}

	static bool DecodeString(const QByteArray closedData,QString& text)
	{
		text.clear();

		//decrypt
		const auto openedData_result=DecodeArray(closedData);
		if(!openedData_result.second)
		{
			return false;
		}

		const auto& openedData=openedData_result.first;
		if(openedData.size()%sizeof(*text.data()))
		{
			return false;
		}

		{
			const int size=openedData.size()/sizeof(*text.data());
			text.resize(size,QChar('\0'));
		}

		char* beg =(char*)text.data();
		const int size=text.size()*sizeof(*text.data());
		if(size!=openedData.size())
		{
			return false;
		}
		std::copy(openedData.begin(),openedData.end(),beg);

		return true;
	}

	static QByteArray EncodeArray(const QByteArray openedData)
	{
		QByteArray closedData;

		//контрольная сумма до шифрования
		const uint16_t crc= C_crc16::st_MakeCRC16_singleCalc(openedData);

		//шифруем
		closedData.reserve(openedData.size()+sizeof(crc));
		closedData=openedData;
		for(int i=0; i<closedData.size(); i++)
		{
			uint8_t by=(uint8_t)closedData[i];
			by^=(i<<4);
			by^=GetXorKey();
			closedData[i]=(char)by;
		}

		//в конце дописываем crc
		closedData.push_back((crc>>0)&0xff);//младший байт
		closedData.push_back((crc>>8)&0xff);//старший байт

		return closedData;
	}

	static QPair<QByteArray,bool> DecodeArray(const QByteArray closedData)
	{
		QPair<QByteArray,bool> openedData_result{};

		auto& openedData=openedData_result.first;
		openedData=closedData;

		//достаём контрольную сумму до шифрования
		uint16_t crc{};
		if(openedData.size()<(int)sizeof(crc))
		{
			return {};
		}

		uint8_t by0=(uint8_t)*(openedData.rbegin()+1);
		uint8_t by1=(uint8_t)*(openedData.rbegin()+0);
		crc=(by0 | (by1<<8));
		openedData=openedData.chopped(sizeof(crc));

		//расшифровываем
		for(int i=0; i<openedData.size(); i++)
		{
			uint8_t by=(uint8_t)openedData[i];
			by^=(i<<4);
			by^=GetXorKey();
			openedData[i]=(char)by;
		}

		//проверка контрольной суммы
		const uint16_t crc_test= C_crc16::st_MakeCRC16_singleCalc(openedData);
		if(crc!=crc_test)
		{
			return {};
		}

		openedData_result.second=true;
		return openedData_result;
	}
};

bool AdminInterface::writeIntoCryptoFile(QString text)
{
	const QByteArray closedData=CCodec::EncodeString(text);

    QString errorMessage;

        QGuiApplication::setOverrideCursor(Qt::WaitCursor);
        QFile file(_cryptFileDirectory);
        if (file.open(QFile::WriteOnly))
        {
		  file.write(closedData);
        }
        else
        {
            errorMessage = tr("Cannot open file %1 for writing:\n%2.")
                           .arg(QDir::toNativeSeparators(_cryptFileDirectory), file.errorString());
        }
        QGuiApplication::restoreOverrideCursor();

        if (!errorMessage.isEmpty())
        {
            QMessageBox::warning(this, tr("Application"), errorMessage);
            return false;
        }

        return true;
}

bool AdminInterface::readCryptoFile(const QString& fileName,QString& text)
{
	text.clear();
	//QMap<QString, User> map;

	{
		QFile file(fileName);

		if (!file.open(QIODevice::ReadOnly))
		{
			return false;
		}

		if(!CCodec::DecodeString(file.readAll(),text))
		{
			return false;
		}
	}

    return true;
}

void AdminInterface::encryptPhrase()
{
    _pwPhrase = "qweasd";
    QByteArray hashArray = _pwPhrase.toUtf8();
    QCryptographicHash* hash = new QCryptographicHash(QCryptographicHash::Md5);
    hash->addData(hashArray);
    QString pass = hash->result().toBase64();

}
