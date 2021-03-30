#ifndef GUI_ABOUT_ABOUTDIALOG_H
#define GUI_ABOUT_ABOUTDIALOG_H

#include "GUI/Common.h"

class Ui_AboutDialog;

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget* parent = nullptr);
    ~AboutDialog() override;

private:
    std::unique_ptr<Ui_AboutDialog> mUi;

    Q_SLOT void on_aboutQtButton_clicked();
    Q_SLOT void on_thirdPartyButton_clicked();

    DISABLE_COPY(AboutDialog);
};

#endif
