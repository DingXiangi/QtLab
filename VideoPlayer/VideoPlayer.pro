QT       += core gui multimedia multimediawidgets sql network concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    historymodel.cpp \
    main.cpp \
    mainwindow.cpp \
    playlistmodel.cpp

HEADERS += \
    historymodel.h \
    mainwindow.h \
    playlistmodel.h

FORMS += \
    mainwindow.ui

# 部署SQLite驱动插件
win32: {
    SQLITE_PLUGIN = $$[QT_INSTALL_PLUGINS]/sqldrivers/qsqlite*.dll
    !isEmpty(SQLITE_PLUGIN): {
        debug: {
            QMAKE_POST_LINK += $$escape_string("$${QMAKE_COPY} $$SQLITE_PLUGIN $$OUT_PWD/debug/")
        } else: release: {
            QMAKE_POST_LINK += $$escape_string("$${QMAKE_COPY} $$SQLITE_PLUGIN $$OUT_PWD/release/")
        }
    }
}


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
