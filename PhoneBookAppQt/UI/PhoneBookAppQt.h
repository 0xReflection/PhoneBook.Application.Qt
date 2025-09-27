#pragma once
#include <QtWidgets/QMainWindow>
#include "ui_PhoneBookAppQt.h"
#include "PhonebookModel.h"
#include "ContactViewModel.h"

class PhoneBookAppQt : public QMainWindow {
    Q_OBJECT

public:
    PhoneBookAppQt(QWidget* parent = nullptr);
    ~PhoneBookAppQt();

private:
    Ui::PhoneBookAppQtClass ui;
    std::unique_ptr<ContactViewModel> m_viewModel;
    std::unique_ptr<PhonebookModel> m_model;

    void setupConnections();
};
