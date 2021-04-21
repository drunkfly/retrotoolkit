#include "BuildDialog.h"
/*
#include "Emulator/Snapshot.h"
#include "Compiler/Output/IOutputWriterProxy.h"
#include "Compiler/Java/JVM.h"
#include "Compiler/CompilerError.h"
#include "GUI/Settings.h"
#include "GUI/Util/Conversion.h"
*/
#include "ui_BuildDialog.h"

BuildDialog::BuildDialog(const QString& projectFile, std::string projectConfiguration, QWidget* parent)
    : QDialog(parent)
    , mUi(new Ui_BuildDialog)
    , mLinkerOutput(nullptr)
    , mEnableWav(false)
{
    mUi->setupUi(this);
    mUi->progressBar->setRange(0, 0);
    setWindowModality(Qt::ApplicationModal);

    mThread = QThread::create([this, projectFile, configuration = std::move(projectConfiguration)]() mutable {
            BuildThread thread(&mHeap, projectFile, std::move(configuration));
            thread.setEnableWav(mEnableWav);
            thread.setOutputProxy(mOutputProxy);

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

            mLinkerOutput = thread.linkerOutput();
            mGeneratedWavFile = thread.generatedWavFile();
        });
}

BuildDialog::~BuildDialog()
{
    mThread->wait();
}

void BuildDialog::setEmulator(std::shared_ptr<Emulator> emulator)
{
    if (emulator)
        mOutputProxy = std::make_shared<OutputProxy>(std::move(emulator));
    else
        mOutputProxy.reset();
}

int BuildDialog::exec()
{
    mThread->start();
    return QDialog::exec();
}

void BuildDialog::done(int result)
{
    if (mThread && mThread->isRunning()) {
        emit cancelRequested();
        return;
    }

    mThread->wait();
    QDialog::done(result);

    if (mOutputProxy) {
        // FIXME
    }
}
