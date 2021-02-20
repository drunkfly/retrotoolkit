#ifndef GUI_ABOUT_THIRDPARTYDIALOG_H
#define GUI_ABOUT_THIRDPARTYDIALOG_H

#include "Common/Common.h"
#include <QDialog>
#include <memory>

class Ui_ThirdPartyDialog;

class ThirdPartyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ThirdPartyDialog(QWidget* parent = nullptr);
    ~ThirdPartyDialog() override;

private:
    std::unique_ptr<Ui_ThirdPartyDialog> mUi;

    DISABLE_COPY(ThirdPartyDialog);
};

#endif
