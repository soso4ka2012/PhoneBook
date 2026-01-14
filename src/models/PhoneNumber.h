#ifndef PHONENUMBER_H
#define PHONENUMBER_H

#include <string>
#include <iostream>

class PhoneNumber {
public:
    enum Type {
        HOME,
        WORK, 
        MOBILE
    };

    // Конструкторы
    PhoneNumber();
    PhoneNumber(const std::string& number, Type type);
    
    // Геттеры
    std::string getNumber() const { return number; }
    Type getType() const { return type; }
    std::string getTypeString() const;
    
    // Сеттеры с валидацией
    bool setNumber(const std::string& newNumber);
    void setType(Type newType) { type = newType; }
    
    // Валидация
    bool isValid() const;
    
    // Преобразование в строку
    std::string toString() const;
    std::string toFileString() const;

    // Статический метод для создания номера
    static PhoneNumber createPhoneNumber(const std::string& number, Type type);

private:
    std::string number;
    Type type;
    
    // Очистка номера от форматирования
    std::string cleanPhoneNumber(const std::string& phone) const;
};

#endif