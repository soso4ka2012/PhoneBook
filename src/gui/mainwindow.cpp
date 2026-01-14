#include "mainwindow.h"
#include "contactdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <iostream>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), storage("data/contacts.txt") {
    setupUI();
    loadContactsFromFile();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    QHBoxLayout *toolbarLayout = new QHBoxLayout();
    
    addButton = new QPushButton("➕ Add Contact", this);
    editButton = new QPushButton("✏️ Edit", this);
    deleteButton = new QPushButton("🗑️ Delete", this);
    saveButton = new QPushButton("💾 Save", this);
    loadButton = new QPushButton("📂 Load", this);
    
    toolbarLayout->addWidget(addButton);
    toolbarLayout->addWidget(editButton);
    toolbarLayout->addWidget(deleteButton);
    toolbarLayout->addWidget(saveButton);
    toolbarLayout->addWidget(loadButton);
    toolbarLayout->addStretch();
    
    mainLayout->addLayout(toolbarLayout);
    
    QHBoxLayout *searchLayout = new QHBoxLayout();
    
    searchField = new QLineEdit(this);
    searchField->setPlaceholderText("Search by name, email or phone...");
    searchButton = new QPushButton("🔍 Search", this);
    
    sortCombo = new QComboBox(this);
    sortCombo->addItem("Sort by: First Name");
    sortCombo->addItem("Sort by: Last Name");
    sortCombo->addItem("Sort by: Email");
    sortCombo->addItem("Sort by: Birth Date");
    sortButton = new QPushButton("Sort", this);
    
    searchLayout->addWidget(searchField);
    searchLayout->addWidget(searchButton);
    searchLayout->addWidget(sortCombo);
    searchLayout->addWidget(sortButton);
    
    mainLayout->addLayout(searchLayout);
    
    table = new QTableWidget(this);
    table->setColumnCount(6);
    table->setHorizontalHeaderLabels({"First Name", "Last Name", "Email", "Phone", "Birth Date", "Address"});
    table->horizontalHeader()->setStretchLastSection(true);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    mainLayout->addWidget(table);
    
    connect(addButton, &QPushButton::clicked, this, &MainWindow::onAddContact);
    connect(editButton, &QPushButton::clicked, this, &MainWindow::onEditContact);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::onDeleteContact);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::onSaveContacts);
    connect(loadButton, &QPushButton::clicked, this, &MainWindow::onLoadContacts);
    connect(searchButton, &QPushButton::clicked, this, &MainWindow::onSearch);
    connect(sortButton, &QPushButton::clicked, this, &MainWindow::onSort);
    
    setWindowTitle("Phone Book");
    resize(900, 600);
}

void MainWindow::refreshTable() {
    table->setRowCount(contacts.size());
    
    for (size_t i = 0; i < contacts.size(); ++i) {
        const Contact& contact = contacts[i];
        
        // Формируем строку с телефонами
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
}

void MainWindow::loadContactsFromFile() {
    contacts = storage.loadContacts();
    refreshTable();
}

void MainWindow::onAddContact() {
    ContactDialog dialog(this, ContactDialog::ADD);
    if (dialog.exec() == QDialog::Accepted) {
        Contact newContact = dialog.getContact();
        contacts.push_back(newContact);
        refreshTable();
        QMessageBox::information(this, "Success", "Contact added successfully!");
    }
}

void MainWindow::onEditContact() {
    int row = table->currentRow();
    if (row >= 0 && row < static_cast<int>(contacts.size())) {
        ContactDialog dialog(this, ContactDialog::EDIT, &contacts[row]);
        if (dialog.exec() == QDialog::Accepted) {
            contacts[row] = dialog.getContact();
            refreshTable();
            QMessageBox::information(this, "Success", "Contact updated successfully!");
        }
    } else {
        QMessageBox::warning(this, "Warning", "Please select a contact to edit");
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
            contacts.erase(contacts.begin() + row);
            refreshTable();
        }
    } else {
        QMessageBox::warning(this, "Warning", "Please select a contact to delete");
    }
}

void MainWindow::onSearch() {
    QString searchText = searchField->text().trimmed();
    if (searchText.isEmpty()) {
        loadContactsFromFile();
        return;
    }
    
    std::vector<Contact> foundContacts;
    std::string searchStr = searchText.toLower().toStdString();
    
    for (const auto& contact : contacts) {
        std::string firstName = contact.getFirstName();
        std::string lastName = contact.getLastName();
        std::string email = contact.getEmail();
        
        std::transform(firstName.begin(), firstName.end(), firstName.begin(), ::tolower);
        std::transform(lastName.begin(), lastName.end(), lastName.begin(), ::tolower);
        std::transform(email.begin(), email.end(), email.begin(), ::tolower);
        
        if (firstName.find(searchStr) != std::string::npos ||
            lastName.find(searchStr) != std::string::npos ||
            email.find(searchStr) != std::string::npos) {
            foundContacts.push_back(contact);
        }
    }
    
    // Временно показываем найденные
    std::vector<Contact> original = contacts;
    contacts = foundContacts;
    refreshTable();
    contacts = original;
}

void MainWindow::onSort() {
    int index = sortCombo->currentIndex();
    
    switch (index) {
        case 0: // First Name
            std::sort(contacts.begin(), contacts.end(),
                     [](const Contact& a, const Contact& b) {
                         return a.getFirstName() < b.getFirstName();
                     });
            break;
        case 1: // Last Name
            std::sort(contacts.begin(), contacts.end(),
                     [](const Contact& a, const Contact& b) {
                         return a.getLastName() < b.getLastName();
                     });
            break;
        case 2: // Email
            std::sort(contacts.begin(), contacts.end(),
                     [](const Contact& a, const Contact& b) {
                         return a.getEmail() < b.getEmail();
                     });
            break;
        case 3: // Birth Date
            std::sort(contacts.begin(), contacts.end(),
                     [](const Contact& a, const Contact& b) {
                         return a.getBirthDate() < b.getBirthDate();
                     });
            break;
    }
    
    refreshTable();
}

void MainWindow::onSaveContacts() {
    if (storage.saveContacts(contacts)) {
        QMessageBox::information(this, "Success", "Contacts saved successfully!");
    } else {
        QMessageBox::critical(this, "Error", "Failed to save contacts!");
    }
}

void MainWindow::onLoadContacts() {
    QString filename = QFileDialog::getOpenFileName(this, "Load Contacts", 
                                                   "data", "Text files (*.txt)");
    if (!filename.isEmpty()) {
        storage = FileStorage(filename);
        loadContactsFromFile();
    }
}
