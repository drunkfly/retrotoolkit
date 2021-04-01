#ifndef GUI_SETTINGSDIALOG_H
#define GUI_SETTINGSDIALOG_H

#include "GUI/Common.h"

class Ui_SettingsDialog;
class QLabel;

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget* parent = nullptr);
    ~SettingsDialog() override;

    void done(int r) override;

private:
    enum class JdkError : int;
    std::unique_ptr<Ui_SettingsDialog> mUi;

    Q_SLOT void on_tabWidget_currentChanged(int index);

    Q_SLOT void on_jdkPathEdit_textChanged(const QString&);
    Q_SLOT void on_jdkBrowseButton_clicked();
    Q_SLOT void on_jdkVerboseGC_toggled(bool);
    Q_SLOT void on_jdkVerboseClass_toggled(bool);
    Q_SLOT void on_jdkVerboseJNI_toggled(bool);

    void updateJdkError();
    void setJdkError(JdkError error);

    Q_DISABLE_COPY(SettingsDialog)
};

#endif
