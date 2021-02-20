#ifndef GUI_WIDGETS_LOGWIDGET_H
#define GUI_WIDGETS_LOGWIDGET_H

#include "Common/Common.h"
#include <QPlainTextEdit>

class LogWidget : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit LogWidget(QWidget* parent = nullptr);
    ~LogWidget() override;

    void print(const char* message);
    void print(const std::string& message);
    void print(const QString& message);

    DISABLE_COPY(LogWidget);
};

#endif
