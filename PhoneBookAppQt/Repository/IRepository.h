#pragma once
#include <memory>
#include <vector>
#include "Domain/Contacts.h"

template<typename T>
class IRepository {
public:
    virtual ~IRepository() = default;

    virtual bool add(const T& contact) = 0;
    virtual bool remove(uint64_t phoneNumber) = 0;
    virtual std::unique_ptr<T> findByPhoneNumber(uint64_t phoneNumber) const = 0;
    virtual std::vector<std::unique_ptr<T>> findAll() const = 0;
    virtual bool exists(uint64_t phoneNumber) const = 0;
    virtual size_t getCount() const = 0;
};