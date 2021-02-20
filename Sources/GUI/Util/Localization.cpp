#include "Localization.h"
#include <QObject>

QString tr(const char* sourceText, const char* disambiguation, int n)
{
    return QObject::tr(sourceText, disambiguation, n);
}
