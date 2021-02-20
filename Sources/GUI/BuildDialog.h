#ifndef GUI_BUILDDIALOG_H
#define GUI_BUILDDIALOG_H

#include "Common/Common.h"
#include <QDialog>
#include <memory>

class QThread;
class SourceLocation;
class Ui_BuildDialog;

class BuildThread : public QObject
{
    Q_OBJECT

public:
    explicit BuildThread(QObject* parent = nullptr);
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
    void checkCancelation() const;

private:
    QAtomicInt mCancelRequested;

    DISABLE_COPY(BuildThread);
};

class BuildDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BuildDialog(QWidget* parent = nullptr);
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
    QThread* mThread;

    Q_SIGNAL void cancelRequested();

    DISABLE_COPY(BuildDialog);
};

#endif
