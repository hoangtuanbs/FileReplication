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
    htv_replicationmanager.cpp \
    core/htv_directorymonitor.cpp \
    network/htv_replicationconnection.cpp \
    network/htv_replicationserver.cpp \
    network/htv_message.cpp \
    core/htv_replicationfile.cpp \
    core/htv_replicationdirectory.cpp \
    network/htv_replicationclient.cpp \
    core/htv_task.cpp \
    core/htv_workerthread.cpp \
    network/htv_replicationconnectionmanager.cpp

HEADERS  += htv_mainwindow.h \
    core/htv_exception.h \
    htv_filereplication.h \
    core/htv_config.h \
    core/htv_directoryiterator.h \
    htv_replicationmanager.h \
    core/htv_directorymonitor.h \
    network/htv_replicationconnection.h \
    network/htv_replicationserver.h \
    network/htv_message.h \
    core/htv_replicationfile.h \
    core/htv_replicationdirectory.h \
    network/htv_replicationclient.h \
    core/htv_task.h \
    core/htv_workerthread.h \
    network/htv_replicationconnectionmanager.h

FORMS    += htv_mainwindow.ui

OTHER_FILES += \
    replication.config













































