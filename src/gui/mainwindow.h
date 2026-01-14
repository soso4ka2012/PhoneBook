#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <vector>
#include "../core/models/Contact.h"
#include "../core/storage/FileStorage.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddContact();
    void onEditContact();
    void onDeleteContact();
    void onSearch();
    void onSort();
    void onSaveContacts();
    void onLoadContacts();

private:
    void setupUI();
    void refreshTable();
    void loadContactsFromFile();
    
    QTableWidget *table;
    QPushButton *addButton;
    QPushButton *editButton;
    QPushButton *deleteButton;
    QPushButton *saveButton;
    QPushButton *loadButton;
    QPushButton *searchButton;
    QPushButton *sortButton;
    QLineEdit *searchField;
    QComboBox *sortCombo;
    
    std::vector<Contact> contacts;
    FileStorage storage;
};

#endif
