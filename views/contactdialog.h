#ifndef CONTACTDIALOG_H
#define CONTACTDIALOG_H

#include <QDialog>
#include "../models/contact.h"

namespace Ui {
class ContactDialog;
}

class ContactDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ContactDialog(const Contact& contact = Contact(), QWidget *parent = nullptr);
    ~ContactDialog();

    Contact getContact() const;

private slots:
    void onSaveButtonClicked();
    void onCancelButtonClicked();

private:
    Ui::ContactDialog *ui;
    Contact m_contact;
};

#endif // CONTACTDIALOG_H
