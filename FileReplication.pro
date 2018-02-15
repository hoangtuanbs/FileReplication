#-------------------------------------------------
#
# Project created by QtCreator 2012-01-20T18:40:23
#
#-------------------------------------------------

QT       += core gui network

TARGET = FileReplication
TEMPLATE = app


SOURCES += main.cpp\
        htv_mainwindow.cpp \
    core/htv_exception.cpp \
    htv_filereplication.cpp \
    core/htv_config.cpp \
    core/htv_directoryiterator.cpp \
    abb_replicationmanager.cpp \
    core/abb_directorymonitor.cpp \
    network/abb_replicationconnection.cpp \
    network/abb_replicationserver.cpp \
    network/abb_message.cpp \
    core/abb_replicationfile.cpp \
    core/abb_replicationdirectory.cpp \
    network/abb_replicationclient.cpp \
    core/abbc_task.cpp \
    core/abb_workerthread.cpp \
    network/abb_replicationconnectionmanager.cpp

HEADERS  += htv_mainwindow.h \
    core/htv_exception.h \
    htv_filereplication.h \
    core/htv_config.h \
    core/htv_directoryiterator.h \
    abb_replicationmanager.h \
    core/abb_directorymonitor.h \
    network/abb_replicationconnection.h \
    network/abb_replicationserver.h \
    network/abb_message.h \
    core/abb_replicationfile.h \
    core/abb_replicationdirectory.h \
    network/abb_replicationclient.h \
    core/abb_task.h \
    core/abb_workerthread.h \
    network/abb_replicationconnectionmanager.h

FORMS    += htv_mainwindow.ui

OTHER_FILES += \
    replication.config













































