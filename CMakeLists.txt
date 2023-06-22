cmake_minimum_required(VERSION 3.16)

project(ModxLinguist VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.2 COMPONENTS Quick Sql REQUIRED)

qt_add_executable(appModxLinguist
    src/main.cpp
    src/databasecomponent.h
    src/databasecomponent.cpp
    src/lexiconprocessor.h
    src/lexiconprocessor.cpp
    src/lexiconworker.h
    src/lexiconworker.cpp
    resources/resources.qrc
)

qt_add_qml_module(appModxLinguist
    URI ModxLinguist
    VERSION 1.0
    QML_FILES
        qml/main.qml
        qml/DialogAddLanguage.qml
        qml/DialogDeleteLanguage.qml
        qml/DialogAddTopic.qml
        qml/DialogEditTopic.qml
        qml/DialogDeleteTopic.qml
        qml/DialogAddTranslations.qml
        qml/DialogEditTranslations.qml
        qml/DialogMoveTranslations.qml
        qml/DialogMoveSelectedTranslations.qml
        qml/DialogDeleteTranslations.qml
        qml/DialogDeleteSelectedTranslations.qml
        qml/DialogAbout.qml
        qml/DialogEditPackage.qml
        qml/Translation.qml
        qml/CustomTextField.qml
        qml/CustomButton.qml
        qml/CustomComboBox.qml
        qml/CustomTextArea.qml
        qml/CustomLabel.qml
        qml/MessagePopup.qml
)

set_target_properties(appModxLinguist PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appModxLinguist
    PRIVATE Qt6::Quick
    PRIVATE Qt6::Sql)

install(TARGETS appModxLinguist
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
