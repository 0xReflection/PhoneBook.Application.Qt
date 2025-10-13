#include "MainWindow.h"
#include "../views/ui_MainWindow.h"
#include "../views/contactdialog.h"
#include <QMessageBox>

MainWindow::MainWindow(ContactController* controller, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_controller(controller)
    , m_contactsModel(new QStandardItemModel(this))
    , m_darkTheme(false)
{
    ui->setupUi(this);
    setupUI();
    setupConnections();
    refreshContacts();
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
    connect(ui->refreshButton, &QPushButton::clicked, this, &MainWindow::onRefreshButtonClicked);
    connect(ui->themeButton, &QPushButton::clicked, this, &MainWindow::onThemeButtonClicked);
    connect(ui->searchLineEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
    connect(ui->contactsTableView, &QTableView::doubleClicked, this, &MainWindow::onContactDoubleClicked);

    connect(m_controller, &ContactController::contactAdded, this, &MainWindow::onContactAdded);
    connect(m_controller, &ContactController::contactUpdated, this, &MainWindow::onContactUpdated);
    connect(m_controller, &ContactController::contactDeleted, this, &MainWindow::onContactDeleted);
    connect(m_controller, &ContactController::errorOccurred, this, &MainWindow::onErrorOccurred);
}

void MainWindow::onThemeButtonClicked()
{
    m_darkTheme = !m_darkTheme;

    if (m_darkTheme) {
        // Dark Theme - applied via code
        QString darkStyle = R"(
            QMainWindow { background: #1a202c; color: #e2e8f0; }
            QDialog { background: #1a202c; color: #e2e8f0; }
            QLabel { color: #e2e8f0; }
            QLineEdit, QPlainTextEdit {
                background: #2d3748;
                color: #e2e8f0;
                border: 1px solid #4a5568;
                border-radius: 6px;
                padding: 6px 12px;
            }
            QLineEdit:focus, QPlainTextEdit:focus {
                border-color: #3182ce;
            }
            QTableView {
                background: #2d3748;
                color: #e2e8f0;
                border: 1px solid #4a5568;
                gridline-color: #4a5568;
                selection-background-color: #3182ce;
                alternate-background-color: #2d3748;
            }
            QHeaderView::section {
                background: #3182ce;
                color: white;
                padding: 8px;
                border: none;
                font-weight: 600;
            }
            QPushButton {
                border: none;
                border-radius: 6px;
                padding: 6px 12px;
                font-weight: 600;
                min-height: 32px;
            }
        )";

        // Apply specific button colors
        darkStyle += R"(
            QPushButton[name="addButton"] { background: #3182ce; color: white; }
            QPushButton[name="addButton"]:hover { background: #2c5aa0; }
            QPushButton[name="editButton"] { background: #38a169; color: white; }
            QPushButton[name="editButton"]:hover { background: #2f855a; }
            QPushButton[name="deleteButton"] { background: #e53e3e; color: white; }
            QPushButton[name="deleteButton"]:hover { background: #c53030; }
            QPushButton[name="refreshButton"] { background: #6b46c1; color: white; }
            QPushButton[name="refreshButton"]:hover { background: #553c9a; }
            QPushButton[name="themeButton"] { background: #4a5568; color: white; }
            QPushButton[name="themeButton"]:hover { background: #2d3748; }
            QPushButton[name="saveButton"] { background: #3182ce; color: white; }
            QPushButton[name="saveButton"]:hover { background: #2c5aa0; }
            QPushButton[name="cancelButton"] { background: #4a5568; color: white; }
            QPushButton[name="cancelButton"]:hover { background: #2d3748; }
        )";

        setStyleSheet(darkStyle);
        ui->themeButton->setText("Light Theme");
    } else {
        // Light Theme - reset to default
        setStyleSheet("");
        ui->themeButton->setText("Dark Theme");
    }
}

void MainWindow::onRefreshButtonClicked()
{
    refreshContacts();
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
    if (contact) showContactDialog(contact.value());
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

    if (QMessageBox::question(this, "Confirm Delete",
                              QString("Delete '%1'?").arg(contactName)) == QMessageBox::Yes) {
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
        ui->statusLabel->setText(QString("Found: %1 contacts").arg(contacts.size()));
    }
}

void MainWindow::onContactDoubleClicked(const QModelIndex& index)
{
    onEditContact();
}

void MainWindow::showContactDialog(const Contact& contact)
{
    ContactDialog dialog(contact, this);

    // Apply current theme to dialog
    if (m_darkTheme) {
        dialog.setStyleSheet(R"(
            QDialog { background: #1a202c; color: #e2e8f0; }
            QLabel { color: #e2e8f0; }
            QLineEdit, QPlainTextEdit {
                background: #2d3748;
                color: #e2e8f0;
                border: 1px solid #4a5568;
                border-radius: 6px;
                padding: 6px 12px;
            }
            QLineEdit:focus, QPlainTextEdit:focus {
                border-color: #3182ce;
            }
            QPushButton {
                border: none;
                border-radius: 6px;
                padding: 6px 12px;
                font-weight: 600;
                min-height: 32px;
            }
        )");
    }

    if (dialog.exec() == QDialog::Accepted) {
        Contact newContact = dialog.getContact();
        if (contact.id() == 0) {
            m_controller->addContact(newContact.firstName(), newContact.lastName(),
                                     newContact.phone(), newContact.email(), newContact.notes());
        } else {
            m_controller->updateContact(contact.id(), newContact.firstName(), newContact.lastName(),
                                        newContact.phone(), newContact.email(), newContact.notes());
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
