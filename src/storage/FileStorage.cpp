#include "FileStorage.h"
#include <fstream>
#include <sstream>
#include <iostream>

// Конструктор
FileStorage::FileStorage(const std::string& filename) 
    : filename(filename) {}

// Сохранение контактов в файл
bool FileStorage::saveContacts(const std::vector<Contact>& contacts) {
    std::ofstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << " for writing!" << std::endl;
        return false;
    }
    
    for (const auto& contact : contacts) {
        file << contact.toFileString() << std::endl;
    }
    
    file.close();
    std::cout << "Successfully saved " << contacts.size() << " contacts to " << filename << std::endl;
    return true;
}

// Загрузка контактов из файла
std::vector<Contact> FileStorage::loadContacts() {
    std::vector<Contact> contacts;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cout << "File " << filename << " not found. Starting with empty contact list." << std::endl;
        return contacts;
    }
    
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Пропускаем пустые строки
        if (line.empty()) {
            continue;
        }
        
        try {
            Contact contact = parseContactFromString(line);
            if (contact.isValid()) {
                contacts.push_back(contact);
            } else {
                std::cout << "Warning: Invalid contact data at line " << lineNumber << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "Error parsing contact at line " << lineNumber << ": " << e.what() << std::endl;
        }
    }
    
    file.close();
    std::cout << "Successfully loaded " << contacts.size() << " contacts from " << filename << std::endl;
    return contacts;
}

// Парсинг строки в объект Contact
Contact FileStorage::parseContactFromString(const std::string& line) {
    std::stringstream ss(line);
    std::string token;
    std::vector<std::string> tokens;
    
    // Разделяем строку по точкам с запятой
    while (std::getline(ss, token, ';')) {
        tokens.push_back(token);
    }
    
    // Должно быть минимум 6 полей + телефоны
    if (tokens.size() < 6) {
        throw std::invalid_argument("Invalid contact format");
    }
    
    // Создаем контакт с обязательными полями
    Contact contact;
    contact.setFirstName(tokens[0]);
    contact.setLastName(tokens[1]);
    contact.setMiddleName(tokens[2]);
    contact.setAddress(tokens[3]);
    contact.setBirthDate(tokens[4]);
    contact.setEmail(tokens[5]);
    
    // Парсим телефонные номера (7-й токен, если есть)
    if (tokens.size() > 6 && !tokens[6].empty()) {
        std::vector<PhoneNumber> phones = parsePhoneNumbers(tokens[6]);
        for (const auto& phone : phones) {
            contact.addPhoneNumber(phone);
        }
    }
    
    return contact;
}

// Парсинг телефонных номеров
std::vector<PhoneNumber> FileStorage::parsePhoneNumbers(const std::string& phonesStr) {
    std::vector<PhoneNumber> phones;
    std::stringstream ss(phonesStr);
    std::string phoneToken;
    
    // Разделяем номера по запятым
    while (std::getline(ss, phoneToken, ',')) {
        // Ищем разделитель типа и номера
        size_t colonPos = phoneToken.find(':');
        if (colonPos != std::string::npos) {
            std::string typeStr = phoneToken.substr(0, colonPos);
            std::string numberStr = phoneToken.substr(colonPos + 1);
            
            PhoneNumber::Type type;
            if (typeStr == "Home") {
                type = PhoneNumber::HOME;
            } else if (typeStr == "Work") {
                type = PhoneNumber::WORK;
            } else if (typeStr == "Mobile") {
                type = PhoneNumber::MOBILE;
            } else {
                continue; // Пропускаем неизвестные типы
            }
            
            PhoneNumber phone(numberStr, type);
            if (phone.isValid()) {
                phones.push_back(phone);
            }
        }
    }
    
    return phones;
}