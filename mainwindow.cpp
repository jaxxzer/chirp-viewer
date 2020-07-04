#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QRandomGenerator>
#include <QDebug>
#include <qcustomplot.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , device(nullptr)
    , connectState(CONNECT)
{
    ui->setupUi(this);

    connect(&portScanner, &PortScanner::scanUpdate, this, &MainWindow::onPortScanFinished);
    portScanner.startScanning(5000);

    ui->customPlot->addGraph()->setVisible(true);
    ui->customPlot->graph()->setAdaptiveSampling(true);
}

MainWindow::~MainWindow()
{
    delete ui;
    if (device) {
        delete device;
    }
}

void MainWindow::on_serialConnectButton_clicked()
{
    if (connectState == CONNECT) {
        for (auto availablePort : portScanner.availablePorts) {
            if (availablePort.portName() == ui->serialComboBox->currentText()) {
                if (!device) {
                    portScanner.stopScanning();
                    device = new Device(availablePort);
                    connect(device, &Device::newData, this, &MainWindow::plotProfile);
                    connect(device, &Device::closed, this, &MainWindow::deviceClosed);

                    if (device->open()) {
                        ui->label->setText(availablePort.portName());
                        ui->serialConnectButton->setText("disconnect");

                        //device->requestDeviceInformation();
                        connectState = DISCONNECT;
                    }
                }
            }
        }

    } else if (connectState == DISCONNECT) {
        device->close();
    }

    //assert_not_reached();

}

void MainWindow::deviceClosed()
{
    if (device) {
        delete(device);
        device = nullptr;
    }

    portScanner.startScanning();
    ui->serialConnectButton->setText("connect");
    connectState = CONNECT;
}

void MainWindow::plotProfile(QVector<double> keys, QVector<double> data)
{
    ui->customPlot->xAxis->setRange(0, keys.last());
    ui->customPlot->yAxis->setRange(0, 255);

    ui->customPlot->graph()->setData(keys, data, true);
    ui->customPlot->replot();
}

void MainWindow::onPortScanFinished(QList<QSerialPortInfo> availablePorts)
{
    ui->serialComboBox->clear();
    for (auto portInfo : availablePorts) {
        ui->serialComboBox->addItem(portInfo.portName());
    }
}

void MainWindow::on_nsamplesSlider_valueChanged(int value)
{
    device->setNSamples(value);
}

void MainWindow::on_f0Slider_valueChanged(int value)
{
    device->setF0(value);
}

void MainWindow::on_f1Slider_valueChanged(int value)
{
    device->setF1(value);
}

void MainWindow::on_pulseDurationSlider_valueChanged(int value)
{
    device->setPulseDuration(value);
}

void MainWindow::on_opamp1GainSlider_valueChanged(int value)
{
    device->setOpamp1Gain(value);
}

void MainWindow::on_opamp2GainSlider_valueChanged(int value)
{
    device->setOpamp2Gain(value);
}
