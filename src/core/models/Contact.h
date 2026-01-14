#ifndef CONTACT_H
#define CONTACT_H

#include "PhoneNumber.h"
#include <string>
#include <vector>
#include <ctime>

class Contact {
public:
    // Конструкторы
    Contact();
    Contact(const std::string& firstName, const std::string& lastName, 
            const std::string& email, const PhoneNumber& phone);
    
    // Геттеры для обязательных полей
    std::string getFirstName() const { return firstName; }
    std::string getLastName() const { return lastName; }
    std::string getEmail() const { return email; }
    const std::vector<PhoneNumber>& getPhoneNumbers() const { return phoneNumbers; }

    std::string getFormattedBirthDate() const; //валидация даты
    
    // Геттеры для дополнительных полей
    std::string getMiddleName() const { return middleName; }
    std::string getAddress() const { return address; }
    std::string getBirthDate() const { return birthDate; }
    
    // Сеттеры
    void setFirstName(const std::string& name);
    void setLastName(const std::string& name);
    void setMiddleName(const std::string& name) { middleName = trim(name); }
    void setEmail(const std::string& newEmail);
    void setAddress(const std::string& addr) { address = trim(addr); }
    void setBirthDate(const std::string& date) { birthDate = trim(date); }
    
    // Работа с телефонами
    bool addPhoneNumber(const PhoneNumber& phone);
    bool removePhoneNumber(int index);
    void clearPhoneNumbers() { phoneNumbers.clear(); }
    
    // Валидация
    bool isValid() const;
    bool hasRequiredFields() const;
    
    // Преобразование в строку
    std::string toString() const;
    std::string toFileString() const;
    
    // Статические методы для создания контакта
    static Contact createContact(const std::string& firstName, const std::string& lastName,
                                const std::string& email, const PhoneNumber& phone);

private:
    std::string firstName;
    std::string lastName;
    std::string middleName;
    std::string address;
    std::string birthDate;  // Формат: YYYY-MM-DD
    std::string email;
    std::vector<PhoneNumber> phoneNumbers;
    
    // Вспомогательные методы
    std::string trim(const std::string& str) const;
};

#endif