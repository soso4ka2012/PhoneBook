#include "StorageFactory.h"
#include "FileStorage.h"
#include "DatabaseStorage.h"
#include <iostream>

StorageFactory::StorageFactory() {}

IStorage* StorageFactory::createStorage(StorageType type, const std::string& params) {
    switch(type) {
        case FILE_STORAGE:
            return createFileStorage(params);
        case DATABASE_STORAGE:
            return createDatabaseStorage(params);
        default:
            return nullptr;
    }
}

IStorage* StorageFactory::createFileStorage(const std::string& filename) {
    std::string filepath = filename.empty() ? "data/contacts.txt" : filename;
    FileStorage* storage = new FileStorage(QString::fromStdString(filepath));
    std::cout << "Created FileStorage with file: " << filepath << std::endl;
    return storage;
}

IStorage* StorageFactory::createDatabaseStorage(const std::string& connectionString) {
    // Парсим строку подключения: host:port:dbname:user:password
    std::string host = "localhost";
    int port = 5432;
    std::string dbName = "phonebook";
    std::string user = "phonebook_user";
    std::string password = "phonebook_password";
    
    if (!connectionString.empty()) {
        size_t pos = 0;
        size_t start = 0;
        int paramIndex = 0;
        
        while ((pos = connectionString.find(':', start)) != std::string::npos) {
            std::string param = connectionString.substr(start, pos - start);
            
            switch(paramIndex) {
                case 0: host = param; break;
                case 1: port = std::stoi(param); break;
                case 2: dbName = param; break;
                case 3: user = param; break;
            }
            
            start = pos + 1;
            paramIndex++;
        }
        
        if (start < connectionString.length()) {
            password = connectionString.substr(start);
        }
    }
    
    DatabaseStorage* storage = new DatabaseStorage(connectionString);
    
    std::cout << "Created DatabaseStorage with params:" << std::endl;
    std::cout << "  Host: " << host << ":" << port << std::endl;
    std::cout << "  Database: " << dbName << std::endl;
    std::cout << "  User: " << user << std::endl;
    
    return storage;
}

StorageType StorageFactory::detectStorageType(const std::string& storageId) {
    if (storageId == "file") {
        return FILE_STORAGE;
    } else if (storageId == "database" || storageId == "postgres" || storageId == "postgresql") {
        return DATABASE_STORAGE;
    } else if (storageId.find("localhost:5432") != std::string::npos ||
               storageId.find("postgresql://") != std::string::npos) {
        return DATABASE_STORAGE;
    } else if (storageId.find(".txt") != std::string::npos ||
               storageId.find(".csv") != std::string::npos) {
        return FILE_STORAGE;
    } else {
        return FILE_STORAGE; // По умолчанию файловое хранилище
    }
}
