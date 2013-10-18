include_directories(${CMAKE_BINARY_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(litteras_SRCS
    AccountNew.cpp
    Accounts.cpp
    highlighter.cpp
    htmleditor.cpp
    NewMail.cpp
    MessagesModel.cpp
    EwsFolderModel.cpp
    EwsEngine.cpp
    AccountsEngine.cpp
    NoSelectionRectDelegate.cpp
    MainUI.cpp
    Litteras.cpp
    main.cpp
)

qt5_wrap_ui(litteras_SRCS
    AccountNew.ui
    Accounts.ui
    htmleditor.ui
    inserthtmldialog.ui
    NewMail.ui
    MainUI.ui
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
