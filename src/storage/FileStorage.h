#ifndef FILESTORAGE_H
#define FILESTORAGE_H

#include <vector>
#include <string>
#include "../models/Contact.h"

class FileStorage {
public:
    FileStorage(const std::string& filename = "data/contacts.txt");
    
    // Сохранение всех контактов в файл
    bool saveContacts(const std::vector<Contact>& contacts);
    
    // Загрузка всех контактов из файла
    std::vector<Contact> loadContacts();
    
    // Получение имени файла
    std::string getFilename() const { return filename; }

private:
    std::string filename;
    
    // Парсинг строки из файла в объект Contact
    Contact parseContactFromString(const std::string& line);
    
    // Парсинг телефонных номеров из строки
    std::vector<PhoneNumber> parsePhoneNumbers(const std::string& phonesStr);
};

#endif