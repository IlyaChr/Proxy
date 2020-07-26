#include <QNetworkProxy>
#include <QSharedPointer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDnsLookup>
#include <QThread>
#include "requester.h"

const QUrl Requester::QUERY_URL = QUrl("http://api4ip.info/api/ip/checker/full");


Requester::Requester(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);

    dnsLookup = new QDnsLookup(this);
    dnsLookup->setType(QDnsLookup::ANY);

    QObject::connect(dnsLookup,&QDnsLookup::finished,this,&Requester::handleDNSLookup);

    QObject::connect(manager, &QNetworkAccessManager::finished,
                     this, [=](QNetworkReply *reply) {
        if (onFinishRequest(reply)) {
            funcSuccess(reply->readAll());
        }else{
            funcFailed(reply->errorString());
        }

        reply->close();
        reply->deleteLater();
    }
    );
}

void Requester::initProxy(const QString& user,
                          const QString& password,
                          const QString& hostname,
                          quint16 port)
{
    QNetworkProxy proxy;
    proxy.setType(QNetworkProxy::Socks5Proxy);
    proxy.setHostName(hostname);
    proxy.setPort(port);

    if (!user.isEmpty()){
        proxy.setUser(user);
    }

    if (!password.isEmpty()){
        proxy.setPassword(password);
    }

    QNetworkProxy::setApplicationProxy(proxy);

}


void Requester::sendRequest(const Requester::handleFuncRequest &funcSuccess,
                            const Requester::handleFuncRequest &funcFailed,
                            const QUrl& url)
{
    this->funcSuccess = funcSuccess;
    this->funcFailed = funcFailed;

    request.setUrl(url);
    manager->get(request);

}

void Requester::lookUpHost(const handleFuncDNSLookup &funcDNSLookup,
                           const QString &hostName)
{
    this->funcDNSLookup = funcDNSLookup;

    dnsLookup->setName(hostName+".zen.spamhaus.org");
    dnsLookup->lookup();
}


bool Requester::onFinishRequest(const QNetworkReply *reply)
{
    auto replyError = reply->error();
    if (replyError == QNetworkReply::NoError ) {
        int code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if ((code >=200) && (code < 300)) {
            return true;
        }
    }
    return false;
}

void Requester::handleDNSLookup()
{
    if (dnsLookup->error() != QDnsLookup::NoError) {
        //qDebug()<<dnsLookup->errorString();
        funcDNSLookup(DNSLookup{this,false,dnsLookup->name()});
        return;
    }

    funcDNSLookup(DNSLookup{this,true,dnsLookup->name()});

    const auto records = dnsLookup->hostAddressRecords();
    for (const auto &record : records) {
        qDebug()<<record.value();
    }
}

