#include "BuildDialog.h"
#include "Compiler/CompilerError.h"
#include "GUI/Util/Conversion.h"
#include "ui_BuildDialog.h"
#include <QThread>

namespace
{
    class Canceled
    {
    };
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BuildThread::BuildThread(const QString& projectFile, QObject* parent)
    : QObject(parent)
    , mProjectFile(projectFile)
    , mCancelRequested(0)
{
}

BuildThread::~BuildThread()
{
}

void BuildThread::compile()
{
    try {
        Compiler compiler(this);
        try {
            compiler.buildProject(toPath(mProjectFile));
        } catch (const Canceled&) {
            emit canceled();
            return;
        } catch (const CompilerError& e) {
            QString file = (e.location() && e.location()->file() ? fromPath(e.location()->file()->name()) : QString());
            int line = (e.location() ? e.location()->line() : 0);
            emit failure(file, line, fromUtf8(e.message()));
            return;
        }
    } catch (const std::exception& e) {
        emit failure(QString(), 0, QString::fromUtf8(e.what()));
        return;
    }

    emit success();
}

void BuildThread::requestCancel()
{
    mCancelRequested.storeRelease(1);
}

void BuildThread::checkCancelation() const
{
    if (mCancelRequested.loadAcquire() != 0)
        throw Canceled();
}

void BuildThread::compilerProgress(int current, int total, const std::string& message)
{
    checkCancelation();
    emit progress(current, total, fromUtf8(message));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BuildDialog::BuildDialog(const QString& projectFile, QWidget* parent)
    : QDialog(parent)
    , mUi(new Ui_BuildDialog)
{
    mUi->setupUi(this);
    mUi->progressBar->setRange(0, 0);
    setWindowModality(Qt::ApplicationModal);

    mThread = QThread::create([this, projectFile]() {
            BuildThread thread(projectFile);

            connect(this, &BuildDialog::cancelRequested, &thread, &BuildThread::requestCancel, Qt::DirectConnection);

            connect(&thread, &BuildThread::success, this, &BuildDialog::success);
            connect(&thread, &BuildThread::failure, this, &BuildDialog::failure);
            connect(&thread, &BuildThread::message, this, &BuildDialog::message);
            connect(&thread, &BuildThread::canceled, this, &BuildDialog::canceled);

            connect(&thread, &BuildThread::success, this, &BuildDialog::accept);
            connect(&thread, &BuildThread::failure, this, &BuildDialog::reject);
            connect(&thread, &BuildThread::canceled, this, &BuildDialog::reject);

            connect(&thread, &BuildThread::progress, this,
                [this](int current, int total, const QString& message) {
                    mUi->statusLabel->setText(message);
                    mUi->progressBar->setRange(0, total);
                    mUi->progressBar->setValue(current);
                });

            thread.compile();
        });

    mThread->start();
}

BuildDialog::~BuildDialog()
{
    mThread->wait();
}

void BuildDialog::done(int result)
{
    if (mThread && mThread->isRunning()) {
        emit cancelRequested();
        return;
    }

    QDialog::done(result);
}
