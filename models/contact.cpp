#include "contact.h"

Contact::Contact(int id, const QString& firstName, const QString& lastName,
                 const QString& phone, const QString& email,
                 const QString& notes ) : m_id(id), m_firstName(firstName),
                m_lastName(lastName), m_phone(phone), m_email(email), m_notes(notes)
{

}

bool Contact::isValid() const
{
    return !m_firstName.isEmpty() && !m_lastName.isEmpty() && m_phone.isEmpty();
}
