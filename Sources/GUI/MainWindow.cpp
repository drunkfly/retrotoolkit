#include "MainWindow.h"
#include "GUI/AboutDialog.h"
#include "GUI/BuildDialog.h"
#include "GUI/Settings.h"
#include "GUI/Widgets/BuildStatusLabel.h"
#include "GUI/Util/Exception.h"
#include "GUI/Util/Conversion.h"
#include "GUI/Util/ComboBox.h"
#include "Compiler/Project.h"
#include "Compiler/Tree/SourceLocation.h"
#include "ui_MainWindow.h"
#include <QProcess>
#include <QMessageBox>
#include <QFileInfo>
#include <QFileDialog>
#include <QComboBox>

MainWindow::MainWindow()
    : mUi(new Ui_MainWindow)
{
    mUi->setupUi(this);

    mUi->menuView->addAction(mUi->outputDockWidget->toggleViewAction());
    mUi->outputDockWidget->hide();

    mConfigCombo = new QComboBox(this);
    mUi->toolBar->insertWidget(mUi->actionBuild, mConfigCombo);

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
        project->load(toPath(file));
    } CATCH(e) {
        e.show(this);
        return;
    }

    setProject(file, std::move(project));
}

void MainWindow::setProject(const QString& file, std::unique_ptr<Project> project)
{
    if (mProjectFile) {
        if (!QProcess::startDetached(QApplication::applicationFilePath(), QStringList() << file))
            QMessageBox::critical(this, tr("Error"), tr("Unable to launch new instance of the application."));
    } else {
        mProjectFile = std::make_unique<QString>(file);
        mProject = std::move(project);
        setWindowTitle(QStringLiteral("%1[*] - %2").arg(QFileInfo(file).completeBaseName()).arg(windowTitle()));
        updateConfigCombo();
        updateUi();
    }
}

bool MainWindow::buildProject()
{
    mUi->outputWidget->clear();
    mStatusLabel->setBuildStatus(tr("Building..."));

    BuildDialog dlg(*mProjectFile, comboSelectedItem(mConfigCombo).toByteArray().toStdString(), this);
    connect(&dlg, &BuildDialog::success, mStatusLabel, &BuildStatusLabel::clearBuildStatus);
    connect(&dlg, &BuildDialog::canceled, mStatusLabel, &BuildStatusLabel::clearBuildStatus);
    connect(&dlg, &BuildDialog::failure, mStatusLabel, &BuildStatusLabel::setBuildError);
    connect(&dlg, &BuildDialog::message, this, [this](const QString& message) {
            mUi->outputWidget->print(message);
            mUi->outputDockWidget->show();
            mUi->outputDockWidget->raise();
        });

    if (dlg.exec() != QDialog::Accepted)
        return false;

    mUi->memoryMapWidget->setData(dlg.linkerOutput());
    return true;
}

void MainWindow::updateUi()
{
    mUi->actionBuild->setEnabled(mProjectFile != nullptr);
    mConfigCombo->setEnabled(mProjectFile != nullptr);
}

void MainWindow::updateConfigCombo()
{
    QVariant selected = comboSelectedItem(mConfigCombo);
    mConfigCombo->clear();

    if (mProject) {
        for (const auto& config : mProject->configurations)
            mConfigCombo->addItem(fromUtf8(config->name), toByteArray(config->name));

        if (!selected.isValid() || !comboSelectItem(mConfigCombo, selected))
            mConfigCombo->setCurrentIndex(0);
    }
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
        project->save(toPath(file), true);
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
