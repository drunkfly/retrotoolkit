#ifndef GUI_UTIL_CONVERSION_H
#define GUI_UTIL_CONVERSION_H

#include <QString>
#include <filesystem>

QByteArray toByteArray(const std::string& str);

QString fromUtf8(const std::string& str);
std::string toUtf8(const QString& str);

std::filesystem::path toPath(const QString& path);
QString fromPath(const std::filesystem::path& path);

#endif
