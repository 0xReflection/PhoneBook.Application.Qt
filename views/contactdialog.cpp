#include "contactdialog.h"
#include "../views/ui_ContactDialog.h"
ContactDialog::ContactDialog(const Contact& contact, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ContactDialog)
    , m_contact(contact)
{
    ui->setupUi(this);
    if (contact.id() != 0) {
        setWindowTitle("Edit Contact");
        ui->firstNameEdit->setText(contact.firstName());
        ui->lastNameEdit->setText(contact.lastName());
        ui->phoneEdit->setText(contact.phone());
        ui->emailEdit->setText(contact.email());
        ui->notesEdit->setPlainText(contact.notes());
    } else {
        setWindowTitle("Add New Contact");
    }

    connect(ui->saveButton, &QPushButton::clicked, this, &ContactDialog::onSaveButtonClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &ContactDialog::onCancelButtonClicked);
}

ContactDialog::~ContactDialog()
{
    delete ui;
}

Contact ContactDialog::getContact() const
{
    return m_contact;
}

void ContactDialog::onSaveButtonClicked()
{
    QString firstName = ui->firstNameEdit->text().trimmed();
    QString lastName = ui->lastNameEdit->text().trimmed();
    QString phone = ui->phoneEdit->text().trimmed();

    if (firstName.isEmpty() || lastName.isEmpty() || phone.isEmpty()) {
        return;
    }

    m_contact.setFirstName(firstName);
    m_contact.setLastName(lastName);
    m_contact.setPhone(phone);
    m_contact.setEmail(ui->emailEdit->text().trimmed());
    m_contact.setNotes(ui->notesEdit->toPlainText().trimmed());

    accept();
}

void ContactDialog::onCancelButtonClicked()
{
    reject();
}
