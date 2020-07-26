#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHash>

class Requester;
class QNetworkAccessManager;
class QRegExpValidator;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    static const QString IP,PTR,COUNTRY,CITY;

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_check_spam_clicked();

    void on_checkButton_clicked();

private:

    void succProcess(const QString& data);
    QRegExpValidator *getIPValidator(QWidget *parent);
    QRegExpValidator *getPortValidator(QWidget *parent);

    Ui::MainWindow *ui;
    QHash<QString, QString> result;
    Requester* req;
    QTimer* checkTimer;



};
#endif // MAINWINDOW_H
