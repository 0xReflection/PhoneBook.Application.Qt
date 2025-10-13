#ifndef CONTACTCONTROLLER_H
#define CONTACTCONTROLLER_H

#include <QObject>
#include <QVector>
#include "../models/contact.h"
#include "../repositories/contactrepository.h"

class ContactController : public QObject
{
    Q_OBJECT

private:
    ContactRepository* m_repository;

public:
    explicit ContactController(ContactRepository* repository, QObject* parent = nullptr);

    bool addContact(const QString& firstName, const QString& lastName,
                    const QString& phone, const QString& email = "",
                    const QString& notes = "");

    bool updateContact(int id, const QString& firstName, const QString& lastName,
                       const QString& phone, const QString& email = "",
                       const QString& notes = "");

    bool deleteContact(int id);
    QVector<Contact> getAllContacts();
    QVector<Contact> searchContacts(const QString& term);
    std::optional<Contact> getContactById(int id);

    bool isDatabaseConnected() const;

signals:
    void contactAdded(const Contact& contact);
    void contactUpdated(const Contact& contact);
    void contactDeleted(int id);
    void contactsLoaded(const QVector<Contact>& contacts);
    void errorOccurred(const QString& error);
};
#endif // CONTACTCONTROLLER_H
