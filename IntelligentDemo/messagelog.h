#ifndef MESSAGELOG_H
#define MESSAGELOG_H

#include <QObject>
#include <QMutex>

class MessageLog : public QObject
{
    Q_OBJECT
public:
    static MessageLog* instance();

signals:
    void message(const QString& messageType, const QString &msg);

private:
    explicit MessageLog(QObject *parent = nullptr);
    static MessageLog* m_instance;
};

#endif // MESSAGELOG_H
