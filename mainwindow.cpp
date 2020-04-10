#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    guiUpdateTimer.start(100, this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::timerEvent(QTimerEvent* /*event*/)
{
    QDateTime now = QDateTime::currentDateTime();

    // Clock
    ui->label_clock->setText(now.toString("HH:mm"));

    // Status
    QString state;
    switch (mRaceState) {
    case MainWindow::STATE_NOT_STARTED:
        state = "Not Started";
        ui->frame_center->setStyleSheet("background-color: gray;");
        ui->frame_top->setStyleSheet("background-color: white;");
        break;
    case MainWindow::STATE_ACTIVE:
        state = "ACTIVE";
        ui->frame_top->setStyleSheet("background-color: #00FF00;");
        break;
    case MainWindow::STATE_PAUSED:
        state = "PAUSED";
        ui->frame_center->setStyleSheet("background-color: gray;");
        ui->frame_top->setStyleSheet("background-color: gray;");
        break;
    case MainWindow::STATE_STOPPED:
        state = "STOPPED";
        ui->frame_center->setStyleSheet("background-color: gray;");
        ui->frame_top->setStyleSheet("background-color: gray;");
        break;
    }
    ui->label_raceStatus->setText(state);

    if (mRaceState == STATE_ACTIVE) {
        // Race time
        ui->label_raceTime->setText(getRaceTime(now).toString("HH:mm:ss"));

        if (getLapSecs(now) < 5) {
            mNewLap = true;
            ui->frame_center->setStyleSheet("background-color: #00FF00;");
        } else {
            if (mNewLap) {
                // Last lap time
                mNewLap = false;
                ui->label_lastLapTime->setText(ui->label_lapTime->text());
            }
            // Lap time
            ui->label_lapTime->setText(getLapTime(now).toString("mm:ss"));
            ui->frame_center->setStyleSheet("");
        }
    }
}

void MainWindow::startRace()
{
    mRaceStart = QDateTime::currentDateTime();
    mLapStart = mRaceStart;
    mLapCount = 0;
    mPauseMsecs = 0;
    mPauseMsecsLastLap = 0;

    openLogFile();

    mRaceState = STATE_ACTIVE;
    ui->pushButton_lap->setFocus();
}

void MainWindow::stopRace()
{
    mRaceState = STATE_STOPPED;
    closeLogFile();
}

void MainWindow::pauseRace(bool pause)
{
    if (pause) {
        mRaceState = STATE_PAUSED;
        mPauseStart = QDateTime::currentDateTime();
    } else {
        mRaceState = STATE_ACTIVE;
        qint64 ms = mPauseStart.msecsTo(QDateTime::currentDateTime());
        mPauseMsecsLastLap += ms;
        mPauseMsecs += ms;
        ui->pushButton_lap->setFocus();
    }
}

void MainWindow::tap()
{
    if (mRaceState != STATE_ACTIVE) { return; }

    QDateTime now = QDateTime::currentDateTime();
    QTime laptime = getLapTime(now);
    mPauseMsecsLastLap = 0;

    mLapCount++;

    // Lap time
    ui->label_lapTime->setText(laptime.toString("mm:ss"));

    // Distance
    float km = (50*mLapCount)/1000.0;
    ui->label_distance->setText(QString("%1 km").arg(km));

    // Statistics
    ui->label_lapCount->setText(QString::number(mLapCount));
    // todo

    // Log
    log(now, getRaceTime(now), laptime, mLapCount, km);

    mLapStart = now;
}

void MainWindow::openLogFile()
{
    QString filename = "lapcounter_log_"
            + QDateTime::currentDateTime().toString("yyyy-MM-dd_HH'h'mm'm'ss")
            + ".csv";

    mLogFile.setFileName(filename);
    if (!mLogFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Log File", "Error opening log file " + filename);
    }

    QTextStream stream(&mLogFile);
    stream << "clocktime,racetime,laptime,laps,km\n";
}

void MainWindow::closeLogFile()
{
    mLogFile.close();
}

void MainWindow::log(QDateTime clockTime, QTime raceTime, QTime lapTime, int laps, float km)
{
    QTextStream stream(&mLogFile);
    stream   << clockTime.toString("HH:mm:ss") << ","
             << QString::number(raceTime.msecsSinceStartOfDay()/1000) << ","
             << QString::number(lapTime.msecsSinceStartOfDay()/1000) << ","
             << QString::number(laps) << ","
             << QString::number(km)
             << "\n";
}

void MainWindow::on_pushButton_startPause_clicked()
{
    switch (mRaceState) {
    case MainWindow::STATE_NOT_STARTED:
        startRace();
        break;
    case MainWindow::STATE_ACTIVE:
        pauseRace(true);
        break;
    case MainWindow::STATE_PAUSED:
        pauseRace(false);
        break;
    case MainWindow::STATE_STOPPED:
        startRace();
        break;
    }
}

void MainWindow::on_pushButton_lap_clicked()
{

}

void MainWindow::on_pushButton_stop_clicked()
{
    stopRace();
}

void MainWindow::on_pushButton_lap_pressed()
{
    tap();
}
