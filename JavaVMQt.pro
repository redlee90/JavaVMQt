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


SOURCES += main.cpp \
    opcodes.cpp \
    ObjectHeap.cpp \
    NativeMethods.cpp \
    jvm.cpp \
    JavaClass.cpp \
    GarbageCollector.cpp \
    ExecutionEngine.cpp \
    ClassArea.cpp

HEADERS += \
    types.h \
    opcodes.h \
    ObjectHeap.h \
    NativeMethods.h \
    JavaClass.h \
    GarbageCollector.h \
    ExecutionEngine.h \
    constants.h \
    Object.h \
    Frame.h \
    ClassArea.h
