#include "BuildThread.h"
#include "Compiler/Java/JVM.h"
#include "Compiler/CompilerError.h"
#include "GUI/Settings.h"
#include "GUI/Util/Conversion.h"

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
    , mOutputProxy(nullptr)
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
                compiler.setJdkPath(toPath(jdkPath));
            JVM::setVerboseGC(settings.jdkVerboseGC);
            JVM::setVerboseClass(settings.jdkVerboseClass);
            JVM::setVerboseJNI(settings.jdkVerboseJNI);
            compiler.setEnableWav(mEnableWav);
            compiler.setOutputWriterProxy(mOutputProxy.get());
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
    } catch (...) {
        emit failure(QString(), 0, tr("Internal compiler error."));
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
  #ifndef NDEBUG
   #ifdef _WIN32
    OutputDebugStringA(text.c_str());
   #else
    printf("%s", text.c_str());
   #endif
  #endif

    emit message(std::move(text));
}
