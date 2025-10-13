#ifndef CONTACT_H
#define CONTACT_H
#include <QString>
#include <QDate>
class Contact
{
private:
    int m_id;
    QString m_firstName;
    QString m_lastName;
    QString m_phone;
    QString m_email;
    QString m_notes;
public:
    Contact(int id=0,
            const QString& firstName ="",
            const QString& lastName ="",
            const QString& phone="",
            const QString& email="",
            const QString& notes="");

    int id() const {return m_id; }
    QString firstName() const {return m_firstName;}
    QString lastName() const {return m_lastName;}
    QString phone() const {return m_phone;}
    QString email() const {return m_email;}
    QString notes() const {return m_notes;}

    void setId(int id) {m_id =id;}
    void setFirstName(const QString& firstName) {m_firstName = firstName;}
    void setLastName ( const QString& lastName) {m_lastName = lastName;}
    void setPhone(const QString& phone) {m_phone = phone;}
    void setNotes(const QString& notes) {m_notes = notes;}

    bool isValid() const;
};

#endif // CONTACT_H
