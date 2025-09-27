#pragma once
#include <QObject>
#include <QString>
#include <memory>
#include "../Service/ContactService.h"
#include "../Domain/Contacts.h"

class ContactViewModel : public QObject {
    Q_OBJECT
        Q_PROPERTY(int contactCount READ getContactCount NOTIFY contactCountChanged)

private:
    std::shared_ptr<IContactService<Contact>> m_service;

public:
    explicit ContactViewModel(std::shared_ptr<IContactService<Contact>> service, QObject* parent = nullptr)
        : QObject(parent), m_service(std::move(service))
    {
        if (m_service) {
            m_service->setOnContactAdded([this](const auto&) {
                emit contactCountChanged();
                emit contactsChanged();
                });
            m_service->setOnContactRemoved([this](uint64_t) {
                emit contactCountChanged();
                emit contactsChanged();
                });
        }
    }

    int getContactCount() const {
        return m_service ? static_cast<int>(m_service->getContactsCount()) : 0;
    }

    Q_INVOKABLE bool addContact(const QString& phoneNumber, const QString& fullName) {
        bool ok;
        uint64_t phone = phoneNumber.toULongLong(&ok);
        return ok && m_service && m_service->addContact(phone, fullName.toStdString());
    }

    Q_INVOKABLE bool removeContact(uint64_t phone) {
        return m_service && m_service->removeContact(phone);
    }

    Q_INVOKABLE QString findContact(const QString& phoneNumber) {
        bool ok;
        uint64_t phone = phoneNumber.toULongLong(&ok);
        if (!ok) return "Wrong number";
        if (auto c = m_service->findContactByPhoneNumber(phone))
            return c->toQString();
        return "Not found contact";
    }

signals:
    void contactCountChanged();
    void contactsChanged();
};
