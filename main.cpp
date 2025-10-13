#include <QApplication>
#include <QMessageBox>
#include <QDir>
#include "repositories/contactrepository.h"
#include "controllers/contactcontroller.h"
#include "views/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("PhoneBook");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("0xReflection");

    ContactRepository* repository = new ContactRepository();
    ContactController* controller = new ContactController(repository);
    if (!repository->connectToDatabase("HOME-PC\\SQLEXPRESS", "phonebook", "", "", 0)) {
        QMessageBox::critical(nullptr, "Database Error",
                              "Cannot connect to SQL Server Express.\n"
                              "Please ensure that:\n"
                              "1. SQL Server Express is running\n"
                              "2. Database 'phonebook' exists\n"
                              "3. Windows Authentication is enabled");
        return -1;
    }
    MainWindow mainWindow(controller);
    mainWindow.show();

    int result = app.exec();

    delete controller;
    delete repository;

    return result;
}
