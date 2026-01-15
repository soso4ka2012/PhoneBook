#ifndef STORAGEDIALOG_H
#define STORAGEDIALOG_H

#include <QDialog>
#include <string>

class StorageDialog : public QDialog {
    Q_OBJECT

public:
    StorageDialog(QWidget *parent = nullptr) : QDialog(parent) {}
    ~StorageDialog() {}
    
    std::string getSelectedStorage() const { return "file"; }
    std::string getConnectionString() const { return ""; }
};

#endif
