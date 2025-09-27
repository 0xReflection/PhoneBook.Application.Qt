#include "PhoneBookAppQt.h"
#include "../Repository/SqlServerContactRepository.h"
#include "../Infrastructure/DatabaseConnection.h"
#include "../Infrastructure/ConnectionStringBuilder.h"
#include "../Service/ContactService.h"
#include <QMessageBox>
#include <QShortcut>

PhoneBookAppQt::PhoneBookAppQt(QWidget* parent)
    : QMainWindow(parent) {
    ui.setupUi(this);

    try {
        auto connStr = ConnectionStringBuilder::buildForSqlServer();
        auto dbConnection = std::make_unique<DatabaseConnection>(connStr);
        auto repository = std::make_unique<SqlServerContactRepository<Contact>>(std::move(dbConnection));
        auto service = std::make_shared<ContactService<Contact, SqlServerContactRepository<Contact>>>(std::move(repository));

        m_viewModel = std::make_unique<ContactViewModel>(service);
        m_model = std::make_unique<PhonebookModel>(service);

        ui.tableView->setModel(m_model.get());
        ui.tableView->horizontalHeader()->setStretchLastSection(true);

        setupConnections();
    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "Ошибка", QString("Инициализация не удалась:\n%1").arg(e.what()));
    }
}

PhoneBookAppQt::~PhoneBookAppQt() {}

void PhoneBookAppQt::setupConnections() {
    connect(ui.addButton, &QPushButton::clicked, this, [this]() {
        if (m_viewModel->addContact(ui.phoneEdit->text(), ui.nameEdit->text())) {
            ui.phoneEdit->clear();
            ui.nameEdit->clear();
            m_model->refresh();
            ui.statusLabel->setText("Контакт добавлен");
        }
        else {
            ui.statusLabel->setText("Ошибка добавления");
        }
        });

    connect(ui.removeButton, &QPushButton::clicked, this, [this]() {
        int row = ui.tableView->currentIndex().row();
        if (row >= 0 && m_model->removeContact(row)) {
            m_model->refresh();
            ui.statusLabel->setText("Контакт удалён");
        }
        else {
            ui.statusLabel->setText("Ошибка удаления");
        }
        });

    connect(ui.searchButton, &QPushButton::clicked, this, [this]() {
        ui.statusLabel->setText("🔍 " + m_viewModel->findContact(ui.searchEdit->text()));
        });

    connect(m_viewModel.get(), &ContactViewModel::contactCountChanged, this, [this]() {
        ui.statusLabel->setText(QString("Всего контактов: %1").arg(m_viewModel->getContactCount()));
        });

    new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_F), this, [this]() { ui.searchEdit->setFocus(); });
    new QShortcut(QKeySequence(Qt::Key_Delete), this, [this]() { ui.removeButton->click(); });
}
