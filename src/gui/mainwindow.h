#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <vector>
#include "../core/models/Contact.h"
#include "../core/storage/IStorage.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddContact();
    void onEditContact();
    void onDeleteContact();
    void onSaveContacts();
    void onLoadContacts();
    void switchToFileStorage();
    void switchToDatabaseStorage();
    // УБРАЛИ: void exportToFile();

private:
    void setupUI();
    void refreshTable();
    void loadContacts();
    void switchStorage(IStorage* newStorage);
    
    QTableWidget *table;
    QPushButton *addButton;
    QPushButton *editButton;
    QPushButton *deleteButton;
    QPushButton *saveButton;
    QPushButton *loadButton;
    QPushButton *fileStorageButton;
    QPushButton *dbStorageButton;
    // УБРАЛИ: QPushButton *exportButton;
    
    std::vector<Contact> contacts;
    IStorage* storage;
};

#endif
