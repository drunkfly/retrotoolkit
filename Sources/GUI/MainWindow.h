#ifndef GUI_MAINWINDOW_H
#define GUI_MAINWINDOW_H

#include "GUI/Common.h"
#include <QMainWindow>

class QComboBox;
class Project;
class BuildStatusLabel;
class Ui_MainWindow;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow() override;

    void openLastProject();
    void openProject(const QString& file, bool mayLaunchNewInstance = true);

private:
    std::unique_ptr<Ui_MainWindow> mUi;
    std::unique_ptr<QString> mProjectFile;
    std::unique_ptr<Project> mProject;
    std::optional<std::filesystem::path> mGeneratedWavFile;
    QComboBox* mConfigCombo;
    BuildStatusLabel* mStatusLabel;

    void setProject(const QString& file, std::unique_ptr<Project> project);
    bool buildProject(bool generateWav);

    void updateUi();
    void updateConfigCombo();

    void openUsefulLink();

    Q_SLOT void on_actionNewProject_triggered();
    Q_SLOT void on_actionOpenProject_triggered();

    Q_SLOT void on_actionBuild_triggered();
    Q_SLOT void on_actionGenerateWAVFile_triggered();
    Q_SLOT void on_actionPlayWAVFile_triggered();

    Q_SLOT void on_actionSettings_triggered();

    Q_SLOT void on_actionAbout_triggered();

    DISABLE_COPY(MainWindow);
};

#endif
