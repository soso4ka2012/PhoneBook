#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <string>
#include <regex>
#include <ctime>

class Validator {
public:
    // Валидация имени (имя, фамилия, отчество)
    static bool validateName(const std::string& name);
    
    // Валидация телефонного номера
    static bool validatePhone(const std::string& phone);
    
    // Валидация email
    static bool validateEmail(const std::string& email);
    
    // Валидация даты рождения
    static bool validateDate(const std::string& date);
    
    // Очистка строки от пробелов в начале и конце
    static std::string trim(const std::string& str);
    
    // Очистка email (удаление пробелов вокруг @)
    static std::string cleanEmail(const std::string& email);

private:
    // Более строгие регулярные выражения
    static const std::regex nameRegex;
    static const std::regex phoneRegex;
    static const std::regex emailRegex;
    static const std::regex dateRegex;
};

#endif