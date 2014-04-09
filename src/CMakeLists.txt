file(GLOB QML_SRC qml/*.qml qml/*.js)

set(litteras_SRCS
    ${QML_SRC}
    AccountNew.cpp
    Accounts.cpp
#    highlighter.cpp
#    htmleditor.cpp
#    NewMail.cpp
    MessagesModel.cpp
    EwsFolderModel.cpp
    EwsEngine.cpp
    AccountsEngine.cpp
#    NoSelectionRectDelegate.cpp
#    MainUI.cpp
#    Litteras.cpp
    iconprovider.cpp
    foldermodel.cpp
    messagesindex.cpp
    main.cpp
)

qt5_add_resources(litteras_SRCS
    application.qrc
    OPTIONS -compress 3 -threshold 3
)

add_executable(litteras
    ${litteras_SRCS}
)
target_link_libraries(litteras
    ${Qt5Widgets_LIBRARIES}
    ${Qt5Quick_LIBRARIES}
    ${Qt5Multimedia_LIBRARIES}
    ${Qt5DBus_LIBRARIES}
    ${Qt5Xml_LIBRARIES}
    ${Qt5WebKit_LIBRARIES}
    ${Qt5WebKitWidgets_LIBRARIES}
    ${XAPIAN_LIBRARIES}
    ${EwsQt5_LIBRARIES}
)

#install(PROGRAMS litteras DESTINATION ${BIN_INSTALL_DIR})
#install(FILES litteras.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
