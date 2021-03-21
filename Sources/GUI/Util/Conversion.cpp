#include "Conversion.h"

QByteArray toByteArray(const std::string& str)
{
    return QByteArray(str.c_str(), int(str.size()));
}

QString fromUtf8(const std::string& str)
{
    return QString::fromUtf8(str.c_str(), int(str.length()));
}

std::string toUtf8(const QString& str)
{
    QByteArray utf8 = str.toUtf8();
    return std::string(utf8.constData(), size_t(utf8.length()));
}

std::filesystem::path toPath(const QString& path)
{
  #ifdef _WIN32
    auto* p = reinterpret_cast<const wchar_t*>(path.utf16());
    return std::filesystem::path(p, p + path.size());
  #else
    return std::filesystem::path(path.toStdString());
  #endif
}

QString fromPath(const std::filesystem::path& path)
{
  #ifdef _WIN32
    return QString::fromStdWString(path.generic_wstring());
  #else
    return QString::fromStdString(path.native());
  #endif
}
