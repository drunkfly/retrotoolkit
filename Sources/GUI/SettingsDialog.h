#ifndef GUI_SETTINGSDIALOG_H
#define GUI_SETTINGSDIALOG_H

#include <QDialog>
#include <QMediaPlayer>
#include <memory>
#include <filesystem>

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
    std::unique_ptr<Ui_SettingsDialog> mUi;

    Q_SLOT void on_tabWidget_currentChanged(int index);

    Q_SLOT void on_jdkPathEdit_textChanged(const QString& newText);
    Q_SLOT void on_jdkBrowseButton_clicked();

    Q_DISABLE_COPY(SettingsDialog)
};

#endif
