#include "storagedialog.h"
#include <QLabel>
#include <QMessageBox>
#include <QFileDialog>
#include <QIntValidator>
#include <iostream>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDateTime>

StorageDialog::StorageDialog(QWidget *parent)
    : QDialog(parent) {
    setupUI();
    setWindowTitle("Select Storage Type");
    resize(500, 400);
}

StorageDialog::~StorageDialog() {}

void StorageDialog::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Заголовок
    QLabel *titleLabel = new QLabel("Select Storage Type for Phone Book", this);
    titleLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    mainLayout->addWidget(titleLabel);
    
    // Радио кнопки для выбора типа хранилища
    fileRadio = new QRadioButton("File Storage (data/contacts.txt)", this);
    databaseRadio = new QRadioButton("PostgreSQL Database", this);
    fileRadio->setChecked(true);
    
    mainLayout->addWidget(fileRadio);
    mainLayout->addWidget(databaseRadio);
    
    // Группа для файлового хранилища
    fileGroup = new QGroupBox("File Storage Settings", this);
    QFormLayout *fileLayout = new QFormLayout(fileGroup);
    
    filePathEdit = new QLineEdit(this);
    filePathEdit->setText("data/contacts.txt");
    
    QPushButton *browseButton = new QPushButton("Browse...", this);
    QHBoxLayout *pathLayout = new QHBoxLayout();
    pathLayout->addWidget(filePathEdit);
    pathLayout->addWidget(browseButton);
    
    fileLayout->addRow("File Path:", pathLayout);
    fileGroup->setEnabled(true);
    
    mainLayout->addWidget(fileGroup);
    
    // Группа для базы данных
    databaseGroup = new QGroupBox("Database Connection Settings", this);
    QFormLayout *dbLayout = new QFormLayout(databaseGroup);
    
    hostEdit = new QLineEdit(this);
    hostEdit->setText("localhost");
    
    portEdit = new QLineEdit(this);
    portEdit->setText("5432");
    portEdit->setValidator(new QIntValidator(1, 65535, this));
    
    dbNameEdit = new QLineEdit(this);
    dbNameEdit->setText("phonebook");
    
    userEdit = new QLineEdit(this);
    userEdit->setText("phonebook_user");
    
    passwordEdit = new QLineEdit(this);
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setPlaceholderText("Enter password");
    passwordEdit->setText("phonebook_password");
    
    dbLayout->addRow("Host:", hostEdit);
    dbLayout->addRow("Port:", portEdit);
    dbLayout->addRow("Database Name:", dbNameEdit);
    dbLayout->addRow("Username:", userEdit);
    dbLayout->addRow("Password:", passwordEdit);
    
    databaseGroup->setEnabled(false);
    mainLayout->addWidget(databaseGroup);
    
    // Кнопка тестирования соединения
    testButton = new QPushButton("Test Connection", this);
    testButton->setEnabled(false);
    mainLayout->addWidget(testButton);
    
    // Кнопки OK/Cancel
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    okButton = new QPushButton("OK", this);
    cancelButton = new QPushButton("Cancel", this);
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Подключаем сигналы
    connect(fileRadio, &QRadioButton::toggled, this, &StorageDialog::onStorageTypeChanged);
    connect(browseButton, &QPushButton::clicked, [this]() {
        QString filename = QFileDialog::getSaveFileName(this, "Select Contacts File",
                                                       "data/contacts.txt",
                                                       "Text files (*.txt);;All files (*.*)");
        if (!filename.isEmpty()) {
            filePathEdit->setText(filename);
        }
    });
    
    connect(testButton, &QPushButton::clicked, this, &StorageDialog::onTestConnection);
    connect(okButton, &QPushButton::clicked, this, &StorageDialog::onAccept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void StorageDialog::onStorageTypeChanged() {
    bool isFileStorage = fileRadio->isChecked();
    fileGroup->setEnabled(isFileStorage);
    databaseGroup->setEnabled(!isFileStorage);
    testButton->setEnabled(!isFileStorage);
}

void StorageDialog::onTestConnection() {
    QString host = hostEdit->text().trimmed();
    QString port = portEdit->text().trimmed();
    QString dbName = dbNameEdit->text().trimmed();
    QString user = userEdit->text().trimmed();
    QString password = passwordEdit->text().trimmed();
    
    std::cout << "\n=== Testing Database Connection ===" << std::endl;
    std::cout << "Host: " << host.toStdString() << ":" << port.toStdString() << std::endl;
    std::cout << "Database: " << dbName.toStdString() << std::endl;
    std::cout << "User: " << user.toStdString() << std::endl;
    
    // Создаем тестовое подключение
    QString connectionName = "test_connection_" + QString::number(QDateTime::currentMSecsSinceEpoch());
    
    std::cout << "Creating database connection..." << std::endl;
    
    // Проверяем доступные драйверы
    QStringList drivers = QSqlDatabase::drivers();
    std::cout << "Available SQL drivers:" << std::endl;
    for (const QString& driver : drivers) {
        std::cout << "  - " << driver.toStdString() << std::endl;
    }
    
    if (!drivers.contains("QPSQL")) {
        QMessageBox::critical(this, "Driver Not Found", 
                            "PostgreSQL driver (QPSQL) not found!\n\n"
                            "Install it with: brew install qt6-postgresql");
        return;
    }
    
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL", connectionName);
    db.setHostName(host);
    db.setPort(port.toInt());
    db.setDatabaseName(dbName);
    db.setUserName(user);
    db.setPassword(password);
    db.setConnectOptions("connect_timeout=5");
    
    std::cout << "Opening connection..." << std::endl;
    
    if (db.open()) {
        std::cout << "✅ Connection successful!" << std::endl;
        
        // Простой тест без сложных запросов
        QSqlQuery query(db);
        if (query.exec("SELECT 1")) {
            std::cout << "✅ Test query executed successfully" << std::endl;
        }
        
        db.close();
        QMessageBox::information(this, "Connection Test", 
                               "✅ Connection successful!\n\n"
                               "Database: " + dbName + "\n"
                               "Host: " + host + ":" + port);
    } else {
        QSqlError error = db.lastError();
        std::cout << "❌ Connection failed!" << std::endl;
        std::cout << "Error: " << error.text().toStdString() << std::endl;
        
        QString errorMsg = "❌ Connection failed!\n\n"
                          "Error: " + error.text() + "\n\n"
                          "Please check:\n"
                          "1. Is PostgreSQL running? (brew services start postgresql@15)\n"
                          "2. Is database created? (createdb phonebook)\n"
                          "3. Try: psql -h localhost -p 5432 -U " + user + " -d " + dbName;
        
        QMessageBox::critical(this, "Connection Test Failed", errorMsg);
    }
    
    QSqlDatabase::removeDatabase(connectionName);
}

void StorageDialog::onAccept() {
    accept();
}

std::string StorageDialog::getSelectedStorage() const {
    if (fileRadio->isChecked()) {
        return "file";
    } else {
        return "database";
    }
}

std::string StorageDialog::getConnectionString() const {
    if (fileRadio->isChecked()) {
        return filePathEdit->text().toStdString();
    } else {
        // Формат: host:port:dbname:user:password
        std::string connection = 
            hostEdit->text().trimmed().toStdString() + ":" +
            portEdit->text().trimmed().toStdString() + ":" +
            dbNameEdit->text().trimmed().toStdString() + ":" +
            userEdit->text().trimmed().toStdString() + ":" +
            passwordEdit->text().toStdString();
        return connection;
    }
}
