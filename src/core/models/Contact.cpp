#include "Contact.h"
#include "PhoneNumber.h"
#include "../validation/Validator.h"
#include <algorithm>
#include <sstream>
#include <iomanip> // для std::setw и std::setfill

// Конструктор по умолчанию
Contact::Contact() 
    : firstName(""), lastName(""), middleName(""), 
      address(""), birthDate(""), email("") {}

// Конструктор с обязательными полями
Contact::Contact(const std::string& firstName, const std::string& lastName,
                 const std::string& email, const PhoneNumber& phone)
    : firstName(trim(firstName)), lastName(trim(lastName)), email(trim(email)) {
    phoneNumbers.push_back(phone);
}

// Сеттер для имени с улучшенной валидацией
void Contact::setFirstName(const std::string& name) {
    std::string trimmed = trim(name);
    if (Validator::validateName(trimmed)) {
        firstName = trimmed;
    } else {
        throw std::invalid_argument("Invalid first name format");
    }
}

// Сеттер для фамилии с проверкой на пустоту
void Contact::setLastName(const std::string& name) {
    std::string trimmed = trim(name);
    if (Validator::validateName(trimmed)) {
        lastName = trimmed;
    } else {
        throw std::invalid_argument("Invalid last name format");
    }
}

// Сеттер для email
void Contact::setEmail(const std::string& newEmail) {
    std::string cleaned = Validator::cleanEmail(newEmail);
    if (Validator::validateEmail(cleaned)) {
        email = cleaned;
    } else {
        throw std::invalid_argument("Invalid email format");
    }
}

// Добавление телефонного номера
bool Contact::addPhoneNumber(const PhoneNumber& phone) {
    if (phone.isValid()) {
        phoneNumbers.push_back(phone);
        return true;
    }
    return false;
}

// Удаление телефонного номера по индексу
bool Contact::removePhoneNumber(int index) {
    if (index >= 0 && static_cast<size_t>(index) < phoneNumbers.size()) {
        phoneNumbers.erase(phoneNumbers.begin() + index);
        return true;
    }
    return false;
}

// Проверка, что контакт имеет все обязательные поля
bool Contact::isValid() const {
    return hasRequiredFields();
}

// Проверка обязательных полей
bool Contact::hasRequiredFields() const {
    // Проверяем, что обязательные поля не пустые
    if (firstName.empty() || lastName.empty() || email.empty()) {
        return false;
    }
    
    // Проверяем, что есть хотя бы один номер телефона
    if (phoneNumbers.empty()) {
        return false;
    }
    
    // Проверяем, что все номера валидны
    for (const auto& phone : phoneNumbers) {
        if (!phone.isValid()) {
            return false;
        }
    }
    
    return true;
}

std::string Contact::toString() const {
    std::stringstream ss;
    ss << "Contact: " << firstName << " " << lastName;
    if (!middleName.empty()) {
        ss << " " << middleName;
    }
    ss << "\nEmail: " << email;
    
    if (!birthDate.empty()) {
        ss << "\nBirth Date: " << getFormattedBirthDate();
    }
    
    if (!address.empty()) {
        ss << "\nAddress: " << address;
    }
    
    ss << "\nPhone Numbers:";
    
    // Проходим по всем номерам и группируем вручную
    bool firstOfType[3] = {true, true, true}; // для HOME, WORK, MOBILE
    
    for (const auto& phone : phoneNumbers) {
        std::string typeStr = phone.getTypeString();
        std::string number = phone.getNumber();
        
        if (phone.getType() == PhoneNumber::HOME) {
            if (firstOfType[0]) {
                ss << "\n  " << typeStr << ": " << number;
                firstOfType[0] = false;
            } else {
                ss << "\n        " << number;
            }
        }
        else if (phone.getType() == PhoneNumber::WORK) {
            if (firstOfType[1]) {
                ss << "\n  " << typeStr << ": " << number;
                firstOfType[1] = false;
            } else {
                ss << "\n        " << number;
            }
        }
        else if (phone.getType() == PhoneNumber::MOBILE) {
            if (firstOfType[2]) {
                ss << "\n  " << typeStr << ": " << number;
                firstOfType[2] = false;
            } else {
                ss << "\n        " << number;
            }
        }
    }
    
    return ss.str();
}

// Для сохранения в файл (формат CSV)
std::string Contact::toFileString() const {
    std::stringstream ss;
    ss << firstName << ";" 
       << lastName << ";" 
       << middleName << ";" 
       << address << ";" 
       << birthDate << ";" 
       << email << ";";
    
    // Сохраняем номера телефонов
    for (size_t i = 0; i < phoneNumbers.size(); ++i) {
        if (i > 0) ss << ",";
        ss << phoneNumbers[i].toFileString();
    }
    
    return ss.str();
}

// Форматирование даты в dd-mm-yyyy
std::string Contact::getFormattedBirthDate() const {
    if (birthDate.empty()) {
        return "";
    }
    
    // Парсим дату из формата YYYY-MM-DD
    int year, month, day;
    char dash1, dash2;
    std::istringstream ss(birthDate);
    ss >> year >> dash1 >> month >> dash2 >> day;
    
    // Форматируем в DD-MM-YYYY
    std::stringstream formatted;
    formatted << std::setw(2) << std::setfill('0') << day << "-"
              << std::setw(2) << std::setfill('0') << month << "-"
              << year;
    
    return formatted.str();
}

// Статический метод создания контакта
Contact Contact::createContact(const std::string& firstName, const std::string& lastName,
                              const std::string& email, const PhoneNumber& phone) {
    return Contact(firstName, lastName, email, phone);
}

// Удаление пробелов в начале и конце строки
std::string Contact::trim(const std::string& str) const {
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");
    
    if (start == std::string::npos) {
        return ""; // Строка состоит только из пробелов
    }
    
    return str.substr(start, end - start + 1);
}