#ifndef FILESTORAGE_H
#define FILESTORAGE_H

#include "IStorage.h"
#include <vector>
#include <string>
#include "../models/Contact.h"
#include <QString>
#include <QFile>

class FileStorage : public IStorage {
public:
    FileStorage(const QString& filename = "data/contacts.txt");
    
    // IStorage interface implementation
    bool saveContacts(const std::vector<Contact>& contacts) override;
    std::vector<Contact> loadContacts() override;
    bool addContact(const Contact& contact) override;
    bool updateContact(const Contact& contact) override;
    bool deleteContact(int id) override;
    std::vector<Contact> searchContacts(const std::string& query) override;
    
    std::string getStorageType() const override { return "File Storage"; }
    bool testConnection() override;
    void setConnectionParams(const std::string& params) override {
        filename = QString::fromStdString(params);
    }
    
    QString getFilename() const { return filename; }

private:
    QString filename;
    
    // Вспомогательные методы
    QString getProjectPath() const;
    Contact parseContactFromString(const std::string& line);
    std::vector<PhoneNumber> parsePhoneNumbers(const std::string& phonesStr);
    bool saveAllContacts(const std::vector<Contact>& contacts);
};

#endif
