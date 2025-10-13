#include "contactrepository.h"
#include <QtSql/qsqlquery.h>
#include <QtSql/QSqlError>
#include <QDebug>

ContactRepository::ContactRepository(QObject *parent)
    : QObject(parent)
{
}

ContactRepository::~ContactRepository()
{
    if(m_database.isOpen())
    {
        m_database.close();
    }
}

bool ContactRepository::connectToDatabase(const QString& host, const QString& dbName,
                                          const QString& user, const QString& password, int port)
{
    m_database = QSqlDatabase::addDatabase("QODBC");

    // Строка подключения для SQL Server Express
    QString connectionString = QString("DRIVER={SQL Server};SERVER=%1;DATABASE=%2;Trusted_Connection=Yes;")
                                   .arg(host)  // HOME-PC\\SQLEXPRESS
                                   .arg(dbName); // phonebook

    m_database.setDatabaseName(connectionString);

    if (!m_database.open()) {
        emit databaseError(m_database.lastError().text());
        return false;
    }

    return InitializeDatabase();
}

bool ContactRepository::InitializeDatabase()
{
    QSqlQuery query;

    // Исправленный SQL для SQL Server
    QString createTable = R"(
        IF NOT EXISTS (SELECT * FROM sysobjects WHERE name='contacts' AND xtype='U')
        CREATE TABLE contacts(
            id INT IDENTITY(1,1) PRIMARY KEY,
            first_name NVARCHAR(100) NOT NULL,
            last_name NVARCHAR(100) NOT NULL,
            phone NVARCHAR(20) NOT NULL,
            email NVARCHAR(100),
            notes NTEXT,
            created_at DATETIME DEFAULT GETDATE()
        )
    )";

    if (!query.exec(createTable)) {
        emit databaseError(query.lastError().text());
        return false;
    }

    QString createIndexes = R"(
        CREATE INDEX idx_contacts_name ON contacts(first_name, last_name);
        CREATE INDEX idx_contacts_phone ON contacts(phone);
        CREATE INDEX idx_contacts_email ON contacts(email);
    )";

    query.exec(createIndexes);

    return true;
}

std::optional<Contact> ContactRepository::createContact(const Contact& contact)
{
    if(!contact.isValid())
    {
        return std::nullopt;
    }

    QSqlQuery query;

    query.prepare(R"(
        INSERT INTO contacts (first_name, last_name, phone, email, notes)
        OUTPUT INSERTED.id
        VALUES (?, ?, ?, ?, ?)
    )");

    query.addBindValue(contact.firstName());
    query.addBindValue(contact.lastName());
    query.addBindValue(contact.phone());
    query.addBindValue(contact.email());
    query.addBindValue(contact.notes());

    if(!query.exec() || !query.next())
    {
        emit databaseError(query.lastError().text());
        return std::nullopt;
    }

    int newId = query.value(0).toInt();
    Contact newContact = contact;
    newContact.setId(newId);

    emit contactChanged();
    return newContact;
}

std::optional<Contact> ContactRepository::getContactById(int id)
{
    QSqlQuery query;
    query.prepare(R"(
        SELECT id, first_name, last_name, phone, email, notes
        FROM contacts WHERE id = ?
    )");
    query.addBindValue(id);

    if (!query.exec() || !query.next()) {
        return std::nullopt;
    }

    return Contact(
        query.value(0).toInt(),
        query.value(1).toString(),
        query.value(2).toString(),
        query.value(3).toString(),
        query.value(4).toString(),
        query.value(5).toString()
        );
}

QVector<Contact> ContactRepository::getAllContacts()
{
    QVector<Contact> contacts;
    QSqlQuery query(R"(
        SELECT id, first_name, last_name, phone, email, notes
        FROM contacts ORDER BY first_name, last_name
    )");

    while(query.next())
    {
        contacts.append(Contact(
            query.value(0).toInt(),
            query.value(1).toString(),
            query.value(2).toString(),
            query.value(3).toString(),
            query.value(4).toString(),
            query.value(5).toString()
            ));
    }
    return contacts;
}

QVector<Contact> ContactRepository::searchContacts(const QString& searchTerm)
{
    QVector<Contact> contacts;
    QSqlQuery query;
    query.prepare(R"(
        SELECT id, first_name, last_name, phone, email, notes
        FROM contacts
        WHERE first_name LIKE ? OR last_name LIKE ? OR phone LIKE ? OR email LIKE ?
        ORDER BY first_name, last_name
    )");

    QString searchPattern = "%" + searchTerm + "%";
    query.addBindValue(searchPattern);
    query.addBindValue(searchPattern);
    query.addBindValue(searchPattern);
    query.addBindValue(searchPattern);

    if (!query.exec()) {
        emit databaseError(query.lastError().text());
        return contacts;
    }

    while (query.next()) {
        contacts.append(Contact(
            query.value(0).toInt(),
            query.value(1).toString(),
            query.value(2).toString(),
            query.value(3).toString(),
            query.value(4).toString(),
            query.value(5).toString()
            ));
    }

    return contacts;
}

bool ContactRepository::updateContact(const Contact& contact)
{
    QSqlQuery query;

    query.prepare(R"(
        UPDATE contacts
        SET first_name = ?, last_name = ?, phone = ?, email = ?, notes = ?
        WHERE id = ?
    )");

    query.addBindValue(contact.firstName());
    query.addBindValue(contact.lastName());
    query.addBindValue(contact.phone());
    query.addBindValue(contact.email());
    query.addBindValue(contact.notes());
    query.addBindValue(contact.id());

    if (!query.exec()) {
        emit databaseError(query.lastError().text());
        return false;
    }

    emit contactChanged();
    return query.numRowsAffected() > 0;
}

bool ContactRepository::deleteContact(int id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM contacts WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        emit databaseError(query.lastError().text());
        return false;
    }

    emit contactChanged();
    return query.numRowsAffected() > 0;
}

bool ContactRepository::isConnected() const
{
    return m_database.isOpen();
}
