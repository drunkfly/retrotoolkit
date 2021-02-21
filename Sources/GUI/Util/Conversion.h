#ifndef GUI_UTIL_CONVERSION_H
#define GUI_UTIL_CONVERSION_H

#include <QString>
#include <filesystem>

QString fromUtf8(const std::string& str);

std::filesystem::path toPath(const QString& path);
QString fromPath(const std::filesystem::path& path);

#endif
