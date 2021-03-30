#ifndef GUI_WIDGETS_BUILDSTATUSLABEL_H
#define GUI_WIDGETS_BUILDSTATUSLABEL_H

#include "GUI/Common.h"

class SourceLocation;

class BuildStatusLabel : public QLabel
{
    Q_OBJECT

public:
    explicit BuildStatusLabel(QWidget* parent = nullptr);
    ~BuildStatusLabel() override;

    void clearBuildStatus();
    void setBuildStatus(const QString& message);
    void setBuildError(const QString& file, int line, const QString& message);

signals:
    void doubleClicked(const QString& file, int line);

protected:
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    QString mFile;
    int mLine;
    bool mClickable;

    DISABLE_COPY(BuildStatusLabel);
};

#endif
