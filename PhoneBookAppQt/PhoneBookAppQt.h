#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_PhoneBookAppQt.h"

class PhoneBookAppQt : public QMainWindow
{
    Q_OBJECT

public:
    PhoneBookAppQt(QWidget *parent = nullptr);
    ~PhoneBookAppQt();

private:
    Ui::PhoneBookAppQtClass ui;
};

