cmake_minimum_required(VERSION 3.16)
project(PhoneBook)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Указываем путь к Qt6 (ARM версия для M1/M2)
set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt")

# Автоматические возможности Qt
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Ищем Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Файлы ядра
set(CORE_SOURCES
    src/core/models/Contact.cpp
    src/core/models/PhoneNumber.cpp
    src/core/storage/FileStorage.cpp
    src/core/validation/Validator.cpp
)

# Файлы GUI
set(GUI_SOURCES
    src/gui/main.cpp
    src/gui/mainwindow.cpp
)

include_directories(src)

# Исполняемый файл
add_executable(PhoneBook ${CORE_SOURCES} ${GUI_SOURCES})

# Подключаем Qt
target_link_libraries(PhoneBook Qt6::Core Qt6::Widgets)

# Для MacOS ARM
if(APPLE)
    target_link_libraries(PhoneBook 
        "-framework AppKit"
        "-framework Foundation"
    )
endif()

message("=====================================")
message("Qt6 version: ${Qt6_VERSION}")
message("Qt6 Core found: ${Qt6Core_FOUND}")
message("Qt6 Widgets found: ${Qt6Widgets_FOUND}")
message("Build type: ${CMAKE_BUILD_TYPE}")
message("=====================================")
