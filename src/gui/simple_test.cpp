#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    QMainWindow window;
    window.setWindowTitle("Phone Book - Test");
    
    QWidget *central = new QWidget(&window);
    QVBoxLayout *layout = new QVBoxLayout(central);
    
    QLabel *label = new QLabel("📱 Phone Book Application\nQt is working!", central);
    label->setAlignment(Qt::AlignCenter);
    
    layout->addWidget(label);
    window.setCentralWidget(central);
    window.resize(500, 300);
    window.show();
    
    return app.exec();
}
