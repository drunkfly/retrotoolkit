#include "Exception.h"
#include "GUI/Util/Localization.h"
#include <QMessageBox>

Exception::Exception()
    : Exception(tr("Unhandled C++ exception (no additional information available)"))
{
}

Exception::Exception(const std::bad_alloc&)
    : Exception(tr("Memory allocation failed."))
{
}

Exception::Exception(const std::exception& e)
    : Exception(QStringLiteral("%1").arg(e.what()))
{
}

Exception::Exception(const QString& message)
    : mMessage(message)
{
}

Exception::~Exception()
{
}

void Exception::show(QWidget* parent) const
{
    QMessageBox::critical(parent, tr("Error"), mMessage);
}
