#ifndef CONTACTDIALOG_H
#define CONTACTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QTableWidget>
#include <vector>
#include "../core/models/Contact.h"
#include "../core/models/PhoneNumber.h"

class ContactDialog : public QDialog {
    Q_OBJECT

public:
    enum Mode { ADD, EDIT };
    
    ContactDialog(QWidget *parent = nullptr, Mode mode = ADD, 
                  const Contact* contact = nullptr);
    ~ContactDialog();
    
    Contact getContact() const { return contact; }
    
private slots:
    void onAddPhone();
    void onRemovePhone();
    void onAccept();
    void onReject();

private:
    void setupUI();
    void loadContactData();
    bool validateInput();
    
    Mode mode;
    Contact contact;
    
    // Основные поля
    QLineEdit *firstNameEdit;
    QLineEdit *lastNameEdit;
    QLineEdit *middleNameEdit;
    QLineEdit *emailEdit;
    QLineEdit *addressEdit;
    QLineEdit *birthDateEdit;
    
    // Телефоны
    QTableWidget *phoneTable;
    QPushButton *addPhoneButton;
    QPushButton *removePhoneButton;
    
    QPushButton *okButton;
    QPushButton *cancelButton;
};

#endif
