cmake_minimum_required(VERSION 3.19)
project(PhoneBookApp LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Sql)

qt_standard_project_setup()

qt_add_executable(PhoneBookApp
    WIN32 MACOSX_BUNDLE
    main.cpp




    views/contactdialog.cpp views/contactdialog.h
    repositories/contactrepository.cpp repositories/contactrepository.h
    models/contact.cpp models/contact.h
    controllers/contactcontroller.cpp controllers/contactcontroller.h
    views/mainwindow.h views/mainwindow.cpp views/mainwindow.ui
    views/contactdialog.ui






)

target_link_libraries(PhoneBookApp
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Sql
)

include(GNUInstallDirs)

install(TARGETS PhoneBookApp
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET PhoneBookApp
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
