#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QBasicTimer>
#include <QElapsedTimer>
#include <QFile>
#include <QMessageBox>
#include <QDateTime>
#include <QTime>
#include <QTextStream>

#include <math.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QTime timeTo(QDateTime a, QDateTime b) { return QTime::fromMSecsSinceStartOfDay(a.msecsTo(b)); }
    QTime getRaceTime(QDateTime clock) {
        qint64 ms = mRaceStart.msecsTo(clock);
        ms -= mPauseMsecs;
        return QTime::fromMSecsSinceStartOfDay(ms);
    }
    QTime getLapTime(QDateTime clock) {
        qint64 lapms = mLapStart.msecsTo(clock);
        lapms -= mPauseMsecsLastLap;
        return QTime::fromMSecsSinceStartOfDay(lapms);
    }
    int getLapSecs(QDateTime clock) { return mLapStart.secsTo(clock); }

private slots:
    void on_pushButton_startPause_clicked();
    void on_pushButton_stop_clicked();
    void on_pushButton_lap_pressed();

private:
    const int LAP_METRES = 50; // Length of one lap in metres

    Ui::MainWindow *ui;

    QBasicTimer guiUpdateTimer;
    void timerEvent(QTimerEvent *event);

    enum RaceState {STATE_NOT_STARTED, STATE_ACTIVE, STATE_PAUSED, STATE_STOPPED};
    RaceState mRaceState = STATE_NOT_STARTED;
    QDateTime mRaceStart;
    QDateTime mLapStart;
    QDateTime mLastKmStart;
    int mLapCount = 0;
    bool mNewLap = false;
    QDateTime mPauseStart;
    qint64 mPauseMsecs = 0;
    qint64 mPauseMsecsLastLap = 0;
    qint64 mPauseMsecsLastKm = 0;

    // Stats
    int mMinLapSecs = 0;
    int mMaxLapSecs = 0;

    void startRace();
    void stopRace();
    void pauseRace(bool pause);
    void tap();

    QFile mLogFile;
    void openLogFile();
    void closeLogFile();
    void log(QDateTime clockTime, QTime raceTime, QTime lapTime, int laps, float km);
};

#endif // MAINWINDOW_H


