#include "PlayAudioDialog.h"
#include "GUI/Util/Conversion.h"
#include "ui_PlayAudioDialog.h"

PlayAudioDialog::PlayAudioDialog(std::filesystem::path path, QWidget* parent)
    : QDialog(parent)
    , mUi(new Ui_PlayAudioDialog)
    , mPlayer(nullptr)
    , mPath(std::move(path))
    , mDurationKnown(false)
{
    mUi->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);

    on_restartButton_clicked();
}

PlayAudioDialog::~PlayAudioDialog()
{
}

void PlayAudioDialog::on_restartButton_clicked()
{
    delete mPlayer;

    mUi->progressBar->setRange(0, 100);
    mUi->progressBar->setValue(0);

    mPlayer = new QMediaPlayer(this);
    connect(mPlayer, &QMediaPlayer::positionChanged, this, &PlayAudioDialog::onPositionChanged);
    connect(mPlayer, &QMediaPlayer::durationChanged, this, &PlayAudioDialog::onDurationChanged);
    connect(mPlayer, &QMediaPlayer::stateChanged, this, &PlayAudioDialog::onStateChanged);
    mPlayer->setVolume(100);
    mPlayer->setNotifyInterval(100);
    mPlayer->setMedia(QUrl::fromLocalFile(fromPath(mPath)));
    mPlayer->play();
}

void PlayAudioDialog::onPositionChanged(qint64 position)
{
    setLabelTime(mUi->positionLabel, position);
    if (mDurationKnown) {
        qint64 duration = mPlayer->duration();
        mUi->progressBar->setRange(0, mUi->progressBar->width());
        mUi->progressBar->setValue(duration == 0 ? 0 : int(position * mUi->progressBar->maximum() / duration));
    }
}

void PlayAudioDialog::onDurationChanged(qint64 duration)
{
    setLabelTime(mUi->durationLabel, duration);
    mDurationKnown = true;
}

void PlayAudioDialog::onStateChanged(QMediaPlayer::State state)
{
    if (state != QMediaPlayer::PlayingState)
        reject();
}

void PlayAudioDialog::setLabelTime(QLabel* label, qint64 time)
{
    time /= 1000;

    int h = time / 3600;
    int m = (time % 3600) / 60;
    int s = (time % 3600) % 60;

    const QChar zero('0');
    label->setText(QStringLiteral("%1:%2:%3").arg(h, 2, 10, zero).arg(m, 2, 10, zero).arg(s, 2, 10, zero));
}
