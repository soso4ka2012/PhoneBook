#include "DatabaseStorage.h"
#include <iostream>
#include <sstream>
#include <QMessageBox>
#include <QSqlError>
#include <QCoreApplication>

DatabaseStorage::DatabaseStorage(const std::string& connectionString)
    : connectionString(connectionString) {
    
    std::cout << "DatabaseStorage initialized" << std::endl;
    parseConnectionString();
}

DatabaseStorage::~DatabaseStorage() {
    closeConnection();
}

void DatabaseStorage::parseConnectionString() {
    if (connectionString.empty()) {
        // Значения по умолчанию
        host = "localhost";
        port = 5432;
        dbName = "phonebook";
        user = "phonebook_user";
        password = "phonebook_password";
    } else {
        // Парсим строку подключения: host:port:dbname:user:password
        std::string str = connectionString;
        size_t pos = 0;
        size_t start = 0;
        int paramIndex = 0;
        
        while ((pos = str.find(':', start)) != std::string::npos) {
            std::string param = str.substr(start, pos - start);
            
            switch(paramIndex) {
                case 0: host = QString::fromStdString(param); break;
                case 1: port = std::stoi(param); break;
                case 2: dbName = QString::fromStdString(param); break;
                case 3: user = QString::fromStdString(param); break;
            }
            
            start = pos + 1;
            paramIndex++;
        }
        
        if (start < str.length()) {
            password = QString::fromStdString(str.substr(start));
        }
    }
    
    std::cout << "Database: " << dbName.toStdString() << " on " 
              << host.toStdString() << ":" << port << std::endl;
}

bool DatabaseStorage::openConnection() {
    if (!db.isOpen()) {
        // Создаем уникальное имя подключения
        static int connectionCount = 0;
        QString connectionName = QString("phonebook_%1").arg(++connectionCount);
        
        db = QSqlDatabase::addDatabase("QPSQL", connectionName);
        db.setHostName(host);
        db.setPort(port);
        db.setDatabaseName(dbName);
        db.setUserName(user);
        db.setPassword(password);
        
        // Настройки таймаута
        db.setConnectOptions("connect_timeout=5");
        
        std::cout << "Connecting to PostgreSQL..." << std::endl;
        
        if (!db.open()) {
            QSqlError error = db.lastError();
            std::cerr << "Database connection failed: " << error.text().toStdString() << std::endl;
            return false;
        }
        
        std::cout << "✅ Database connected!" << std::endl;
    }
    return true;
}

void DatabaseStorage::closeConnection() {
    if (db.isOpen()) {
        QString connectionName = db.connectionName();
        db.close();
        QSqlDatabase::removeDatabase(connectionName);
    }
}

bool DatabaseStorage::testConnection() {
    std::cout << "Testing database connection..." << std::endl;
    
    if (!openConnection()) {
        return false;
    }
    
    QSqlQuery query(db);
    if (!query.exec("SELECT 1")) {
        QSqlError error = query.lastError();
        std::cerr << "Test query failed: " << error.text().toStdString() << std::endl;
        return false;
    }
    
    std::cout << "✅ Connection test passed!" << std::endl;
    return true;
}

bool DatabaseStorage::initializeDatabase() {
    if (!openConnection()) {
        return false;
    }
    
    QSqlQuery query(db);
    
    // Создаем таблицу контактов
    QString createContacts = 
        "CREATE TABLE IF NOT EXISTS contacts ("
        "id SERIAL PRIMARY KEY, "
        "first_name VARCHAR(100) NOT NULL, "
        "last_name VARCHAR(100) NOT NULL, "
        "middle_name VARCHAR(100), "
        "address TEXT, "
        "birth_date DATE, "
        "email VARCHAR(255) NOT NULL UNIQUE, "
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ");";
    
    if (!query.exec(createContacts)) {
        std::cerr << "Failed to create contacts table" << std::endl;
        return false;
    }
    
    // Создаем таблицу телефонов
    QString createPhones = 
        "CREATE TABLE IF NOT EXISTS phone_numbers ("
        "id SERIAL PRIMARY KEY, "
        "contact_id INTEGER NOT NULL REFERENCES contacts(id) ON DELETE CASCADE, "
        "phone_type VARCHAR(20) NOT NULL CHECK (phone_type IN ('MOBILE', 'HOME', 'WORK')), "
        "phone_number VARCHAR(20) NOT NULL"
        ");";
    
    if (!query.exec(createPhones)) {
        std::cerr << "Failed to create phone_numbers table" << std::endl;
        return false;
    }
    
    std::cout << "✅ Database tables initialized" << std::endl;
    return true;
}

std::vector<Contact> DatabaseStorage::loadContacts() {
    std::vector<Contact> contacts;
    
    if (!openConnection()) {
        return contacts;
    }
    
    initializeDatabase();
    
    QSqlQuery query(db);
    QString sql = "SELECT * FROM contacts ORDER BY last_name, first_name";
    
    if (!query.exec(sql)) {
        std::cerr << "Failed to load contacts" << std::endl;
        return contacts;
    }
    
    while (query.next()) {
        Contact contact;
        
        contact.setFirstName(query.value("first_name").toString().toStdString());
        contact.setLastName(query.value("last_name").toString().toStdString());
        contact.setMiddleName(query.value("middle_name").toString().toStdString());
        contact.setAddress(query.value("address").toString().toStdString());
        
        if (!query.value("birth_date").isNull()) {
            contact.setBirthDate(query.value("birth_date").toString().toStdString());
        }
        
        contact.setEmail(query.value("email").toString().toStdString());
        
        // Загружаем телефоны
        int contactId = query.value("id").toInt();
        QSqlQuery phoneQuery(db);
        phoneQuery.prepare("SELECT * FROM phone_numbers WHERE contact_id = :id");
        phoneQuery.bindValue(":id", contactId);
        
        if (phoneQuery.exec()) {
            while (phoneQuery.next()) {
                std::string phoneNumber = phoneQuery.value("phone_number").toString().toStdString();
                std::string phoneTypeStr = phoneQuery.value("phone_type").toString().toStdString();
                
                PhoneNumber::Type type = PhoneNumber::MOBILE;
                if (phoneTypeStr == "HOME") type = PhoneNumber::HOME;
                else if (phoneTypeStr == "WORK") type = PhoneNumber::WORK;
                
                PhoneNumber phone(phoneNumber, type);
                contact.addPhoneNumber(phone);
            }
        }
        
        contacts.push_back(contact);
    }
    
    std::cout << "Loaded " << contacts.size() << " contacts from database" << std::endl;
    return contacts;
}

bool DatabaseStorage::saveContacts(const std::vector<Contact>& contacts) {
    if (!openConnection()) {
        return false;
    }
    
    db.transaction();
    
    try {
        // Очищаем таблицы
        QSqlQuery clearQuery(db);
        clearQuery.exec("DELETE FROM phone_numbers");
        clearQuery.exec("DELETE FROM contacts");
        
        // Вставляем контакты
        for (const auto& contact : contacts) {
            QSqlQuery insertContact(db);
            insertContact.prepare(
                "INSERT INTO contacts (first_name, last_name, middle_name, address, birth_date, email) "
                "VALUES (:first_name, :last_name, :middle_name, :address, :birth_date, :email) "
                "RETURNING id"
            );
            
            insertContact.bindValue(":first_name", QString::fromStdString(contact.getFirstName()));
            insertContact.bindValue(":last_name", QString::fromStdString(contact.getLastName()));
            insertContact.bindValue(":middle_name", QString::fromStdString(contact.getMiddleName()));
            insertContact.bindValue(":address", QString::fromStdString(contact.getAddress()));
            
            std::string birthDate = contact.getBirthDate();
            if (birthDate.empty()) {
                insertContact.bindValue(":birth_date", QVariant());
            } else {
                insertContact.bindValue(":birth_date", QString::fromStdString(birthDate));
            }
            
            insertContact.bindValue(":email", QString::fromStdString(contact.getEmail()));
            
            if (!insertContact.exec()) {
                throw std::runtime_error("Failed to insert contact");
            }
            
            if (!insertContact.next()) {
                throw std::runtime_error("Failed to get contact ID");
            }
            
            int contactId = insertContact.value(0).toInt();
            
            // Вставляем телефоны
            const auto& phones = contact.getPhoneNumbers();
            for (const auto& phone : phones) {
                QSqlQuery insertPhone(db);
                insertPhone.prepare(
                    "INSERT INTO phone_numbers (contact_id, phone_type, phone_number) "
                    "VALUES (:contact_id, :phone_type, :phone_number)"
                );
                
                insertPhone.bindValue(":contact_id", contactId);
                
                QString typeStr;
                switch (phone.getType()) {
                    case PhoneNumber::MOBILE: typeStr = "MOBILE"; break;
                    case PhoneNumber::HOME: typeStr = "HOME"; break;
                    case PhoneNumber::WORK: typeStr = "WORK"; break;
                }
                
                insertPhone.bindValue(":phone_type", typeStr);
                insertPhone.bindValue(":phone_number", QString::fromStdString(phone.getNumber()));
                
                if (!insertPhone.exec()) {
                    throw std::runtime_error("Failed to insert phone");
                }
            }
        }
        
        db.commit();
        std::cout << "✅ Saved " << contacts.size() << " contacts to database" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        db.rollback();
        std::cerr << "❌ Error: " << e.what() << std::endl;
        return false;
    }
}

bool DatabaseStorage::addContact(const Contact& contact) {
    std::vector<Contact> contacts = loadContacts();
    contacts.push_back(contact);
    return saveContacts(contacts);
}

bool DatabaseStorage::updateContact(const Contact& contact) {
    std::vector<Contact> contacts = loadContacts();
    
    // Найти и обновить
    for (auto& c : contacts) {
        if (c.getEmail() == contact.getEmail()) {
            c = contact;
            return saveContacts(contacts);
        }
    }
    
    return addContact(contact);
}

bool DatabaseStorage::deleteContact(int id) {
    if (!openConnection()) {
        return false;
    }
    
    QSqlQuery query(db);
    query.prepare("DELETE FROM contacts WHERE id = :id");
    query.bindValue(":id", id);
    
    return query.exec();
}

std::vector<Contact> DatabaseStorage::searchContacts(const std::string& queryStr) {
    std::vector<Contact> results;
    
    if (!openConnection()) {
        return results;
    }
    
    QSqlQuery query(db);
    QString sql = 
        "SELECT * FROM contacts WHERE "
        "first_name ILIKE :query OR "
        "last_name ILIKE :query OR "
        "email ILIKE :query OR "
        "address ILIKE :query";
    
    query.prepare(sql);
    query.bindValue(":query", "%" + QString::fromStdString(queryStr) + "%");
    
    if (query.exec()) {
        while (query.next()) {
            Contact contact;
            contact.setFirstName(query.value("first_name").toString().toStdString());
            contact.setLastName(query.value("last_name").toString().toStdString());
            contact.setMiddleName(query.value("middle_name").toString().toStdString());
            contact.setAddress(query.value("address").toString().toStdString());
            contact.setEmail(query.value("email").toString().toStdString());
            
            if (!query.value("birth_date").isNull()) {
                contact.setBirthDate(query.value("birth_date").toString().toStdString());
            }
            
            results.push_back(contact);
        }
    }
    
    return results;
}

void DatabaseStorage::setConnectionParams(const std::string& params) {
    connectionString = params;
    parseConnectionString();
    closeConnection();
}

std::string DatabaseStorage::getStorageType() const { 
    return "PostgreSQL Database";
}
