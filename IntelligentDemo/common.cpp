#include "common.h"
#include <QIODevice>
#include <QDebug>
#include <QFile>
#include <QMutex>

Common::Common()
{

}

/* 读取文件 */
 bool Common::getFileData(const QString& filePath, QByteArray& fileData)
{
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly))
    {
        qCritical() << "Can't open file for read";
        return false;
    }

    if(file.size() > NETDEMO_SMALL_IMAGE_SIZE)
    {
        qCritical() << "Image size error";
        return false;
    }

    char* aszImageData = new char[NETDEMO_SMALL_IMAGE_SIZE];

    int dwLen = file.read(aszImageData, NETDEMO_SMALL_IMAGE_SIZE);
    file.close();

    if(dwLen <= 0)
    {
        return false;
    }

    fileData.append(aszImageData, dwLen);
    delete[] aszImageData;

    return true;
}
