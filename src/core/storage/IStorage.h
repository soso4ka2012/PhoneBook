#ifndef ISTORAGE_H
#define ISTORAGE_H

#include <vector>
#include "../models/Contact.h"

class IStorage {
public:
    virtual ~IStorage() = default;
    
    virtual bool saveContacts(const std::vector<Contact>& contacts) = 0;
    virtual std::vector<Contact> loadContacts() = 0;
    virtual bool addContact(const Contact& contact) = 0;
    virtual bool updateContact(const Contact& contact) = 0;
    virtual bool deleteContact(int id) = 0;
    virtual std::vector<Contact> searchContacts(const std::string& query) = 0;
    
    virtual std::string getStorageType() const = 0;
    virtual bool testConnection() = 0;
    virtual void setConnectionParams(const std::string& params) = 0;
};

#endif
