#include "messagelog.h"
#include <QDateTime>
#include <QFile>
#include <QTextStream>

MessageLog * MessageLog::m_instance = nullptr;

static QMutex messageMutex;

void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QMutexLocker locker(&messageMutex);
    QString MessageType;
    QString currentDataTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString contentInfo = QString("File:%1, function:%2, Line:%3").arg(context.file).arg(context.function).arg(context.line);
    switch (type) {
    case QtDebugMsg:
        MessageType = "[Debug]";
        break;
    case QtInfoMsg:
        MessageType = "[Info]";
        break;
    case QtWarningMsg:
        MessageType = "[Warning]";
        break;
    case QtCriticalMsg:
        MessageType = "[Critical]";
        break;
    case QtFatalMsg:
        MessageType = "[Fatal:]";
        break;
    }

    QString messageLogInfo = QString(currentDataTime + "," + MessageType + contentInfo + ":" + msg);

    QString fileName = "NetDemo_" + QDateTime::currentDateTime().toString("yyyy-MM-dd") + ".txt";
    QFile file(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << messageLogInfo << endl;
    file.flush();
    file.close();

    QMetaObject::invokeMethod(MessageLog::instance(), "message" ,
                              Q_ARG(QString, MessageType),
                              Q_ARG(QString, messageLogInfo));
}

MessageLog::MessageLog(QObject *parent) : QObject(parent)
{
    qInstallMessageHandler(outputMessage);
}

/* 获取单例对象 */
MessageLog* MessageLog::instance()
{
    static QMutex mutex;
    if (nullptr == m_instance)
    {
        QMutexLocker locker(&mutex);
        if (nullptr == m_instance)
        {
            m_instance = new MessageLog;
        }
    }

    return m_instance;
}
