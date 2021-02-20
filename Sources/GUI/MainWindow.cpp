#include "MainWindow.h"
#include "GUI/AboutDialog.h"
#include "GUI/BuildDialog.h"
#include "GUI/Settings.h"
#include "GUI/Widgets/BuildStatusLabel.h"
#include "GUI/Util/Exception.h"
#include "Compiler/Project.h"
#include "Compiler/Tree/SourceLocation.h"
#include "ui_MainWindow.h"
#include <QProcess>
#include <QMessageBox>
#include <QFileInfo>
#include <QFileDialog>

MainWindow::MainWindow()
    : mUi(new Ui_MainWindow)
{
    mUi->setupUi(this);
    mUi->menuView->addAction(mUi->outputDockWidget->toggleViewAction());

    mStatusLabel = new BuildStatusLabel(mUi->statusBar);
    mUi->statusBar->addWidget(mStatusLabel);
    connect(mStatusLabel, &BuildStatusLabel::doubleClicked, this, [](const QString& file, int line) {
            /* FIXME */
        });
}

MainWindow::~MainWindow()
{
}

void MainWindow::openLastProject()
{
    Settings settings;
    if (settings.loadLastProjectOnStart) {
        QString lastProject = settings.lastProjectFile;
        if (!lastProject.isEmpty())
            openProject(lastProject, false);
    }
}

void MainWindow::openProject(const QString& file, bool mayLaunchNewInstance)
{
    Settings settings;
    settings.lastProjectFile = file;

    auto project = std::make_unique<Project>();
    TRY {
        project->load(file.toUtf8().constData());
    } CATCH(e) {
        e.show(this);
        return;
    }

    setProject(file, std::move(project));
}

void MainWindow::setProject(const QString& file, std::unique_ptr<Project> project)
{
    if (mProject) {
        if (!QProcess::startDetached(QApplication::applicationFilePath(), QStringList() << file))
            QMessageBox::critical(this, tr("Error"), tr("Unable to launch new instance of the application."));
    } else {
        mProject = std::move(project);
        setWindowTitle(QStringLiteral("%1[*] - %2").arg(QFileInfo(file).completeBaseName()).arg(windowTitle()));
        updateUi();
    }
}

bool MainWindow::buildProject()
{
    mUi->outputWidget->clear();
    mStatusLabel->setBuildStatus(tr("Building..."));

    BuildDialog dlg(this);
    connect(&dlg, &BuildDialog::success, mStatusLabel, &BuildStatusLabel::clearBuildStatus);
    connect(&dlg, &BuildDialog::canceled, mStatusLabel, &BuildStatusLabel::clearBuildStatus);
    connect(&dlg, &BuildDialog::failure, mStatusLabel, &BuildStatusLabel::setBuildError);
    connect(&dlg, &BuildDialog::message, this, [this](const QString& message) {
            mUi->outputWidget->print(message);
            mUi->outputDockWidget->show();
            mUi->outputDockWidget->raise();
        });

    return (dlg.exec() == QDialog::Accepted);
}

void MainWindow::updateUi()
{
    mUi->actionBuild->setEnabled(mProject != nullptr);
}

void MainWindow::on_actionNewProject_triggered()
{
    Settings settings;
    auto filter = QStringLiteral("%1 (*.%2)").arg(tr("Project files")).arg(Project::FileSuffix);
    QString file = QFileDialog::getSaveFileName(this, tr("Create project"), settings.lastProjectFile, filter);
    if (file.isEmpty())
        return;

    settings.lastProjectFile = file;

    auto project = std::make_unique<Project>();
    TRY {
        project->save(file.toUtf8().constData(), true);
    } CATCH(e) {
        e.show(this);
        return;
    }

    setProject(file, std::move(project));
}

void MainWindow::on_actionOpenProject_triggered()
{
    Settings settings;
    auto filter = QStringLiteral("%1 (*.%2)").arg(tr("Project files")).arg(Project::FileSuffix);
    QString file = QFileDialog::getOpenFileName(this, tr("Open project"), settings.lastProjectFile, filter);
    if (file.isEmpty())
        return;

    openProject(file);
}

void MainWindow::on_actionBuild_triggered()
{
    buildProject();
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog dlg(this);
    dlg.exec();
}
