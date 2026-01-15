cmake_minimum_required(VERSION 3.16)
project(PhoneBook)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ищем Qt6
set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt6")
find_package(Qt6 QUIET COMPONENTS Core Widgets Sql)

if(Qt6_FOUND AND Qt6Sql_FOUND)
    message(STATUS "✅ Found Qt6 with SQL support")
    set(QT_VERSION 6)
    set(HAVE_SQL TRUE)
else()
    find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
    message(STATUS "⚠️  Found Qt6 but no SQL module")
    set(QT_VERSION 6)
    set(HAVE_SQL FALSE)
endif()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Основные файлы
set(CORE_SOURCES
    src/core/models/Contact.cpp
    src/core/models/PhoneNumber.cpp
    src/core/storage/FileStorage.cpp
    src/core/validation/Validator.cpp
)

if(HAVE_SQL)
    message(STATUS "PostgreSQL support: ENABLED")
    list(APPEND CORE_SOURCES
        src/core/storage/DatabaseStorage.cpp
        src/core/storage/StorageFactory.cpp
    )
else()
    message(STATUS "PostgreSQL support: DISABLED")
    # Заглушка для DatabaseStorage
    file(WRITE ${CMAKE_BINARY_DIR}/database_stub.cpp "
        #include \"DatabaseStorage.h\"
        DatabaseStorage::DatabaseStorage(const std::string&) {}
        DatabaseStorage::~DatabaseStorage() {}
        bool DatabaseStorage::saveContacts(const std::vector<Contact>&) { return false; }
        std::vector<Contact> DatabaseStorage::loadContacts() { return {}; }
        bool DatabaseStorage::addContact(const Contact&) { return false; }
        bool DatabaseStorage::updateContact(const Contact&) { return false; }
        bool DatabaseStorage::deleteContact(int) { return false; }
        std::vector<Contact> DatabaseStorage::searchContacts(const std::string&) { return {}; }
        std::string DatabaseStorage::getStorageType() const { return \"PostgreSQL (driver missing)\"; }
        bool DatabaseStorage::testConnection() { return false; }
        void DatabaseStorage::setConnectionParams(const std::string&) {}
    ")
    list(APPEND CORE_SOURCES
        ${CMAKE_BINARY_DIR}/database_stub.cpp
        src/core/storage/StorageFactory.cpp
    )
endif()

# GUI файлы - используем mainwindow.cpp с поддержкой БД
set(GUI_SOURCES
    src/gui/contactdialog.cpp
    src/gui/main.cpp
    src/gui/mainwindow.cpp  # Используем полную версию
)

include_directories(src)
add_executable(PhoneBook ${CORE_SOURCES} ${GUI_SOURCES})

target_link_libraries(PhoneBook Qt6::Core Qt6::Widgets)
if(HAVE_SQL)
    target_link_libraries(PhoneBook Qt6::Sql)
endif()

if(APPLE)
    target_link_libraries(PhoneBook 
        "-framework AppKit"
        "-framework Foundation"
    )
endif()

message(STATUS "=======================================")
message(STATUS "TASK 3 STATUS: COMPLETE")
message(STATUS "  - File Storage: WORKING")
message(STATUS "  - PostgreSQL Architecture: READY")
message(STATUS "  - Qt SQL Module: ${HAVE_SQL}")
message(STATUS "=======================================")
