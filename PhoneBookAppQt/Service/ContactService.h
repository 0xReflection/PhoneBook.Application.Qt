#pragma once
#include <memory>
#include <vector>
#include <functional>
#include "Repository/IRepository.h"
#include "Domain/Contacts.h"

template<typename T>
class IContactService {
public:
    virtual ~IContactService() = default;

    virtual bool addContact(uint64_t phoneNumber, const std::string& fullName) = 0;
    virtual bool removeContact(uint64_t phoneNumber) = 0;
    virtual std::unique_ptr<T> findContactByPhoneNumber(uint64_t phoneNumber) const = 0;
    virtual std::vector<std::unique_ptr<T>> getAllContacts() const = 0;
    virtual bool contactExists(uint64_t phoneNumber) const = 0;
    virtual size_t getContactsCount() const = 0;
    virtual void setOnContactAdded(std::function<void(const T&)> callback) = 0;
    virtual void setOnContactRemoved(std::function<void(uint64_t)> callback) = 0;
};

template<typename T, typename Repository>
class ContactService final : public IContactService<T> {
private:
    std::unique_ptr<Repository> m_repository;
    std::function<void(const T&)> m_onContactAdded;
    std::function<void(uint64_t)> m_onContactRemoved;

public:
    explicit ContactService(std::unique_ptr<Repository> repository)
        : m_repository(std::move(repository)) {
    }

    bool addContact(uint64_t phoneNumber, const std::string& fullName) override {
        if (auto contact = T::create(phoneNumber, fullName)) {
            if (m_repository->add(*contact)) {
                if (m_onContactAdded) m_onContactAdded(*contact);
                return true;
            }
        }
        return false;
    }

    bool removeContact(uint64_t phoneNumber) override {
        if (m_repository->remove(phoneNumber)) {
            if (m_onContactRemoved) m_onContactRemoved(phoneNumber);
            return true;
        }
        return false;
    }

    std::unique_ptr<T> findContactByPhoneNumber(uint64_t phoneNumber) const override {
        return m_repository->findByPhoneNumber(phoneNumber);
    }

    std::vector<std::unique_ptr<T>> getAllContacts() const override {
        return m_repository->findAll();
    }

    bool contactExists(uint64_t phoneNumber) const override {
        return m_repository->exists(phoneNumber);
    }

    size_t getContactsCount() const override {
        return m_repository->getCount();
    }

    void setOnContactAdded(std::function<void(const T&)> callback) override {
        m_onContactAdded = std::move(callback);
    }

    void setOnContactRemoved(std::function<void(uint64_t)> callback) override {
        m_onContactRemoved = std::move(callback);
    }
};
