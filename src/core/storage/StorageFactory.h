#ifndef STORAGEFACTORY_H
#define STORAGEFACTORY_H

#include "IStorage.h"
#include <string>

enum StorageType {
    FILE_STORAGE,
    DATABASE_STORAGE
};

class StorageFactory {
public:
    StorageFactory();
    
    static IStorage* createStorage(StorageType type, const std::string& params = "");
    static IStorage* createFileStorage(const std::string& filename = "");
    static IStorage* createDatabaseStorage(const std::string& connectionString = "");
    
    static StorageType detectStorageType(const std::string& storageId);
};

#endif
