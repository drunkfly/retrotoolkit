#ifndef GUI_BUILD_BUILDDIALOG_H
#define GUI_BUILD_BUILDDIALOG_H

#include "GUI/Build/BuildThread.h"
#include "GUI/Build/OutputProxy.h"

class QThread;
class CompiledOutput;
class Emulator;
class Ui_BuildDialog;

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
