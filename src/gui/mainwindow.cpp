#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <iostream>

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
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    addButton = new QPushButton("Add Contact", this);
    editButton = new QPushButton("Edit", this);
    deleteButton = new QPushButton("Delete", this);
    saveButton = new QPushButton("Save", this);
    
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(editButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addWidget(saveButton);
    buttonLayout->addStretch();
    
    mainLayout->addLayout(buttonLayout);
    
    table = new QTableWidget(this);
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels({"Name", "Email", "Phone"});
    table->horizontalHeader()->setStretchLastSection(true);
    
    mainLayout->addWidget(table);
    
    connect(addButton, &QPushButton::clicked, this, &MainWindow::onAddContact);
    connect(editButton, &QPushButton::clicked, this, &MainWindow::onEditContact);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::onDeleteContact);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::onSaveContacts);
    
    setWindowTitle("Phone Book");
    resize(800, 600);
}

void MainWindow::refreshTable() {
    table->setRowCount(contacts.size());
    
    for (size_t i = 0; i < contacts.size(); ++i) {
        const Contact& contact = contacts[i];
        
        std::string name = contact.getFirstName() + " " + contact.getLastName();
        if (!contact.getMiddleName().empty()) {
            name += " " + contact.getMiddleName();
        }
        
        table->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(name)));
        table->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(contact.getEmail())));
        
        std::string phones;
        for (const auto& phone : contact.getPhoneNumbers()) {
            if (!phones.empty()) phones += ", ";
            phones += phone.getNumber();
        }
        table->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(phones)));
    }
}

void MainWindow::loadContactsFromFile() {
    contacts = storage.loadContacts();
    refreshTable();
}

void MainWindow::onAddContact() {
    QMessageBox::information(this, "Info", "Add contact - to be implemented");
}

void MainWindow::onEditContact() {
    QMessageBox::information(this, "Info", "Edit contact - to be implemented");
}

void MainWindow::onDeleteContact() {
    int row = table->currentRow();
    if (row >= 0) {
        contacts.erase(contacts.begin() + row);
        refreshTable();
    }
}

void MainWindow::onSearch() {
    // To be implemented
}

void MainWindow::onSort() {
    // To be implemented
}

void MainWindow::onSaveContacts() {
    if (storage.saveContacts(contacts)) {
        QMessageBox::information(this, "Success", "Contacts saved!");
    }
}

void MainWindow::onLoadContacts() {
    loadContactsFromFile();
}
