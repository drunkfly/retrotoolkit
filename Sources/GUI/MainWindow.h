#ifndef GUI_MAINWINDOW_H
#define GUI_MAINWINDOW_H

#include <QMainWindow>
#include <memory>

class Ui_MainWindow;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow() override;

private:
    std::unique_ptr<Ui_MainWindow> mUi;

    Q_SLOT void on_actionBuild_triggered();

    Q_SLOT void on_actionAbout_triggered();

    Q_DISABLE_COPY(MainWindow)
};

#endif
