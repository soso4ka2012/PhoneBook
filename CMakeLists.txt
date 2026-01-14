cmake_minimum_required(VERSION 3.16)
project(PhoneBook)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Для MacOS добавляем флаги
if(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++17")
endif()

set(SOURCE_FILES
    src/main.cpp
    src/models/Contact.cpp
    src/models/PhoneNumber.cpp
    src/storage/FileStorage.cpp
    src/validation/Validator.cpp
)

include_directories(src)

add_executable(PhoneBook ${SOURCE_FILES})

message("=== PhoneBook Configuration ===")
message("Source files: ${SOURCE_FILES}")
message("C++ standard: ${CMAKE_CXX_STANDARD}")
