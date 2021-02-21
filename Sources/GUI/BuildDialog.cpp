#include "BuildDialog.h"
#include "Compiler/Compiler.h"
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

BuildThread::BuildThread(QObject* parent)
    : QObject(parent)
    , mCancelRequested(0)
{
}

BuildThread::~BuildThread()
{
}

void BuildThread::compile()
{
    try {
        Compiler compiler;
        /* FIXME */
        for (int i = 0; i < 20; i++) {
            QThread::msleep(100);
            checkCancelation();
            emit progress(i+1, 20, QStringLiteral("Step %1").arg(i+1));
        }
    } catch (const Canceled&) {
        emit canceled();
        return;
    } catch (const CompilerError& e) {
        QString file = (e.location() && e.location()->file() ? fromPath(e.location()->file()->path()) : QString());
        int line = (e.location() ? e.location()->line() : 0);
        emit failure(file, line, fromUtf8(e.message()));
        return;
    } catch (const std::exception& e) {
        emit failure(QString(), 0, QString::fromUtf8(e.what()));
        return;
    }

    emit success();
}

void BuildThread::checkCancelation() const
{
    if (mCancelRequested.loadAcquire() != 0)
        throw Canceled();
}

void BuildThread::requestCancel()
{
    mCancelRequested.storeRelease(1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BuildDialog::BuildDialog(QWidget* parent)
    : QDialog(parent)
    , mUi(new Ui_BuildDialog)
{
    mUi->setupUi(this);
    mUi->progressBar->setRange(0, 0);
    setWindowModality(Qt::ApplicationModal);

    mThread = QThread::create([this]() {
            BuildThread thread;

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
}

void BuildDialog::done(int result)
{
    if (mThread && mThread->isRunning()) {
        emit cancelRequested();
        return;
    }

    QDialog::done(result);
}
