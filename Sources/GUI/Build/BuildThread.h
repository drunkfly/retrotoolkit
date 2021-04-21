#ifndef GUI_BUILD_BUILDTHREAD_H
#define GUI_BUILD_BUILDTHREAD_H

#include "GUI/Common.h"
#include "Compiler/Compiler.h"

class CompiledOutput;
class IOutputWriterProxy;

class BuildThread : public QObject, public ICompilerListener
{
    Q_OBJECT

public:
    BuildThread(GCHeap* heap, const QString& projectFile, std::string projectConfiguration, QObject* parent = nullptr);
    ~BuildThread() override;

    CompiledOutput* linkerOutput() const { return mLinkerOutput; }
    const std::optional<std::filesystem::path>& generatedWavFile() const { return mGeneratedWavFile; }

    void setEnableWav(bool flag) { mEnableWav = flag; }
    void setOutputProxy(std::shared_ptr<IOutputWriterProxy> proxy) { mOutputProxy = std::move(proxy); }

    void compile();

    void requestCancel();

signals:
    void success();
    void failure(QString file, int line, QString message);
    void progress(int current, int total, QString message);
    void message(std::string message);
    void canceled();

protected:
    void checkCancelation() const override;

private:
    GCHeap* mHeap;
    QAtomicInt mCancelRequested;
    QString mProjectFile;
    std::string mProjectConfiguration;
    std::optional<std::filesystem::path> mGeneratedWavFile;
    std::shared_ptr<IOutputWriterProxy> mOutputProxy;
    CompiledOutput* mLinkerOutput;
    bool mEnableWav;

    void compilerProgress(int current, int total, const std::string& message) override;
    void printMessage(std::string text) override;

    DISABLE_COPY(BuildThread);
};

#endif
