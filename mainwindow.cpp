#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <register-model.h>

#include <QRandomGenerator>
#include <QDebug>
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
    ui->customPlot_2->addGraph();
    // random color for each plot
    ui->customPlot_2->graph(0)->setPen(QColor(QRandomGenerator::global()->bounded(50,255), QRandomGenerator::global()->bounded(50, 255), QRandomGenerator::global()->bounded(50,255)));
    
    // voltage
    ui->customPlot_3->addGraph();
    // random color for each plot
    ui->customPlot_3->graph(0)->setPen(QColor(QRandomGenerator::global()->bounded(50,255), QRandomGenerator::global()->bounded(50, 255), QRandomGenerator::global()->bounded(50,255)));

    // cell voltages
    for (uint8_t i = 0; i < 6; i++) {
        ui->customPlot_4->addGraph();
        // random color for each plot
        ui->customPlot_4->graph(i)->setPen(QColor(QRandomGenerator::global()->bounded(50,255), QRandomGenerator::global()->bounded(50, 255), QRandomGenerator::global()->bounded(50,255)));
    }

    connect(&replotTimer, &QTimer::timeout, this, &MainWindow::replot);
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
                    connect(device, &Device::closed, this, &MainWindow::deviceClosed);

                    if (device->open()) {
                        ui->label->setText(QString("connected to %1").arg(availablePort.portName()));
                        ui->serialConnectButton->setText("disconnect");
                        device->requestDeviceInformation();
                        replotTimer.start(30);
                        ui->tableView->setModel(&device->registerModel);

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
//    ui->customPlot->graph(2)->addData(key, device->state.board_temperature);

    ui->customPlot->xAxis->setRange(key, 5, Qt::AlignRight);
    ui->customPlot->graph(0)->rescaleValueAxis(false, true);
    ui->customPlot->graph(1)->rescaleValueAxis(true, true);
//    ui->customPlot->graph(2)->rescaleValueAxis(true, true);

    ui->customPlot_2->graph(0)->addData(key, device->state.battery_current);
    ui->customPlot_2->xAxis->setRange(key, 5, Qt::AlignRight);
    ui->customPlot_2->graph(0)->rescaleValueAxis(false, true);

    ui->customPlot_3->graph(0)->addData(key, device->state.battery_voltage);
    ui->customPlot_3->xAxis->setRange(key, 5, Qt::AlignRight);
    ui->customPlot_3->graph(0)->rescaleValueAxis(false, true);

    ui->customPlot_4->xAxis->setRange(key, 5, Qt::AlignRight);
    ui->customPlot_4->graph(0)->rescaleValueAxis(false, true);
    ui->customPlot_4->graph(0)->addData(key, device->state.cell_voltages[0]);

    for (uint8_t i = 1; i < 4; i++) {
        ui->customPlot_4->graph(i)->addData(key, device->state.cell_voltages[i]);
        ui->customPlot_4->graph(i)->rescaleValueAxis(true, true);
        qDebug() << "cell" << i << device->state.cell_voltages[i];
    }

    ui->customPlot->replot();
    ui->customPlot_2->replot();
    ui->customPlot_3->replot();
    ui->customPlot_4->replot();

    device->registerModel.refreshAll();
}

void MainWindow::onPortScanFinished(QList<QSerialPortInfo> availablePorts)
{
    ui->serialComboBox->clear();
    for (auto portInfo : availablePorts) {
        ui->serialComboBox->addItem(portInfo.portName());
    }
}
