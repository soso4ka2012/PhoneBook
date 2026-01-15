#include "mainwindow.h"
#include "contactdialog.h"
#include "../core/storage/StorageFactory.h"
#include "../core/storage/FileStorage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QStatusBar>
#include <QLabel>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), storage(nullptr) {
    
    storage = StorageFactory::createStorage(FILE_STORAGE, "data/contacts.txt");
    setupUI();
    loadContacts();
}

MainWindow::~MainWindow() {
    if (storage) delete storage;
}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    // Заголовок (УБРАЛИ большой заголовок)
    QLabel *titleLabel = new QLabel("PhoneBook - Switchable Storage", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-weight: bold; margin: 5px;");
    mainLayout->addWidget(titleLabel);
    
    // Панель управления хранилищем
    QHBoxLayout *storageLayout = new QHBoxLayout();
    
    fileStorageButton = new QPushButton("📁 File Storage", this);
    dbStorageButton = new QPushButton("🗄️ PostgreSQL", this);
    loadButton = new QPushButton("📂 Load", this);
    saveButton = new QPushButton("💾 Save", this);
    
    storageLayout->addWidget(fileStorageButton);
    storageLayout->addWidget(dbStorageButton);
    storageLayout->addWidget(loadButton);
    storageLayout->addWidget(saveButton);
    storageLayout->addStretch();
    
    mainLayout->addLayout(storageLayout);
    
    // Панель управления контактами
    QHBoxLayout *contactsLayout = new QHBoxLayout();
    
    addButton = new QPushButton("➕ Add Contact", this);
    editButton = new QPushButton("✏️ Edit", this);
    deleteButton = new QPushButton("🗑️ Delete", this);
    
    contactsLayout->addWidget(addButton);
    contactsLayout->addWidget(editButton);
    contactsLayout->addWidget(deleteButton);
    contactsLayout->addStretch();
    
    mainLayout->addLayout(contactsLayout);
    
    // Таблица контактов
    table = new QTableWidget(this);
    table->setColumnCount(6);
    table->setHorizontalHeaderLabels({"First Name", "Last Name", "Email", "Phone", "Birth Date", "Address"});
    table->horizontalHeader()->setStretchLastSection(true);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    
    mainLayout->addWidget(table);
    
    // Строка состояния
    statusBar()->showMessage("Ready | Storage: File");
    
    // Подключаем сигналы
    connect(addButton, &QPushButton::clicked, this, &MainWindow::onAddContact);
    connect(editButton, &QPushButton::clicked, this, &MainWindow::onEditContact);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::onDeleteContact);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::onSaveContacts);
    connect(loadButton, &QPushButton::clicked, this, &MainWindow::onLoadContacts);
    connect(fileStorageButton, &QPushButton::clicked, this, &MainWindow::switchToFileStorage);
    connect(dbStorageButton, &QPushButton::clicked, this, &MainWindow::switchToDatabaseStorage);
    
    // УПРОЩЕННЫЙ заголовок окна
    setWindowTitle("PhoneBook");
    resize(1000, 600);
}

void MainWindow::refreshTable() {
    table->setRowCount(contacts.size());
    
    for (size_t i = 0; i < contacts.size(); ++i) {
        const Contact& contact = contacts[i];
        
        std::string phonesStr;
        const auto& phones = contact.getPhoneNumbers();
        for (size_t j = 0; j < phones.size(); ++j) {
            if (j > 0) phonesStr += ", ";
            phonesStr += phones[j].getNumber();
        }
        
        table->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(contact.getFirstName())));
        table->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(contact.getLastName())));
        table->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(contact.getEmail())));
        table->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(phonesStr)));
        table->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(contact.getBirthDate())));
        table->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(contact.getAddress())));
    }
    
    QString status = QString("Contacts: %1 | Storage: %2")
                        .arg(contacts.size())
                        .arg(storage ? QString::fromStdString(storage->getStorageType()) : "None");
    statusBar()->showMessage(status);
}

void MainWindow::loadContacts() {
    if (storage) {
        contacts = storage->loadContacts();
        refreshTable();
    }
}

void MainWindow::switchStorage(IStorage* newStorage) {
    if (storage) {
        delete storage;
    }
    storage = newStorage;
    
    if (storage) {
        loadContacts();
    }
}

void MainWindow::onAddContact() {
    ContactDialog dialog(this, ContactDialog::ADD);
    if (dialog.exec() == QDialog::Accepted) {
        Contact newContact = dialog.getContact();
        
        if (storage) {
            if (storage->addContact(newContact)) {
                contacts.push_back(newContact);
                refreshTable();
            }
        }
    }
}

void MainWindow::onEditContact() {
    int row = table->currentRow();
    if (row >= 0 && row < static_cast<int>(contacts.size())) {
        ContactDialog dialog(this, ContactDialog::EDIT, &contacts[row]);
        if (dialog.exec() == QDialog::Accepted) {
            contacts[row] = dialog.getContact();
            if (storage) storage->saveContacts(contacts);
            refreshTable();
        }
    }
}

void MainWindow::onDeleteContact() {
    int row = table->currentRow();
    if (row >= 0 && row < static_cast<int>(contacts.size())) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Delete Contact", 
                                      "Are you sure you want to delete this contact?",
                                      QMessageBox::Yes | QMessageBox::No);
        
        if (reply == QMessageBox::Yes) {
            if (storage) {
                if (storage->deleteContact(row)) {
                    contacts.erase(contacts.begin() + row);
                    refreshTable();
                }
            }
        }
    }
}

void MainWindow::onSaveContacts() {
    if (storage && storage->saveContacts(contacts)) {
        statusBar()->showMessage("Saved successfully");
    }
}

void MainWindow::onLoadContacts() {
    loadContacts();
    statusBar()->showMessage("Loaded from storage");
}

void MainWindow::switchToFileStorage() {
    IStorage* fileStorage = StorageFactory::createStorage(FILE_STORAGE, "data/contacts.txt");
    switchStorage(fileStorage);
    statusBar()->showMessage("Switched to File Storage");
}

void MainWindow::switchToDatabaseStorage() {
    IStorage* dbStorage = StorageFactory::createStorage(DATABASE_STORAGE, 
                                                       "localhost:5432:phonebook:phonebook_user:phonebook_password");
    if (dbStorage && dbStorage->testConnection()) {
        switchStorage(dbStorage);
        statusBar()->showMessage("Switched to PostgreSQL Database");
    } else {
        QMessageBox::critical(this, "Connection Failed", 
                            "Cannot connect to PostgreSQL!\n"
                            "Make sure PostgreSQL is running and Qt driver is installed.");
        delete dbStorage;
    }
}
