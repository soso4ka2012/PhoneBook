#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include <iostream>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Устанавливаем информацию об приложении
    app.setApplicationName("Phone Book");
    app.setOrganizationName("MyCompany");
    app.setApplicationVersion("3.0.0");
    
    std::cout << "=====================================" << std::endl;
    std::cout << "Phone Book Application v3.0" << std::endl;
    std::cout << "Features:" << std::endl;
    std::cout << "1. File Storage (data/contacts.txt)" << std::endl;
    std::cout << "2. PostgreSQL Database Storage" << std::endl;
    std::cout << "3. Switch between storage types" << std::endl;
    std::cout << "4. Import/Export functionality" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    try {
        MainWindow window;
        window.show();
        
        return app.exec();
    } catch (const std::exception& e) {
        QMessageBox::critical(nullptr, "Fatal Error", 
                            QString("Application crashed:\n%1").arg(e.what()));
        return 1;
    }
}
