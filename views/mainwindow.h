#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include "../controllers/contactcontroller.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;
    ContactController* m_controller;
    QStandardItemModel* m_contactsModel;
    bool m_darkTheme;

    void setupUI();
    void setupConnections();
    void refreshContacts();
    void showContactDialog(const Contact& contact = Contact());
    void toggleTheme();

public:
    MainWindow(ContactController* controller, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddContact();
    void onEditContact();
    void onDeleteContact();
    void onSearchTextChanged(const QString& text);
    void onContactDoubleClicked(const QModelIndex& index);
    void onRefreshButtonClicked();
    void onThemeButtonClicked();

    void onContactAdded(const Contact& contact);
    void onContactUpdated(const Contact& contact);
    void onContactDeleted(int id);
    void onErrorOccurred(const QString& error);
};

#endif // MAINWINDOW_H
