#ifndef COUNTDOWNWINDOW_H
#define COUNTDOWNWINDOW_H

#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QDateTime>
#include <QSettings>
#include <QFile>
#include <QScreen>
#include <QFontDatabase>
#include <QPainter>
#include <QFontMetrics>
#include <QDebug>
#include <QDir>

class CountdownWindow : public QWidget
{
    Q_OBJECT

public:
    CountdownWindow(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *) override;

private:
    QLabel *label;
    QDateTime targetDateTime;

    QString findConfigFile();
    void updateCountdown();
    void moveToTopRight();
};
#endif // COUNTDOWNWINDOW_H
