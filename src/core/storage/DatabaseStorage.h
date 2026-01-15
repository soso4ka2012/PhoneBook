#ifndef DATABASESTORAGE_H
#define DATABASESTORAGE_H

#include "IStorage.h"
#include <string>
#include <vector>
#include "../models/Contact.h"
#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

class DatabaseStorage : public IStorage {
public:
    DatabaseStorage(const std::string& connectionString = "");
    ~DatabaseStorage();
    
    // IStorage interface implementation
    bool saveContacts(const std::vector<Contact>& contacts) override;
    std::vector<Contact> loadContacts() override;
    bool addContact(const Contact& contact) override;
    bool updateContact(const Contact& contact) override;
    bool deleteContact(int id) override;
    std::vector<Contact> searchContacts(const std::string& query) override;
    
    std::string getStorageType() const override;
    
    bool testConnection() override;
    void setConnectionParams(const std::string& params) override;
    
private:
    QSqlDatabase db;
    QString host;
    int port;
    QString dbName;
    QString user;
    QString password;
    std::string connectionString;
    
    void parseConnectionString();
    bool openConnection();
    void closeConnection();
    bool initializeDatabase();
    std::vector<PhoneNumber> loadPhoneNumbers(int contactId);
};

#endif
