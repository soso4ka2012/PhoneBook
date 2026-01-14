#include "FileStorage.h"
#include <sstream>
#include <iostream>
#include <QTextStream>
#include <QDir>
#include <QCoreApplication>

FileStorage::FileStorage(const QString& filename) 
    : filename(getProjectPath() + "/" + filename) {
    
    std::cout << "FileStorage initialized" << std::endl;
    std::cout << "Looking for file: " << this->filename.toStdString() << std::endl;
}

QString FileStorage::getProjectPath() const {
    // Получаем путь к исполняемому файлу
    QString exePath = QCoreApplication::applicationDirPath();
    QDir exeDir(exePath);
    
    // Если мы в папке build, поднимаемся на уровень вверх (в корень проекта)
    if (exeDir.dirName() == "build") {
        exeDir.cdUp();
    }
    
    QString projectPath = exeDir.absolutePath();
    std::cout << "Project path: " << projectPath.toStdString() << std::endl;
    
    return projectPath;
}

bool FileStorage::saveContacts(const std::vector<Contact>& contacts) {
    std::cout << "Saving to: " << filename.toStdString() << std::endl;
    
    // Создаем директорию если ее нет
    QFileInfo fileInfo(filename);
    QDir dir = fileInfo.dir();
    std::cout << "Directory: " << dir.path().toStdString() << std::endl;
    
    if (!dir.exists()) {
        std::cout << "Creating directory..." << std::endl;
        if (!dir.mkpath(".")) {
            std::cerr << "Error: Cannot create directory " << dir.path().toStdString() << std::endl;
            return false;
        }
        std::cout << "Directory created successfully" << std::endl;
    }
    
    QFile file(filename);
    
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        std::cerr << "Error: Cannot open file " << filename.toStdString() << " for writing!" << std::endl;
        std::cerr << "Error details: " << file.errorString().toStdString() << std::endl;
        return false;
    }
    
    QTextStream out(&file);
    int savedCount = 0;
    
    for (const auto& contact : contacts) {
        out << QString::fromStdString(contact.toFileString()) << "\n";
        savedCount++;
    }
    
    file.close();
    std::cout << "Successfully saved " << savedCount << " contacts to " << filename.toStdString() << std::endl;
    
    // Проверяем что файл действительно создался
    if (QFile::exists(filename)) {
        std::cout << "File verified: exists and saved correctly" << std::endl;
    } else {
        std::cerr << "Warning: File was not created!" << std::endl;
    }
    
    return true;
}

std::vector<Contact> FileStorage::loadContacts() {
    std::vector<Contact> contacts;
    
    std::cout << "Loading from: " << filename.toStdString() << std::endl;
    
    QFile file(filename);
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cout << "File not found. Will create new file when saving." << std::endl;
        std::cout << "Full path attempted: " << file.fileName().toStdString() << std::endl;
        return contacts;
    }
    
    QTextStream in(&file);
    int lineNumber = 0;
    int loadedCount = 0;
    
    while (!in.atEnd()) {
        QString line = in.readLine();
        lineNumber++;
        
        if (line.isEmpty()) {
            continue;
        }
        
        try {
            Contact contact = parseContactFromString(line.toStdString());
            if (contact.isValid()) {
                contacts.push_back(contact);
                loadedCount++;
            } else {
                std::cout << "Warning: Invalid contact data at line " << lineNumber << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "Error parsing contact at line " << lineNumber << ": " << e.what() << std::endl;
        }
    }
    
    file.close();
    std::cout << "Successfully loaded " << loadedCount << " contacts" << std::endl;
    return contacts;
}

// Остальные функции без изменений...
Contact FileStorage::parseContactFromString(const std::string& line) {
    std::stringstream ss(line);
    std::string token;
    std::vector<std::string> tokens;
    
    while (std::getline(ss, token, ';')) {
        tokens.push_back(token);
    }
    
    if (tokens.size() < 6) {
        throw std::invalid_argument("Invalid contact format");
    }
    
    Contact contact;
    contact.setFirstName(tokens[0]);
    contact.setLastName(tokens[1]);
    contact.setMiddleName(tokens[2]);
    contact.setAddress(tokens[3]);
    contact.setBirthDate(tokens[4]);
    contact.setEmail(tokens[5]);
    
    if (tokens.size() > 6 && !tokens[6].empty()) {
        std::vector<PhoneNumber> phones = parsePhoneNumbers(tokens[6]);
        for (const auto& phone : phones) {
            contact.addPhoneNumber(phone);
        }
    }
    
    return contact;
}

std::vector<PhoneNumber> FileStorage::parsePhoneNumbers(const std::string& phonesStr) {
    std::vector<PhoneNumber> phones;
    std::stringstream ss(phonesStr);
    std::string phoneToken;
    
    while (std::getline(ss, phoneToken, ',')) {
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
                continue;
            }
            
            PhoneNumber phone(numberStr, type);
            if (phone.isValid()) {
                phones.push_back(phone);
            }
        }
    }
    
    return phones;
}
