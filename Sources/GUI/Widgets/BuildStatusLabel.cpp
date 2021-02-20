#include "BuildStatusLabel.h"

BuildStatusLabel::BuildStatusLabel(QWidget* parent)
    : QLabel(parent)
    , mClickable(false)
{
    clearBuildStatus();
}

BuildStatusLabel::~BuildStatusLabel()
{
}

void BuildStatusLabel::mouseDoubleClickEvent(QMouseEvent*)
{
    if (mClickable)
        emit doubleClicked(mFile, mLine);
}

void BuildStatusLabel::clearBuildStatus()
{
    setToolTip(QString());
    setText(tr("Ready"));
    setStyleSheet(QStringLiteral("color: black; font-weight: bold; padding-right: 5px"));
    mClickable = false;
}

void BuildStatusLabel::setBuildStatus(const QString& message)
{
    setToolTip(QString());
    setText(message);
    setStyleSheet(QStringLiteral("color: blue; font-weight: bold; padding-right: 5px"));
    mClickable = false;
}

void BuildStatusLabel::setBuildError(const QString& file, int line, const QString& message)
{
    QString msg;
    if (file.isEmpty()) {
        msg = message;
        mClickable = false;
    } else if (line <= 0) {
        msg = QStringLiteral("%1: %2").arg(file).arg(message);
        mFile = file;
        mLine = 1;
        mClickable = true;
    } else {
        msg = QStringLiteral("%1(%2): %3").arg(file).arg(line).arg(message);
        mFile = file;
        mLine = line;
        mClickable = true;
    }

    setToolTip(msg);
    setText(msg);
    setStyleSheet(QStringLiteral("color: red; font-weight: bold; padding-right: 5px"));
}
