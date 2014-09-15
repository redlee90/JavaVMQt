#-------------------------------------------------
#
# Project created by QtCreator 2014-09-11T15:31:41
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = JavaVMQt
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
    opcodes.cpp \
    ObjectHeap.cpp \
    JavaClass.cpp \
    ExecutionEngine.cpp \
    MethodArea.cpp \
    ClassLoader.cpp \
    JVM.cpp

HEADERS += \
    types.h \
    opcodes.h \
    ObjectHeap.h \
    JavaClass.h \
    ExecutionEngine.h \
    constants.h \
    Object.h \
    Frame.h \
    MethodArea.h \
    ClassLoader.h
