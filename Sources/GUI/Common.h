#ifndef GUI_COMMON_H
#define GUI_COMMON_H

#include "Common/Common.h"
#include "Common/GC.h"

#include <QDialog>
#include <QMessageBox>
#include <QThread>
#include <QSettings>
#include <QApplication>
#include <QProcess>
#include <QFileInfo>
#include <QComboBox>
#include <QLabel>
#include <QHeaderView>
#include <QVariant>
#include <QTreeWidget>
#include <QString>
#include <QObject>

#ifdef _WIN32
 #define WIN32_LEAN_AND_MEAN
 #define NOMINMAX
 #include <windows.h>
#endif

#endif
