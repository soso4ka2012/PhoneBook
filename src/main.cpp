#include <iostream>
#include <vector>
#include <limits>
#include "models/Contact.h"
#include "models/PhoneNumber.h"
#include "storage/FileStorage.h"
#include "validation/Validator.h"

// Прототипы функций
void printMenu();
void addContact(std::vector<Contact>& contacts);
void listContacts(const std::vector<Contact>& contacts);
void searchContacts(const std::vector<Contact>& contacts);
void deleteContact(std::vector<Contact>& contacts);
void editContact(std::vector<Contact>& contacts);
PhoneNumber::Type getPhoneTypeFromUser();

int main() {
    std::vector<Contact> contacts;
    FileStorage storage("../data/contacts.txt");
    
    std::cout << "=== PHONE BOOK APPLICATION ===" << std::endl;
    std::cout << "Loading contacts..." << std::endl;
    
    // Загружаем контакты из файла
    contacts = storage.loadContacts();
    
    int choice;
    bool running = true;
    
    while (running) {
        printMenu();
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        
        // Очищаем буфер ввода
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        switch (choice) {
            case 1:
                addContact(contacts);
                break;
            case 2:
                listContacts(contacts);
                break;
            case 3:
                searchContacts(contacts);
                break;
            case 4:
                deleteContact(contacts);
                break;
            case 5:
                editContact(contacts);
                break;
            case 6:
                // Сохраняем контакты в файл
                if (storage.saveContacts(contacts)) {
                    std::cout << "Contacts saved successfully!" << std::endl;
                } else {
                    std::cout << "Error saving contacts!" << std::endl;
                }
                break;
            case 0:
                running = false;
                std::cout << "Goodbye!" << std::endl;
                break;
            default:
                std::cout << "Invalid choice! Please try again." << std::endl;
        }
        
        std::cout << std::endl;
    }
    
    return 0;
}

// Вывод главного меню
void printMenu() {
    std::cout << "\n=== MAIN MENU ===" << std::endl;
    std::cout << "1. Add Contact" << std::endl;
    std::cout << "2. List All Contacts" << std::endl;
    std::cout << "3. Search Contacts" << std::endl;
    std::cout << "4. Delete Contact" << std::endl;
    std::cout << "5. Edit Contact" << std::endl;
    std::cout << "6. Save Contacts" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "=================" << std::endl;
}

// Ввод неограниченного количества телефонных номеров
std::vector<PhoneNumber> inputPhoneNumbers() {
    std::vector<PhoneNumber> phones;
    std::cout << "\n=== PHONE NUMBERS ===" << std::endl;
    std::cout << "You can add unlimited phone numbers. Press ENTER with empty input to finish." << std::endl;
    std::cout << "At least one phone number is required!" << std::endl;
    
    int phoneCount = 0;
    bool firstNumber = true;
    
    while (true) {
        std::string phoneNumber;
        
        if (firstNumber) {
            std::cout << "Phone Number #1 (required): ";
            firstNumber = false;
        } else {
            std::cout << "Phone Number #" << (phoneCount + 1) << " (or press ENTER to finish): ";
        }
        
        std::getline(std::cin, phoneNumber);
        
        // Проверяем, хочет ли пользователь закончить ввод (пустая строка)
        if (!firstNumber && phoneNumber.empty()) {
            // Проверяем, что есть хотя бы один номер
            if (phones.empty()) {
                std::cout << "Error: At least one phone number is required! Please enter a phone number." << std::endl;
                firstNumber = true; // Сбрасываем флаг, чтобы снова показать "required"
                continue;
            }
            break;
        }
        
        // Проверяем валидность номера
        if (!Validator::validatePhone(phoneNumber)) {
            std::cout << "Invalid phone number! Please use Russian format: +78121234567, 88121234567, etc." << std::endl;
            continue;
        }
        
        PhoneNumber::Type phoneType = getPhoneTypeFromUser();
        PhoneNumber phone(phoneNumber, phoneType);
        phones.push_back(phone);
        phoneCount++;
        
        std::cout << "✅ Phone number added! Total numbers: " << phones.size() << std::endl;
        
        if (phoneCount == 1) {
            std::cout << "You can add more numbers or press ENTER to finish." << std::endl;
        }
    }
    
    std::cout << "📞 Total phone numbers: " << phones.size() << std::endl;
    return phones;
}

// Добавление нового контакта
void addContact(std::vector<Contact>& contacts) {
    std::cout << "\n=== ADD NEW CONTACT ===" << std::endl;
    
    std::string firstName, lastName, email, phoneNumber;
    
    // Простая проверка имени с обязательным началом с буквы
    auto simpleNameCheck = [](const std::string& name) -> bool {
        if (name.empty()) return false;
        
        // Удаляем пробелы в начале и конце
        std::string trimmed = name;
        trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
        trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
        
        if (trimmed.empty()) return false;
        
        // ДОЛЖЕН НАЧИНАТЬСЯ С БУКВЫ (любой)
        unsigned char firstChar = trimmed[0];
        bool startsWithLetter = std::isalpha(firstChar) || 
                            (firstChar >= 0xC0 && firstChar <= 0xFF); // русские буквы
        
        if (!startsWithLetter) {
            return false;
        }
        
        // Не должен начинаться или заканчиваться дефисом
        if (trimmed[0] == '-') return false;
        if (trimmed.back() == '-') return false;
        
        return true;
    };

    // Ввод имени с проверкой
    std::cout << "First Name (required): ";
    std::getline(std::cin, firstName);

    while (!simpleNameCheck(firstName)) {
        std::cout << "Invalid first name! Rules:" << std::endl;
        std::cout << "- Must start with LETTER (not digit or hyphen)" << std::endl;
        std::cout << "- Can contain letters, digits, spaces, hyphens" << std::endl;
        std::cout << "- Cannot start or end with hyphen" << std::endl;
        std::cout << "First Name (required): ";
        std::getline(std::cin, firstName);
    }

    // Ввод фамилии
    std::cout << "Last Name (required): ";
    std::getline(std::cin, lastName);
    
    while (!simpleNameCheck(lastName)) {
        std::cout << "Invalid last name! Rules:" << std::endl;
        std::cout << "- Cannot be empty" << std::endl;
        std::cout << "- Cannot start with hyphen" << std::endl;
        std::cout << "- Cannot end with hyphen" << std::endl;
        std::cout << "Last Name (required): ";
        std::getline(std::cin, lastName);
    }

    // Ввод email
    std::cout << "Email (required): ";
    std::getline(std::cin, email);

    // Автоматически очищаем email от ВСЕХ пробелов
    std::string cleanEmail = email;
    cleanEmail.erase(std::remove(cleanEmail.begin(), cleanEmail.end(), ' '), cleanEmail.end());

    while (!Validator::validateEmail(cleanEmail)) {
        std::cout << "Invalid email format! Rules:" << std::endl;
        std::cout << "- Only Latin letters and digits" << std::endl;
        std::cout << "- Format: username@domain.com" << std::endl;
        std::cout << "- Spaces will be automatically removed" << std::endl;
        std::cout << "Email (required): ";
        std::getline(std::cin, email);
        cleanEmail = email;
        cleanEmail.erase(std::remove(cleanEmail.begin(), cleanEmail.end(), ' '), cleanEmail.end());
    }
    email = cleanEmail;

    // Ввод телефонных номеров (неограниченное количество)
    std::vector<PhoneNumber> phones = inputPhoneNumbers();

    // Создаем контакт с первым номером
    Contact newContact = Contact::createContact(firstName, lastName, email, phones[0]);

    // Добавляем остальные номера (если есть)
    for (size_t i = 1; i < phones.size(); i++) {
        newContact.addPhoneNumber(phones[i]);
    }

    std::cout << "✅ Contact created with " << phones.size() << " phone number(s)" << std::endl;
    
    // Ввод дополнительных полей
    std::string middleName, address, birthDate;
    
    // Отчество (опционально)
    std::cout << "Middle Name (optional): ";
    std::getline(std::cin, middleName);
    if (!middleName.empty()) {
        if (simpleNameCheck(middleName)) {
            newContact.setMiddleName(middleName);
        } else {
            std::cout << "Invalid middle name format. Skipping..." << std::endl;
        }
    }
    

    
    // Адрес (опционально)
    std::cout << "Address (optional): ";
    std::getline(std::cin, address);
    if (!address.empty()) {
        newContact.setAddress(address);
    }
    
    // Дата рождения (опционально) - С ПЕРЕЗАПРОСОМ ПРИ ОШИБКЕ
    std::cout << "Birth Date (optional, format: YYYY-MM-DD): ";
    std::getline(std::cin, birthDate);
    
    if (!birthDate.empty()) {
        while (!Validator::validateDate(birthDate)) {
            std::cout << "Invalid date! Please check:" << std::endl;
            std::cout << "- Format must be YYYY-MM-DD" << std::endl;
            std::cout << "- Date must be in the past" << std::endl;
            std::cout << "- Must be a valid date (correct days in month, leap years)" << std::endl;
            std::cout << "Birth Date (optional, format: YYYY-MM-DD, or press Enter to skip): ";
            std::getline(std::cin, birthDate);
            if (birthDate.empty()) {
                break; // Пользователь решил пропустить
            }
        }
        
        if (!birthDate.empty()) {
            newContact.setBirthDate(birthDate);
        }
    }
    
    // Добавляем контакт в список
    contacts.push_back(newContact);
    std::cout << "Contact added successfully!" << std::endl;
}

// Вывод всех контактов
void listContacts(const std::vector<Contact>& contacts) {
    std::cout << "\n=== ALL CONTACTS ===" << std::endl;
    
    if (contacts.empty()) {
        std::cout << "No contacts found." << std::endl;
        return;
    }
    
    for (size_t i = 0; i < contacts.size(); ++i) {
        std::cout << "\n--- Contact #" << (i + 1) << " ---" << std::endl;
        std::cout << contacts[i].toString() << std::endl;
    }
    
    std::cout << "\nTotal contacts: " << contacts.size() << std::endl;
}

// Поиск контактов
void searchContacts(const std::vector<Contact>& contacts) {
    std::cout << "\n=== SEARCH CONTACTS ===" << std::endl;
    
    if (contacts.empty()) {
        std::cout << "No contacts to search." << std::endl;
        return;
    }
    
    std::string searchTerm;
    std::cout << "Enter search term (name, last name, email, or phone): ";
    std::getline(std::cin, searchTerm);
    
    if (searchTerm.empty()) {
        std::cout << "Empty search term!" << std::endl;
        return;
    }
    
    std::vector<Contact> foundContacts;
    
    for (const auto& contact : contacts) {
        // Поиск по имени
        if (contact.getFirstName().find(searchTerm) != std::string::npos) {
            foundContacts.push_back(contact);
            continue;
        }
        
        // Поиск по фамилии
        if (contact.getLastName().find(searchTerm) != std::string::npos) {
            foundContacts.push_back(contact);
            continue;
        }
        
        // Поиск по email
        if (contact.getEmail().find(searchTerm) != std::string::npos) {
            foundContacts.push_back(contact);
            continue;
        }
        
        // Поиск по телефонным номерам
        for (const auto& phone : contact.getPhoneNumbers()) {
            if (phone.getNumber().find(searchTerm) != std::string::npos) {
                foundContacts.push_back(contact);
                break;
            }
        }
    }
    
    // Вывод результатов поиска
    std::cout << "\n=== SEARCH RESULTS ===" << std::endl;
    
    if (foundContacts.empty()) {
        std::cout << "No contacts found for: " << searchTerm << std::endl;
        return;
    }
    
    for (size_t i = 0; i < foundContacts.size(); ++i) {
        std::cout << "\n--- Result #" << (i + 1) << " ---" << std::endl;
        std::cout << foundContacts[i].toString() << std::endl;
    }
    
    std::cout << "\nFound " << foundContacts.size() << " contact(s)" << std::endl;
}

// Удаление контакта
void deleteContact(std::vector<Contact>& contacts) {
    std::cout << "\n=== DELETE CONTACT ===" << std::endl;
    
    if (contacts.empty()) {
        std::cout << "No contacts to delete." << std::endl;
        return;
    }
    
    listContacts(contacts);
    
    int contactNumber;
    std::cout << "Enter contact number to delete: ";
    std::cin >> contactNumber;
    
    if (std::cin.fail() || contactNumber < 1 || contactNumber > static_cast<int>(contacts.size())) {
        std::cout << "Invalid contact number!" << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }
    
    // Подтверждение удаления
    std::cout << "Are you sure you want to delete this contact? (y/n): ";
    char confirm;
    std::cin >> confirm;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    if (confirm == 'y' || confirm == 'Y') {
        size_t index = contactNumber - 1;
        std::string deletedName = contacts[index].getFirstName() + " " + contacts[index].getLastName();
        contacts.erase(contacts.begin() + index);
        std::cout << "Contact '" << deletedName << "' deleted successfully!" << std::endl;
    } else {
        std::cout << "Deletion cancelled." << std::endl;
    }
}

// Редактирование контакта (базовая версия)
void editContact(std::vector<Contact>& contacts) {
    std::cout << "\n=== EDIT CONTACT ===" << std::endl;
    
    if (contacts.empty()) {
        std::cout << "No contacts to edit." << std::endl;
        return;
    }
    
    listContacts(contacts);
    
    int contactNumber;
    std::cout << "Enter contact number to edit: ";
    std::cin >> contactNumber;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    if (contactNumber < 1 || contactNumber > static_cast<int>(contacts.size())) {
        std::cout << "Invalid contact number!" << std::endl;
        return;
    }
    
    Contact& contact = contacts[contactNumber - 1];
    std::cout << "Editing contact: " << contact.getFirstName() << " " << contact.getLastName() << std::endl;
    
    // Простая реализация редактирования (можно расширить)
    std::string newAddress;
    std::cout << "Current address: " << contact.getAddress() << std::endl;
    std::cout << "New address (press Enter to keep current): ";
    std::getline(std::cin, newAddress);
    
    if (!newAddress.empty()) {
        contact.setAddress(newAddress);
    }
    
    std::cout << "Contact updated successfully!" << std::endl;
}

// Выбор типа телефона
PhoneNumber::Type getPhoneTypeFromUser() {
    int typeChoice;
    
    std::cout << "Select phone type:" << std::endl;
    std::cout << "1. Mobile" << std::endl;
    std::cout << "2. Home" << std::endl;
    std::cout << "3. Work" << std::endl;
    std::cout << "Enter choice (1-3, default 1): ";
    
    std::string input;
    std::getline(std::cin, input);
    
    if (input.empty()) {
        return PhoneNumber::MOBILE;
    }
    
    try {
        typeChoice = std::stoi(input);
    } catch (...) {
        typeChoice = 1;
    }
    
    switch (typeChoice) {
        case 1: return PhoneNumber::MOBILE;
        case 2: return PhoneNumber::HOME;
        case 3: return PhoneNumber::WORK;
        default: return PhoneNumber::MOBILE;
    }
}