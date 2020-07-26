#ifndef REQUESTER_H
#define REQUESTER_H

#include <QObject>
#include <QNetworkRequest>
#include <functional>

class QNetworkAccessManager;
class QNetworkReply;
class QDnsLookup;


struct DNSLookup : public QObject{

    Q_OBJECT

public:
    explicit DNSLookup(QObject *parent = nullptr,bool isInList = false,const QString& hostName = ""):
        QObject(parent),
        isInList(isInList),
        hostName(hostName){}

    const bool isInList;
    const QString& hostName;

};

class Requester : public QObject
{
    Q_OBJECT
public:
    static const QUrl QUERY_URL;
    typedef std::function<void(const QString &)> handleFuncRequest;
    typedef std::function<void(const DNSLookup& )> handleFuncDNSLookup;

    explicit Requester(QObject *parent = nullptr);

    void initProxy(const QString& user,
                   const QString& password,
                   const QString& hostname,
                   quint16 port);

    void sendRequest(const Requester::handleFuncRequest &funcSuccess,
                     const Requester::handleFuncRequest &funcFail,
                     const QUrl& url);

    void lookUpHost(const handleFuncDNSLookup &funcDNSLookup,
                    const QString& hostName);

private:
    bool onFinishRequest(const QNetworkReply *reply);
    void handleDNSLookup();

    QNetworkRequest request;
    QNetworkAccessManager *manager;
    QDnsLookup* dnsLookup;
    handleFuncRequest funcSuccess;
    handleFuncRequest funcFailed;
    handleFuncDNSLookup funcDNSLookup;
    QString host;
    int port;
    QString pathTemplate;


signals:
    void networkError();


public slots:
};

#endif // REQUESTER_H
