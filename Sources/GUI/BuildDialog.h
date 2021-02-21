#ifndef GUI_BUILDDIALOG_H
#define GUI_BUILDDIALOG_H

#include "Common/Common.h"
#include "Compiler/Compiler.h"
#include <QDialog>
#include <memory>

class QThread;
class SourceLocation;
class Ui_BuildDialog;

class BuildThread : public QObject, public ICompilerListener
{
    Q_OBJECT

public:
    explicit BuildThread(const QString& projectFile, QObject* parent = nullptr);
    ~BuildThread() override;

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
    QAtomicInt mCancelRequested;
    QString mProjectFile;

    void compilerProgress(int current, int total, const std::string& message) override;

    DISABLE_COPY(BuildThread);
};

class BuildDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BuildDialog(const QString& projectFile, QWidget* parent = nullptr);
    ~BuildDialog() override;

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
    QString mProjectFile;
    QThread* mThread;

    Q_SIGNAL void cancelRequested();

    DISABLE_COPY(BuildDialog);
};

#endif
