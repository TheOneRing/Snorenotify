if(Qt5Quick_FOUND)
    QT5_ADD_RESOURCES(SNORENOTIFIER_RCS ${CMAKE_CURRENT_SOURCE_DIR}/snore_notification.qrc)

    add_snore_plugin(Snore SOURCES snorenotifier.cpp
                                   notifywidget.cpp
                                   ${SNORENOTIFIER_RCS}
                           TYPE Backend SETTINGS_SOURCES snorenotifiersettings.cpp LIBS Qt5::Quick
                           RESOURCES snore_notification )
endif()
