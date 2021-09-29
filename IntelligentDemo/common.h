#ifndef COMMON_H
#define COMMON_H
#include <QByteArray>
#include <QString>

class Common
{
public:
    Common();
    static bool getFileData(const QString& filePath, QByteArray& fileData);

private:
    static const int NETDEMO_SMALL_IMAGE_SIZE = 1024 * 1024;
};

#endif // COMMON_H
