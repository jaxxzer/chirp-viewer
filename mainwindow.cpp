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
    ui->adcResolutionComboBox->addItem("12 bit");
    ui->adcResolutionComboBox->addItem("10 bit");
    ui->adcResolutionComboBox->addItem("8 bit");

    ui->adcSampleTimeComboBox->addItem("1.5 cycles");
    ui->adcSampleTimeComboBox->addItem("2.5 cycles");
    ui->adcSampleTimeComboBox->addItem("4.5 cycles");
    ui->adcSampleTimeComboBox->addItem("7.5 cycles");
    ui->adcSampleTimeComboBox->addItem("19.5 cycles");
    ui->adcSampleTimeComboBox->addItem("61.5 cycles");
    ui->adcSampleTimeComboBox->addItem("181.5.5 cycles");
    ui->adcSampleTimeComboBox->addItem("601.5 cycles");

    ui->windowComboBox->addItem("rectangular");
    ui->windowComboBox->addItem("hanning");
    ui->windowComboBox->addItem("hamming");



    connect(&portScanner, &PortScanner::scanUpdate, this, &MainWindow::onPortScanFinished);
    portScanner.startScanning(500);

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
                    connect(device, &Device::rxParsedChanged, this, &MainWindow::rxParsedChanged);
                    connect(device, &Device::rxErrorsChanged, this, &MainWindow::rxErrorsChanged);
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
//    ui->nsamplesSlider->setValue(device->nsamples);
//    ui->f0Slider->setValue(device->f0);
//    ui->f1Slider->setValue(device->f1);
//    ui->pulseDurationSlider->setValue(device->pulse_duration);
    ui->nsamplesLabel->setNum((int)device->nsamples);
    ui->f0Label->setNum((int)device->f0);
    ui->f1Label->setNum((int)device->f1);
    ui->pulseDurationLabel->setNum(device->pulse_duration);
    ui->opamp1GainLabel->setNum(pow(2, device->opamp1_gain+1));
    ui->opamp2GainLabel->setNum(pow(2, device->opamp2_gain+1));
    ui->adcResolutionLabel->setNum(device->adc_resolution);
    ui->adcSampleTimeLabel->setNum(device->adc_sample_time);
    ui->windowLabel->setNum(device->window);
}

void MainWindow::onPortScanFinished(QList<QSerialPortInfo> availablePorts)
{
    ui->serialComboBox->clear();
    for (auto portInfo : availablePorts) {
        if (portInfo.portName().contains("USB")) {
            ui->serialComboBox->addItem(portInfo.portName());
        }
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

void MainWindow::rxParsedChanged(uint32_t rxParsed)
{
    ui->rxParsedLabel->setNum((int)rxParsed);
}

void MainWindow::rxErrorsChanged(uint32_t rxErrors)
{
    ui->rxErrorsLabel->setNum((int)rxErrors);
}

void MainWindow::on_windowComboBox_activated(int index)
{
    device->setWindow(index);
}

void MainWindow::on_adcSampleTimeComboBox_activated(int index)
{
    device->setAdcSampleTime(index);
}

void MainWindow::on_adcResolutionComboBox_activated(int index)
{
    device->setAdcResolution(index);
}
