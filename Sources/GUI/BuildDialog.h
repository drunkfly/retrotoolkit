#ifndef GUI_BUILDDIALOG_H
#define GUI_BUILDDIALOG_H

#include "Common/Common.h"
#include "Common/GC.h"
#include "Compiler/Compiler.h"
#include <QDialog>
#include <memory>

class QThread;
class CompiledOutput;
class SourceLocation;
class Ui_BuildDialog;

class BuildThread : public QObject, public ICompilerListener
{
    Q_OBJECT

public:
    BuildThread(GCHeap* heap, const QString& projectFile, std::string projectConfiguration, QObject* parent = nullptr);
    ~BuildThread() override;

    CompiledOutput* linkerOutput() const { return mLinkerOutput; }
    const std::optional<std::filesystem::path>& generatedWavFile() const { return mGeneratedWavFile; }

    void setEnableWav(bool flag) { mEnableWav = flag; }

    void compile();

    void requestCancel();

signals:
    void success();
    void failure(QString file, int line, QString message);
    void progress(int current, int total, QString message);
    void message(QString message);
    void canceled();

protected:
    void checkCancelation() const override;

private:
    GCHeap* mHeap;
    QAtomicInt mCancelRequested;
    QString mProjectFile;
    std::string mProjectConfiguration;
    std::optional<std::filesystem::path> mGeneratedWavFile;
    CompiledOutput* mLinkerOutput;
    bool mEnableWav;

    void compilerProgress(int current, int total, const std::string& message) override;
    void printMessage(const std::string& text) override;

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

    int exec() override;

signals:
    void success();
    void failure(QString file, int line, QString message);
    void message(QString message);
    void canceled();

protected:
    void done(int result) override;

private:
    class Thread;

    std::unique_ptr<Ui_BuildDialog> mUi;
    GCHeap mHeap;
    QString mProjectFile;
    CompiledOutput* mLinkerOutput;
    std::optional<std::filesystem::path> mGeneratedWavFile;
    QThread* mThread;
    bool mEnableWav;

    Q_SIGNAL void cancelRequested();

    DISABLE_COPY(BuildDialog);
};

#endif
