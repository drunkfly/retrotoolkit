#ifndef GUI_PLAYAUDIODIALOG_H
#define GUI_PLAYAUDIODIALOG_H

#include "GUI/Common.h"

class Ui_PlayAudioDialog;
class QLabel;

class PlayAudioDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlayAudioDialog(std::filesystem::path path, QWidget* parent = nullptr);
    ~PlayAudioDialog() override;

private:
    std::unique_ptr<Ui_PlayAudioDialog> mUi;
    QMediaPlayer* mPlayer;
    std::filesystem::path mPath;
    bool mDurationKnown;

    Q_SLOT void on_restartButton_clicked();

    void onPositionChanged(qint64 position);
    void onDurationChanged(qint64 duration);
    void onStateChanged(QMediaPlayer::State state);

    static void setLabelTime(QLabel* label, qint64 time);

    Q_DISABLE_COPY(PlayAudioDialog)
};

#endif
