#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <register-model.h>

#include <QRandomGenerator>

#include <qcustomplot.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , device(nullptr)
    , replotTimer(this)
    , connectState(CONNECT)
{
    ui->setupUi(this);

    connect(&portScanner, &PortScanner::scanUpdate, this, &MainWindow::onPortScanFinished);
    portScanner.startScanning(500);

    // temperatures
    for (uint8_t i = 0; i < 3; i++) {
        ui->customPlot->addGraph();
        // random color for each plot
        ui->customPlot->graph(i)->setPen(QColor(QRandomGenerator::global()->bounded(50,255), QRandomGenerator::global()->bounded(50, 255), QRandomGenerator::global()->bounded(50,255)));
    }

    // current
    ui->customPlot_1->addGraph();
    // random color for each plot
    ui->customPlot_1->graph(0)->setPen(QColor(QRandomGenerator::global()->bounded(50,255), QRandomGenerator::global()->bounded(50, 255), QRandomGenerator::global()->bounded(50,255)));
    
    // voltage
    ui->customPlot_2->addGraph();
    // random color for each plot
    ui->customPlot_2->graph(0)->setPen(QColor(QRandomGenerator::global()->bounded(50,255), QRandomGenerator::global()->bounded(50, 255), QRandomGenerator::global()->bounded(50,255)));

    // cell voltages
    for (uint8_t i = 0; i < 8; i++) {
        ui->customPlot_3->addGraph()->setVisible(false);
        // random color for each plot
        ui->customPlot_3->graph(i)->setPen(QColor(QRandomGenerator::global()->bounded(50,255), QRandomGenerator::global()->bounded(50, 255), QRandomGenerator::global()->bounded(50,255)));
    }

    connect(&replotTimer, &Timer::timeout, this, &MainWindow::replot);
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
                    connect(device, &Device::newData, this, &MainWindow::handleNewDeviceData);
                    connect(device, &Device::closed, this, &MainWindow::deviceClosed);
                    connect(&device->registerModel, &RegisterModel::plotEnabledChanged, this, &MainWindow::onPlotEnabledChanged);

                    if (device->open()) {
                        ui->label->setText(QString("connected to %1").arg(availablePort.portName()));
                        ui->serialConnectButton->setText("disconnect");
                        device->requestDeviceInformation();
                        replotTimer.start(30);
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
    replotTimer.stop();
    if (device) {
        delete(device);
        device = nullptr;
    }

    portScanner.startScanning();
    ui->serialConnectButton->setText("connect");
    connectState = CONNECT;
}

void MainWindow::replot()
{
    static QTime time(QTime::currentTime());
    // calculate two new data points:
    double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds

    // TODO bug in openesc implementation, key not getting updated to limit rate

    // temperatures
    ui->customPlot->graph(0)->addData(key, device->state.battery_temperature);
    ui->customPlot->graph(1)->addData(key, device->state.cpu_temperature);
    ui->customPlot->graph(2)->addData(key, device->state.board_temperature);

    ui->customPlot->xAxis->setRange(key, 5, Qt::AlignRight);
    ui->customPlot->graph(i)->rescaleValueAxis(false, true);

    ui->customPlot_1->graph(0)->addData(key, device->state.battery_current);
    ui->customPlot_1->xAxis->setRange(key, 5, Qt::AlignRight);
    ui->customPlot_1->graph(0)->rescaleValueAxis(false, true);

    ui->customPlot_2->graph(0)->addData(key, device->state.battery_voltage);
    ui->customPlot_2->xAxis->setRange(key, 5, Qt::AlignRight);
    ui->customPlot_2->graph(0)->rescaleValueAxis(false, true);

    ui->customPlot_3->xAxis->setRange(key, 5, Qt::AlignRight);
    ui->customPlot_3->graph(0)->rescaleValueAxis(false, true);
    ui->customPlot_3->graph(0)->addData(key, device->state.cell_voltages[0]);

    for (uint8_t i = 1; i < 8; i++) {
        ui->customPlot_3->graph(i)->addData(key, device->state.cell_voltages[i]);
        ui->customPlot_3->graph(i)->rescaleValueAxis(true, true);
    }

    ui->customPlot->replot();
    ui->customPlot_1->replot();
    ui->customPlot_2->replot();
    ui->customPlot_3->replot();

}

void MainWindow::onPortScanFinished(QList<QSerialPortInfo> availablePorts)
{
    ui->serialComboBox->clear();
    for (auto portInfo : availablePorts) {
        ui->serialComboBox->addItem(portInfo.portName());
    }
}
