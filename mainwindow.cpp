#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "requester.h"
#include <QDesktopWidget>
#include <QDebug>
#include <QRegExpValidator>
#include <QTimer>


const QString MainWindow::IP = "IP";
const QString MainWindow::PTR = "PTR";
const QString MainWindow::COUNTRY = "COUNTRY";
const QString MainWindow::CITY = "CITY";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    QSize screenSize = QDesktopWidget().availableGeometry(this).size();
    this->setFixedSize(QSize(static_cast<int>(screenSize.width()*0.2),static_cast<int>(screenSize.height()*0.5)));

    ui->spam_ip->setValidator(getIPValidator(ui->spam_ip));
    ui->proxy_hostname->setValidator(getIPValidator(ui->proxy_hostname));
    ui->proxy_port->setValidator(getPortValidator(ui->proxy_port));

    req = new Requester(this);

    checkTimer = new QTimer(this);
    checkTimer->setSingleShot(true);
    checkTimer->setInterval(10000);
    QObject::connect(checkTimer,&QTimer::timeout,this,[=](){
        ui->checkButton->setEnabled(true);
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_checkButton_clicked()
{
    if (ui->proxy_hostname->text().isEmpty() || ui->proxy_port->text().isEmpty()){
        return;
    }

    ui->checkButton->setDisabled(true);

    ui->proxy_hostname_result->clear();
    ui->proxy_ptr_result->clear();
    ui->proxy_country_result->clear();
    ui->proxy_city_result->clear();


    req->initProxy(
                ui->proxy_login->text(),
                ui->proxy_password->text(),
                ui->proxy_hostname->text(),
                static_cast<quint16>(ui->proxy_port->text().toUInt()));


    req->sendRequest([=](const QString &succRes){

        succProcess(succRes);

    },
    [](const QString& failedRes){

        qDebug()<<failedRes;

    },
    Requester::QUERY_URL);

    checkTimer->start();

}

void MainWindow::succProcess(const QString &data)
{
    checkTimer->stop();
    QVector<QString> vector_res;
    vector_res.reserve(4);


    for (auto str : data.split("\n")){
        vector_res.push_back(std::move(str));
    }

    if (vector_res.size()>=4){
        result[MainWindow::IP] = std::move(vector_res.at(0));
        result[MainWindow::PTR] = std::move(vector_res.at(1));
        result[MainWindow::COUNTRY] = std::move(vector_res.at(2));
        result[MainWindow::CITY] = std::move(vector_res.at(3));

        ui->proxy_hostname_result->setText(result[MainWindow::IP]);
        ui->proxy_ptr_result->setText(result[MainWindow::PTR]);
        ui->proxy_country_result->setText(result[MainWindow::COUNTRY]);
        ui->proxy_city_result->setText(result[MainWindow::CITY]);
    }

    ui->checkButton->setEnabled(true);
}

QRegExpValidator *MainWindow::getIPValidator(QWidget *parent)
{
    QString ipRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    QRegExp ipRegex ("^" + ipRange
                         + "\\." + ipRange
                         + "\\." + ipRange
                         + "\\." + ipRange + "$");

    QRegExpValidator* ipValidator = new QRegExpValidator(ipRegex, parent);
    return  ipValidator;
}

QRegExpValidator *MainWindow::getPortValidator(QWidget *parent)
{
    QString ipRange = "^([0-9]{1,4}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-5])$";
    QRegExp ipRegex (ipRange);

    QRegExpValidator* ipValidator = new QRegExpValidator(ipRegex, parent);
    return  ipValidator;
}




void MainWindow::on_check_spam_clicked()
{
    if (ui->spam_ip->text().isEmpty()){
        return;
    }

    ui->check_spam->setDisabled(true);

    req->lookUpHost([this](const DNSLookup& dnsLookup){

        if (dnsLookup.isInList){
            ui->spamhaus_result->appendPlainText("IP адрес "+ dnsLookup.hostName.split(".zen.spamhaus.org")[0] +" находится в списке ZEN");
        }else{
            ui->spamhaus_result->appendPlainText("IP адрес "+ dnsLookup.hostName.split(".zen.spamhaus.org")[0] +" не замечен ни в одном блэклисте");
        }

        ui->check_spam->setEnabled(true);

    },
    ui->spam_ip->text());
}

