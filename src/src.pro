TEMPLATE=app

QT -= gui
QT += core

CONFIG += link_pkgconfig

PKGCONFIG += libkcalcoren-qt5 libical

HEADERS=\
  timezone.h

SOURCES=\
  main.cpp \
  timezone.cpp
