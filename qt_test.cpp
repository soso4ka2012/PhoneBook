#include <QApplication>
#include <QPushButton>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    QPushButton button("Click me!");
    button.resize(200, 100);
    button.show();
    
    QObject::connect(&button, &QPushButton::clicked, []() {
        QApplication::quit();
    });
    
    return app.exec();
}
