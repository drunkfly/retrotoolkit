#ifndef GUI_MAINWINDOW_H
#define GUI_MAINWINDOW_H

#include "Common/Common.h"
#include <QMainWindow>
#include <memory>

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
    QComboBox* mConfigCombo;
    BuildStatusLabel* mStatusLabel;

    void setProject(const QString& file, std::unique_ptr<Project> project);
    bool buildProject();

    void updateUi();
    void updateConfigCombo();

    Q_SLOT void on_actionNewProject_triggered();
    Q_SLOT void on_actionOpenProject_triggered();
    Q_SLOT void on_actionBuild_triggered();

    Q_SLOT void on_actionAbout_triggered();

    DISABLE_COPY(MainWindow);
};

#endif
