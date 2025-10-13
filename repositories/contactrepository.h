#ifndef CONTACTREPOSITORY_H
#define CONTACTREPOSITORY_H
#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QVector>
#include <Optional>
#include "../models/contact.h"


class ContactRepository : public QObject
{
    Q_OBJECT
private:
    QSqlDatabase m_database;
    bool InitializeDatabase();
public:
    explicit ContactRepository(QObject *parent = nullptr);
    ~ContactRepository();

    bool connectToDatabase(const QString& host ="HOME-PC\\SQLEXPRESS",
                           const QString& dbName = "phonebook",
                           const QString& user = "",
                           const QString& password ="",
                           int port=5432);

    std::optional<Contact> createContact(const Contact& contact);
    std::optional<Contact> getContactById(int id);
    QVector<Contact> getAllContacts();
    QVector<Contact> searchContacts(const QString& searchTerm);
    bool updateContact(const Contact& contact);
    bool deleteContact(int id);

    bool isConnected() const;

signals:
    void databaseError(const QString& error);
    void contactChanged();
};

#endif // CONTACTREPOSITORY_H
