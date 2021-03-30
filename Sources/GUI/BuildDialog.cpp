#include "BuildDialog.h"
#include "Compiler/Java/JVM.h"
#include "Compiler/CompilerError.h"
#include "GUI/Settings.h"
#include "GUI/Util/Conversion.h"
#include "ui_BuildDialog.h"

namespace
{
    class Canceled
    {
    };
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BuildThread::BuildThread(GCHeap* heap, const QString& projectFile, std::string projectConfiguration, QObject* parent)
    : QObject(parent)
    , mHeap(heap)
    , mCancelRequested(0)
    , mProjectFile(projectFile)
    , mProjectConfiguration(std::move(projectConfiguration))
    , mLinkerOutput(nullptr)
    , mEnableWav(false)
{
}

BuildThread::~BuildThread()
{
}

void BuildThread::compile()
{
    try {
        Compiler compiler(mHeap, toPath(QApplication::applicationDirPath()), this);
        try {
            Settings settings;
            QString jdkPath = settings.jdkPath;
            if (!jdkPath.isEmpty())
                compiler.setJvmDllPath(JVM::findJvmDll(toPath(jdkPath)));
            compiler.setEnableWav(mEnableWav);
            compiler.buildProject(toPath(mProjectFile), mProjectConfiguration);
            mLinkerOutput = compiler.linkerOutput();
            mGeneratedWavFile = compiler.generatedWavFile();
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

void BuildThread::printMessage(std::string text)
{
    emit message(std::move(text));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
}
