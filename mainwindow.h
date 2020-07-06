#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <qcustomplot.h>
#include <port-scanner.h>
#include <device.h>
#include <QList>
#include <QSerialPortInfo>
#include <QVector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    typedef enum {
        DISCONNECT,
        CONNECT,
    } connect_e;

    connect_e connectState;
    void deviceClosed();

    void replot();

private slots:
//    void on_serialComboBox_activated(QString portName);
    void on_serialConnectButton_clicked();
    void onPortScanFinished(QList<QSerialPortInfo>);
    void plotProfile(QVector<double> keys, QVector<double> data);
    void on_nsamplesSlider_valueChanged(int value);

    void on_f0Slider_valueChanged(int value);

    void on_f1Slider_valueChanged(int value);

    void on_pulseDurationSlider_valueChanged(int value);

    void on_opamp1GainSlider_valueChanged(int value);

    void on_opamp2GainSlider_valueChanged(int value);
    void rxParsedChanged(uint32_t rxParsed);
    void rxErrorsChanged(uint32_t rxErrors);

    void on_windowComboBox_activated(int index);

    void on_adcSampleTimeComboBox_activated(int index);

    void on_adcResolutionComboBox_activated(int index);

private:
    Ui::MainWindow *ui;

    PortScanner portScanner;
    Device* device;
};
#endif // MAINWINDOW_H
