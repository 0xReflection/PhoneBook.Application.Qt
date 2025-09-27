#pragma once
#include <QAbstractTableModel>
#include <memory>
#include "../Service/ContactService.h"
#include "../Domain/Contacts.h"

class PhonebookModel : public QAbstractTableModel {
    Q_OBJECT

private:
    std::shared_ptr<IContactService<Contact>> m_service;
    std::vector<std::unique_ptr<Contact>> m_contacts;

    void loadContacts() {
        beginResetModel();
        m_contacts = m_service ? m_service->getAllContacts() : std::vector<std::unique_ptr<Contact>>{};
        endResetModel();
    }

public:
    enum Column { FullName, PhoneNumber, ColumnCount };

    explicit PhonebookModel(std::shared_ptr<IContactService<Contact>> service, QObject* parent = nullptr)
        : QAbstractTableModel(parent), m_service(std::move(service))
    {
        loadContacts();
    }

    int rowCount(const QModelIndex& parent = QModelIndex()) const override {
        return parent.isValid() ? 0 : static_cast<int>(m_contacts.size());
    }

    int columnCount(const QModelIndex& parent = QModelIndex()) const override {
        return parent.isValid() ? 0 : ColumnCount;
    }

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || role != Qt::DisplayRole) return {};
        const auto& c = m_contacts[index.row()];
        return (index.column() == FullName)
            ? QString::fromStdString(c->getFullName())
            : QString::number(c->getPhoneNumber());
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override {
        if (role != Qt::DisplayRole || orientation != Qt::Horizontal) return {};
        return section == FullName ? "FullName" : "Phone";
    }

    bool removeContact(int row) {
        if (!m_service || row < 0 || row >= (int)m_contacts.size()) return false;
        return m_service->removeContact(m_contacts[row]->getPhoneNumber());
    }

    void refresh() { loadContacts(); }
};
