CREATE DATABASE PhoneBookDB;
GO

USE PhoneBookDB;
GO


CREATE TABLE Contacts (
    ContactId BIGINT IDENTITY(1,1) PRIMARY KEY,
    PhoneNumber BIGINT NOT NULL UNIQUE,
    FullName NVARCHAR(255) NOT NULL,
    CreatedAt DATETIME2 DEFAULT GETDATE(),
    UpdatedAt DATETIME2 DEFAULT GETDATE()
);
GO

-- Индекс для быстрого поиска по номеру телефона
CREATE UNIQUE INDEX IX_Contacts_PhoneNumber ON Contacts(PhoneNumber);
GO

CREATE INDEX IX_Contacts_FullName ON Contacts(FullName);
GO


CREATE TRIGGER TR_Contacts_UpdateTimestamp 
ON Contacts 
AFTER UPDATE AS
BEGIN
    UPDATE Contacts 
    SET UpdatedAt = GETDATE() 
    FROM Contacts c 
    INNER JOIN inserted i ON c.ContactId = i.ContactId;
END;
GO


INSERT INTO Contacts (PhoneNumber, FullName) VALUES
(88005553535, N'Иванов Иван Иванович'),
GO


CREATE PROCEDURE sp_SearchContacts
    @SearchTerm NVARCHAR(255) = NULL
AS
BEGIN
    IF @SearchTerm IS NULL OR @SearchTerm = ''
        SELECT * FROM Contacts ORDER BY FullName;
    ELSE
        SELECT * FROM Contacts 
        WHERE FullName LIKE '%' + @SearchTerm + '%' 
           OR CAST(PhoneNumber AS NVARCHAR(20)) LIKE '%' + @SearchTerm + '%'
        ORDER BY FullName;
END;
GO