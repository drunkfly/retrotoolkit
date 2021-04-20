#ifndef GUI_BUILDDIALOG_H
#define GUI_BUILDDIALOG_H

#include "GUI/Common.h"
#include "Compiler/Compiler.h"

class QThread;
class CompiledOutput;
class SourceLocation;
class Emulator;
class IOutputWriterProxy;
class Ui_BuildDialog;
struct SnapshotState;

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

class BuildDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BuildDialog(const QString& projectFile, std::string projectConfiguration, QWidget* parent = nullptr);
    ~BuildDialog() override;

    CompiledOutput* linkerOutput() const { return mLinkerOutput; }
    const std::optional<std::filesystem::path>& generatedWavFile() const { return mGeneratedWavFile; }

    void setEnableWav(bool flag) { mEnableWav = flag; }
    void setEmulator(std::shared_ptr<Emulator> emulator);

    int exec() override;

signals:
    void success();
    void failure(QString file, int line, QString message);
    void message(std::string message);
    void canceled();

protected:
    void done(int result) override;

private:
    class OutputProxy;

    std::unique_ptr<Ui_BuildDialog> mUi;
    GCHeap mHeap;
    QString mProjectFile;
    CompiledOutput* mLinkerOutput;
    std::shared_ptr<OutputProxy> mOutputProxy;
    std::optional<std::filesystem::path> mGeneratedWavFile;
    QThread* mThread;
    bool mEnableWav;

    Q_SIGNAL void cancelRequested();

    DISABLE_COPY(BuildDialog);
};

#endif
