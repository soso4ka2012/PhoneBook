#include "PhoneNumber.h"
#include <algorithm>
#include <cctype>

// Конструктор по умолчанию
PhoneNumber::PhoneNumber() : number(""), type(HOME) {}

// Конструктор с параметрами
PhoneNumber::PhoneNumber(const std::string& number, Type type) 
    : number(cleanPhoneNumber(number)), type(type) {}

// Возвращает строковое представление типа номера
std::string PhoneNumber::getTypeString() const {
    switch(type) {
        case HOME: return "Home";
        case WORK: return "Work";
        case MOBILE: return "Mobile";
        default: return "Unknown";
    }
}

// Установка номера с валидацией
bool PhoneNumber::setNumber(const std::string& newNumber) {
    std::string cleaned = cleanPhoneNumber(newNumber);
    
    // Проверяем, что номер состоит только из цифр
    for (char c : cleaned) {
        if (!std::isdigit(c)) {
            return false;
        }
    }
    
    // Проверяем длину номера (10-11 цифр после очистки)
    if (cleaned.length() < 10 || cleaned.length() > 11) {
        return false;
    }
    
    number = cleaned;
    return true;
}

// Проверка валидности номера
bool PhoneNumber::isValid() const {
    if (number.empty()) return false;
    
    // Проверяем, что номер состоит только из цифр
    for (char c : number) {
        if (!std::isdigit(c)) {
            return false;
        }
    }
    
    // Проверяем длину номера
    return number.length() >= 10 && number.length() <= 11;
}

// Для красивого вывода в консоль
std::string PhoneNumber::toString() const {
    return getTypeString() + ": " + number;
}

// Для сохранения в файл
std::string PhoneNumber::toFileString() const {
    return getTypeString() + ":" + number;
}

// Статический метод создания номера
PhoneNumber PhoneNumber::createPhoneNumber(const std::string& number, Type type) {
    return PhoneNumber(number, type);
}

// Очистка номера от скобок, пробелов, дефисов
std::string PhoneNumber::cleanPhoneNumber(const std::string& phone) const {
    std::string result;
    for (char c : phone) {
        if (std::isdigit(c)) {
            result += c;
        }
    }
    return result;
}