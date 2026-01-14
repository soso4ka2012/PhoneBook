#ifndef FILESTORAGE_H
#define FILESTORAGE_H

#include <vector>
#include <string>
#include "../models/Contact.h"
#include <QString>
#include <QFile>

class FileStorage {
public:
    FileStorage(const QString& filename = "data/contacts.txt");
    
    bool saveContacts(const std::vector<Contact>& contacts);
    std::vector<Contact> loadContacts();
    
    QString getFilename() const { return filename; }

private:
    QString filename;
    
    // Вспомогательные методы
    QString getProjectPath() const;
    Contact parseContactFromString(const std::string& line);
    std::vector<PhoneNumber> parsePhoneNumbers(const std::string& phonesStr);
};

#endif
