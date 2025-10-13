#include "contact.h"
#include <QDebug>
Contact::Contact(int id, const QString& firstName, const QString& lastName,
                 const QString& phone, const QString& email,
                 const QString& notes ) : m_id(id), m_firstName(firstName),
                m_lastName(lastName), m_phone(phone), m_email(email), m_notes(notes)
{
}

bool Contact::isValid() const
{
    bool firstValid = !m_firstName.trimmed().isEmpty();
    bool lastValid = !m_lastName.trimmed().isEmpty();
    bool phoneValid = !m_phone.trimmed().isEmpty();

    bool result = firstValid && lastValid && phoneValid;
    return result;
    //return !m_firstName.isEmpty() && !m_lastName.isEmpty() && m_phone.isEmpty();
}
