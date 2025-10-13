#include "MainWindow.h"
#include "../views/ui_MainWindow.h"
#include "../views/contactdialog.h"
#include <QMessageBox>
#include <QInputDialog>

MainWindow::MainWindow(ContactController* controller, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_controller(controller)
    , m_contactsModel(new QStandardItemModel(this))
{
    ui->setupUi(this);
    setupUI();
    setupConnections();
    refreshContacts();

    setWindowTitle("PhoneBook");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    m_contactsModel->setHorizontalHeaderLabels({"ID", "First Name", "Last Name", "Phone", "Email"});
    ui->contactsTableView->setModel(m_contactsModel);
    ui->contactsTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->contactsTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->contactsTableView->setColumnHidden(0, true);
    ui->contactsTableView->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::setupConnections()
{
    connect(ui->addButton, &QPushButton::clicked, this, &MainWindow::onAddContact);
    connect(ui->editButton, &QPushButton::clicked, this, &MainWindow::onEditContact);
    connect(ui->deleteButton, &QPushButton::clicked, this, &MainWindow::onDeleteContact);
    connect(ui->searchLineEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
    connect(ui->contactsTableView, &QTableView::doubleClicked, this, &MainWindow::onContactDoubleClicked);
    connect(m_controller, &ContactController::contactAdded, this, &MainWindow::onContactAdded);
    connect(m_controller, &ContactController::contactUpdated, this, &MainWindow::onContactUpdated);
    connect(m_controller, &ContactController::contactDeleted, this, &MainWindow::onContactDeleted);
    connect(m_controller, &ContactController::errorOccurred, this, &MainWindow::onErrorOccurred);
}

void MainWindow::refreshContacts()
{
    auto contacts = m_controller->getAllContacts();
    m_contactsModel->removeRows(0, m_contactsModel->rowCount());

    for (const auto& contact : contacts) {
        QList<QStandardItem*> items;
        items << new QStandardItem(QString::number(contact.id()));
        items << new QStandardItem(contact.firstName());
        items << new QStandardItem(contact.lastName());
        items << new QStandardItem(contact.phone());
        items << new QStandardItem(contact.email());

        m_contactsModel->appendRow(items);
    }

    ui->statusLabel->setText(QString("Total contacts: %1").arg(contacts.size()));
}

void MainWindow::onAddContact()
{
    showContactDialog();
}

void MainWindow::onEditContact()
{
    QModelIndexList selected = ui->contactsTableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please select a contact to edit");
        return;
    }

    int contactId = m_contactsModel->item(selected.first().row(), 0)->text().toInt();
    auto contact = m_controller->getContactById(contactId);

    if (contact) {
        showContactDialog(contact.value());
    }
}

void MainWindow::onDeleteContact()
{
    QModelIndexList selected = ui->contactsTableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please select a contact to delete");
        return;
    }

    int contactId = m_contactsModel->item(selected.first().row(), 0)->text().toInt();
    QString contactName = m_contactsModel->item(selected.first().row(), 1)->text() + " " +
                          m_contactsModel->item(selected.first().row(), 2)->text();

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Confirm Delete",
        QString("Are you sure you want to delete '%1'?").arg(contactName),
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        m_controller->deleteContact(contactId);
    }
}

void MainWindow::onSearchTextChanged(const QString& text)
{
    if (text.isEmpty()) {
        refreshContacts();
    } else {
        auto contacts = m_controller->searchContacts(text);
        m_contactsModel->removeRows(0, m_contactsModel->rowCount());

        for (const auto& contact : contacts) {
            QList<QStandardItem*> items;
            items << new QStandardItem(QString::number(contact.id()));
            items << new QStandardItem(contact.firstName());
            items << new QStandardItem(contact.lastName());
            items << new QStandardItem(contact.phone());
            items << new QStandardItem(contact.email());

            m_contactsModel->appendRow(items);
        }
    }
}

void MainWindow::onContactDoubleClicked(const QModelIndex& index)
{
    onEditContact();
}

void MainWindow::showContactDialog(const Contact& contact)
{
    ContactDialog dialog(contact, this);
    if (dialog.exec() == QDialog::Accepted) {
        Contact newContact = dialog.getContact();

        if (contact.id() == 0) {
            m_controller->addContact(
                newContact.firstName(), newContact.lastName(), newContact.phone(),
                newContact.email(),
                newContact.notes()
                );
        } else {
            newContact.setId(contact.id());
            m_controller->updateContact(
                newContact.id(), newContact.firstName(), newContact.lastName(), newContact.phone(),
                newContact.email(), newContact.notes()
                );
        }
    }
}

void MainWindow::onContactAdded(const Contact& contact)
{
    refreshContacts();
    QMessageBox::information(this, "Success", "Contact added successfully");
}

void MainWindow::onContactUpdated(const Contact& contact)
{
    refreshContacts();
    QMessageBox::information(this, "Success", "Contact updated successfully");
}

void MainWindow::onContactDeleted(int id)
{
    refreshContacts();
    QMessageBox::information(this, "Success", "Contact deleted successfully");
}

void MainWindow::onErrorOccurred(const QString& error)
{
    QMessageBox::critical(this, "Error", error);
}
