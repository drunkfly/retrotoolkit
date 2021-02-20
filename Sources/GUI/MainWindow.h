#ifndef GUI_MAINWINDOW_H
#define GUI_MAINWINDOW_H

#include <QMainWindow>
#include <memory>

class Project;
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
    std::unique_ptr<Project> mProject;

    void setProject(const QString& file, std::unique_ptr<Project> project);

    void updateUi();

    Q_SLOT void on_actionNewProject_triggered();
    Q_SLOT void on_actionOpenProject_triggered();
    Q_SLOT void on_actionBuild_triggered();

    Q_SLOT void on_actionAbout_triggered();

    Q_DISABLE_COPY(MainWindow)
};

#endif
