#include "contactcontroller.h"
#include <QDebug>

ContactController::ContactController(ContactRepository* repository, QObject* parent)
    : QObject(parent), m_repository(repository)
{
    connect(m_repository, &ContactRepository::databaseError,
            this, &ContactController::errorOccurred);
}

bool ContactController::addContact(const QString& firstName, const QString& lastName,
                                   const QString& phone, const QString& email,
                                   const QString& notes)
{
    Contact contact(0, firstName, lastName, phone, email, notes);

    if (!contact.isValid()) {
        emit errorOccurred("Invalid contact data");
        return false;
    }

    auto result = m_repository->createContact(contact);
    if (result) {
        emit contactAdded(result.value());
        return true;
    }

    return false;
}

bool ContactController::updateContact(int id, const QString& firstName, const QString& lastName,
                                      const QString& phone, const QString& email,
                                      const QString& notes)
{
    Contact contact(id, firstName, lastName, phone, email, notes);

    if (!contact.isValid()) {
        emit errorOccurred("Invalid contact data");
        return false;
    }

    if (m_repository->updateContact(contact)) {
        emit contactUpdated(contact);
        return true;
    }

    return false;
}

bool ContactController::deleteContact(int id)
{
    if (m_repository->deleteContact(id)) {
        emit contactDeleted(id);
        return true;
    }

    return false;
}

QVector<Contact> ContactController::getAllContacts()
{
    return m_repository->getAllContacts();
}

QVector<Contact> ContactController::searchContacts(const QString& term)
{
    return m_repository->searchContacts(term);
}

std::optional<Contact> ContactController::getContactById(int id)
{
    return m_repository->getContactById(id);
}

bool ContactController::isDatabaseConnected() const
{
    return m_repository->isConnected();
}
