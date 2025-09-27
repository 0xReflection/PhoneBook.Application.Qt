#pragma once
#include <string>
#include <memory>
#include <QString>

class IContact {
public:
    virtual ~IContact() = default;
    virtual uint64_t getPhoneNumber() const = 0;
    virtual std::string getFullName() const = 0;
    virtual bool isValid() const = 0;
    virtual QString toQString() const = 0;
};

class Contact final : public IContact {
private:
    uint64_t m_phoneNumber;
    std::string m_fullName;

public:
    Contact(uint64_t phoneNumber, std::string fullName)
        : m_phoneNumber(phoneNumber), m_fullName(std::move(fullName)) {
    }

    uint64_t getPhoneNumber() const override { return m_phoneNumber; }
    std::string getFullName() const override { return m_fullName; }

    bool isValid() const override {
        std::string phoneStr = std::to_string(m_phoneNumber);
        return phoneStr.length() == 11 && !m_fullName.empty() && m_fullName.length() > 5;
    }

    QString toQString() const override {
        return QString::fromStdString(m_fullName + " - +" + std::to_string(m_phoneNumber));
    }

    static std::unique_ptr<Contact> create(uint64_t phoneNumber, const std::string& fullName) {
        auto contact = std::make_unique<Contact>(phoneNumber, fullName);
        return contact->isValid() ? std::move(contact) : nullptr;
    }
};
