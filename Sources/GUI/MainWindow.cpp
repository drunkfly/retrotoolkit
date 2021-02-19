#include "MainWindow.h"
#include "AboutDialog.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow()
    : mUi(new Ui_MainWindow)
{
    mUi->setupUi(this);
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_actionBuild_triggered()
{
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog dlg(this);
    dlg.exec();
}
