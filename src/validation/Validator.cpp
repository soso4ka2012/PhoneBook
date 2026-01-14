#include "Validator.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

// Инициализация статических регулярных выражений
// Более строгие регулярные выражения
const std::regex Validator::nameRegex("^[A-Za-zА-Яа-я][A-Za-zА-Яа-я0-9\\s\\-]*[A-Za-zА-Яа-я0-9]$");
const std::regex Validator::phoneRegex("^(\\+7|8)[\\s\\-\\(\\)0-9]{10,15}$");
const std::regex Validator::emailRegex("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$");
const std::regex Validator::dateRegex("^\\d{4}-\\d{2}-\\d{2}$");

// Простая валидация имени
bool Validator::validateName(const std::string& name) {
    if (name.empty()) return false;
    
    std::string trimmed = trim(name);
    if (trimmed.empty()) return false;
    
    // Первый символ не должен быть цифрой или дефисом
    char firstChar = trimmed[0];
    if (std::isdigit(firstChar) || firstChar == '-') {
        return false;
    }
    
    // Не должен заканчиваться дефисом
    if (trimmed.back() == '-') {
        return false;
    }
    
    return true;
}

// Валидация email
bool Validator::validateEmail(const std::string& email) {
    if (email.empty()) return false;
    
    // Проверяем что есть ровно один @
    int atCount = 0;
    for (char c : email) {
        if (c == '@') atCount++;
    }
    if (atCount != 1) return false;
    
    // Разделяем на local part и domain part
    size_t atPos = email.find('@');
    std::string localPart = email.substr(0, atPos);
    std::string domainPart = email.substr(atPos + 1);
    
    // Проверяем local part (только латинские буквы, цифры и разрешенные символы)
    if (localPart.empty()) return false;
    for (char c : localPart) {
        if (!std::isalnum(c) && c != '.' && c != '_' && c != '%' && c != '+' && c != '-') {
            return false;
        }
    }
    
    // Local part не должен начинаться или заканчиваться точкой
    if (localPart.front() == '.' || localPart.back() == '.') {
        return false;
    }
    
    // Проверяем domain part (только латинские буквы, цифры, точки и дефисы)
    if (domainPart.empty()) return false;
    for (char c : domainPart) {
        if (!std::isalnum(c) && c != '.' && c != '-') {
            return false;
        }
    }
    
    // Domain part должен содержать точку
    if (domainPart.find('.') == std::string::npos) {
        return false;
    }
    
    // Domain part не должен начинаться или заканчиваться дефисом или точкой
    if (domainPart.front() == '.' || domainPart.front() == '-' ||
        domainPart.back() == '.' || domainPart.back() == '-') {
        return false;
    }
    
    return true;
}

// Валидация телефонного номера
bool Validator::validatePhone(const std::string& phone) {
    if (phone.empty()) return false;
    
    // Проверяем регулярным выражением
    bool regexValid = std::regex_match(phone, phoneRegex);
    if (!regexValid) return false;
    
    // Дополнительная проверка: после очистки должно быть 10-11 цифр
    std::string cleaned;
    for (char c : phone) {
        if (std::isdigit(c)) {
            cleaned += c;
        }
    }
    
    // Если номер начинается с +7 или 8, убираем первую цифру для проверки длины
    if (!cleaned.empty() && (cleaned[0] == '7' || cleaned[0] == '8')) {
        cleaned = cleaned.substr(1);
    }
    
    return cleaned.length() == 10;
}

// Валидация даты рождения
bool Validator::validateDate(const std::string& date) {
    if (date.empty()) return true; // Дата не обязательна
    
    // Проверяем формат с помощью regex
    if (!std::regex_match(date, dateRegex)) {
        return false;
    }
    
    // Парсим дату
    int year, month, day;
    char dash1, dash2;
    std::istringstream ss(date);
    ss >> year >> dash1 >> month >> dash2 >> day;
    
    if (dash1 != '-' || dash2 != '-') {
        return false;
    }
    
    // Проверяем диапазоны месяцев и дней
    if (month < 1 || month > 12) {
        return false;
    }
    
    if (day < 1 || day > 31) {
        return false;
    }
    
    // Проверяем конкретные месяцы
    if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30) {
        return false;
    }
    
    // Проверяем февраль
    if (month == 2) {
        bool isLeap = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
        if (day > (isLeap ? 29 : 28)) {
            return false;
        }
    }
    
    // Проверяем, что дата не в будущем
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    int currentYear = now->tm_year + 1900;
    int currentMonth = now->tm_mon + 1;
    int currentDay = now->tm_mday;
    
    if (year > currentYear) {
        return false;
    }
    if (year == currentYear && month > currentMonth) {
        return false;
    }
    if (year == currentYear && month == currentMonth && day > currentDay) {
        return false;
    }
    
    // Проверяем разумный минимальный год (например, 1900)
    if (year < 1900) {
        return false;
    }
    
    return true;
}

// Очистка строки от пробелов
std::string Validator::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) {
        return "";
    }
    
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

// Очистка email - удаляем ВСЕ пробелы
std::string Validator::cleanEmail(const std::string& email) {
    std::string result = email;
    
    // Удаляем ВСЕ пробелы из email (включая пробелы вокруг @)
    result.erase(std::remove(result.begin(), result.end(), ' '), result.end());
    
    return result;
}