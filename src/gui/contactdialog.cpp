#include "contactdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QDateEdit>
#include "../core/validation/Validator.h"
#include <iostream>

ContactDialog::ContactDialog(QWidget *parent, Mode mode, const Contact* existingContact)
    : QDialog(parent), mode(mode) {
    
    if (existingContact) {
        contact = *existingContact;
    }
    
    setupUI();
    setWindowTitle(mode == ADD ? "Add Contact" : "Edit Contact");
    resize(600, 500);
}

ContactDialog::~ContactDialog() {}

void ContactDialog::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Форма с полями
    QGroupBox *formGroup = new QGroupBox("Contact Information", this);
    QFormLayout *formLayout = new QFormLayout(formGroup);
    
    firstNameEdit = new QLineEdit(this);
    lastNameEdit = new QLineEdit(this);
    middleNameEdit = new QLineEdit(this);
    emailEdit = new QLineEdit(this);
    addressEdit = new QLineEdit(this);
    birthDateEdit = new QLineEdit(this);
    birthDateEdit->setPlaceholderText("YYYY-MM-DD");
    
    formLayout->addRow("First Name *:", firstNameEdit);
    formLayout->addRow("Last Name *:", lastNameEdit);
    formLayout->addRow("Middle Name:", middleNameEdit);
    formLayout->addRow("Email *:", emailEdit);
    formLayout->addRow("Address:", addressEdit);
    formLayout->addRow("Birth Date:", birthDateEdit);
    
    mainLayout->addWidget(formGroup);
    
    // Таблица телефонов
    QGroupBox *phoneGroup = new QGroupBox("Phone Numbers (at least one required)", this);
    QVBoxLayout *phoneLayout = new QVBoxLayout(phoneGroup);
    
    phoneTable = new QTableWidget(this);
    phoneTable->setColumnCount(3);
    phoneTable->setHorizontalHeaderLabels({"Type", "Number", "Remove"});
    phoneTable->horizontalHeader()->setStretchLastSection(true);
    
    // Кнопки для телефонов
    QHBoxLayout *phoneButtonsLayout = new QHBoxLayout();
    addPhoneButton = new QPushButton("➕ Add Phone", this);
    removePhoneButton = new QPushButton("➖ Remove Selected", this);
    
    phoneButtonsLayout->addWidget(addPhoneButton);
    phoneButtonsLayout->addWidget(removePhoneButton);
    phoneButtonsLayout->addStretch();
    
    phoneLayout->addWidget(phoneTable);
    phoneLayout->addLayout(phoneButtonsLayout);
    
    mainLayout->addWidget(phoneGroup);
    
    // Кнопки OK/Cancel
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    okButton = new QPushButton("OK", this);
    cancelButton = new QPushButton("Cancel", this);
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Загружаем данные если редактируем
    if (mode == EDIT) {
        loadContactData();
    } else {
        // Добавляем одну пустую строку для телефона
        phoneTable->setRowCount(1);
        
        QComboBox *typeCombo = new QComboBox();
        typeCombo->addItem("Mobile");
        typeCombo->addItem("Home");
        typeCombo->addItem("Work");
        phoneTable->setCellWidget(0, 0, typeCombo);
        
        QLineEdit *phoneEdit = new QLineEdit();
        phoneEdit->setPlaceholderText("+79161234567");
        phoneTable->setCellWidget(0, 1, phoneEdit);
        
        QPushButton *removeBtn = new QPushButton("X");
        phoneTable->setCellWidget(0, 2, removeBtn);
        connect(removeBtn, &QPushButton::clicked, [this]() {
            if (phoneTable->rowCount() > 1) {
                phoneTable->removeRow(phoneTable->currentRow());
            }
        });
    }
    
    // Подключаем сигналы
    connect(addPhoneButton, &QPushButton::clicked, this, &ContactDialog::onAddPhone);
    connect(removePhoneButton, &QPushButton::clicked, this, &ContactDialog::onRemovePhone);
    connect(okButton, &QPushButton::clicked, this, &ContactDialog::onAccept);
    connect(cancelButton, &QPushButton::clicked, this, &ContactDialog::onReject);
}

void ContactDialog::loadContactData() {
    // Заполняем поля данными контакта
    firstNameEdit->setText(QString::fromStdString(contact.getFirstName()));
    lastNameEdit->setText(QString::fromStdString(contact.getLastName()));
    middleNameEdit->setText(QString::fromStdString(contact.getMiddleName()));
    emailEdit->setText(QString::fromStdString(contact.getEmail()));
    addressEdit->setText(QString::fromStdString(contact.getAddress()));
    birthDateEdit->setText(QString::fromStdString(contact.getBirthDate()));
    
    // Загружаем телефоны
    const auto& phones = contact.getPhoneNumbers();
    phoneTable->setRowCount(phones.size());
    
    for (size_t i = 0; i < phones.size(); ++i) {
        const PhoneNumber& phone = phones[i];
        
        QComboBox *typeCombo = new QComboBox();
        typeCombo->addItem("Mobile");
        typeCombo->addItem("Home");
        typeCombo->addItem("Work");
        
        switch (phone.getType()) {
            case PhoneNumber::MOBILE: typeCombo->setCurrentIndex(0); break;
            case PhoneNumber::HOME: typeCombo->setCurrentIndex(1); break;
            case PhoneNumber::WORK: typeCombo->setCurrentIndex(2); break;
        }
        
        phoneTable->setCellWidget(i, 0, typeCombo);
        
        QLineEdit *phoneEdit = new QLineEdit();
        phoneEdit->setText(QString::fromStdString(phone.getNumber()));
        phoneTable->setCellWidget(i, 1, phoneEdit);
        
        QPushButton *removeBtn = new QPushButton("X");
        phoneTable->setCellWidget(i, 2, removeBtn);
        connect(removeBtn, &QPushButton::clicked, [this, i]() {
            if (phoneTable->rowCount() > 1) {
                phoneTable->removeRow(i);
            }
        });
    }
}

void ContactDialog::onAddPhone() {
    int row = phoneTable->rowCount();
    phoneTable->insertRow(row);
    
    QComboBox *typeCombo = new QComboBox();
    typeCombo->addItem("Mobile");
    typeCombo->addItem("Home");
    typeCombo->addItem("Work");
    phoneTable->setCellWidget(row, 0, typeCombo);
    
    QLineEdit *phoneEdit = new QLineEdit();
    phoneEdit->setPlaceholderText("+79161234567");
    phoneTable->setCellWidget(row, 1, phoneEdit);
    
    QPushButton *removeBtn = new QPushButton("X");
    phoneTable->setCellWidget(row, 2, removeBtn);
    connect(removeBtn, &QPushButton::clicked, [this, row]() {
        if (phoneTable->rowCount() > 1) {
            phoneTable->removeRow(row);
        }
    });
}

void ContactDialog::onRemovePhone() {
    int row = phoneTable->currentRow();
    if (row >= 0 && phoneTable->rowCount() > 1) {
        phoneTable->removeRow(row);
    }
}

bool ContactDialog::validateInput() {
    // Проверка обязательных полей
    if (firstNameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "First name is required!");
        return false;
    }
    
    if (lastNameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", "Last name is required!");
        return false;
    }
    
    // Проверка email
    std::string email = emailEdit->text().trimmed().toStdString();
    std::string cleanEmail = email;
    cleanEmail.erase(std::remove(cleanEmail.begin(), cleanEmail.end(), ' '), cleanEmail.end());
    
    if (!Validator::validateEmail(cleanEmail)) {
        QMessageBox::warning(this, "Validation Error", 
                            "Invalid email format!\nExample: user@example.com");
        return false;
    }
    
    // Проверка телефонов
    bool hasValidPhone = false;
    for (int i = 0; i < phoneTable->rowCount(); ++i) {
        QLineEdit *phoneEdit = qobject_cast<QLineEdit*>(phoneTable->cellWidget(i, 1));
        if (phoneEdit && !phoneEdit->text().trimmed().isEmpty()) {
            std::string phone = phoneEdit->text().trimmed().toStdString();
            if (Validator::validatePhone(phone)) {
                hasValidPhone = true;
            }
        }
    }
    
    if (!hasValidPhone) {
        QMessageBox::warning(this, "Validation Error", 
                            "At least one valid phone number is required!");
        return false;
    }
    
    // Проверка даты (если указана)
    std::string birthDate = birthDateEdit->text().trimmed().toStdString();
    if (!birthDate.empty() && !Validator::validateDate(birthDate)) {
        QMessageBox::warning(this, "Validation Error", 
                            "Invalid date format! Use YYYY-MM-DD");
        return false;
    }
    
    return true;
}

void ContactDialog::onAccept() {
    if (!validateInput()) {
        return;
    }
    
    // Создаем контакт
    Contact newContact;
    newContact.setFirstName(firstNameEdit->text().trimmed().toStdString());
    newContact.setLastName(lastNameEdit->text().trimmed().toStdString());
    newContact.setMiddleName(middleNameEdit->text().trimmed().toStdString());
    newContact.setEmail(emailEdit->text().trimmed().toStdString());
    newContact.setAddress(addressEdit->text().trimmed().toStdString());
    
    std::string birthDate = birthDateEdit->text().trimmed().toStdString();
    if (!birthDate.empty()) {
        newContact.setBirthDate(birthDate);
    }
    
    // Добавляем телефоны
    for (int i = 0; i < phoneTable->rowCount(); ++i) {
        QComboBox *typeCombo = qobject_cast<QComboBox*>(phoneTable->cellWidget(i, 0));
        QLineEdit *phoneEdit = qobject_cast<QLineEdit*>(phoneTable->cellWidget(i, 1));
        
        if (typeCombo && phoneEdit && !phoneEdit->text().trimmed().isEmpty()) {
            std::string phoneStr = phoneEdit->text().trimmed().toStdString();
            std::string typeStr = typeCombo->currentText().toStdString();
            
            PhoneNumber::Type type = PhoneNumber::MOBILE;
            if (typeStr == "Home") type = PhoneNumber::HOME;
            else if (typeStr == "Work") type = PhoneNumber::WORK;
            
            PhoneNumber phone(phoneStr, type);
            if (phone.isValid()) {
                newContact.addPhoneNumber(phone);
            }
        }
    }
    
    contact = newContact;
    accept();
}

void ContactDialog::onReject() {
    reject();
}
