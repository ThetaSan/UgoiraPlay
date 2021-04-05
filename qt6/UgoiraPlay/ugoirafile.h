#ifndef UGOIRAFILE_H
#define UGOIRAFILE_H
#include <QString>
#include <QPixmap>
#include <QList>
#include <External/zlib/minizip/unzip.h>

namespace Ugoira {
class UgoiraFile;
struct Frame{
    QString file_name;
    QByteArray* pix_data;
    int frame_delay;
};
enum Error{
    NO_ERROR,
    ZIP_OPEN_ERROR,
    META_NOT_FOUND,
    FILE_ENTRY_ERROR,
};

}

using FRAMES = QList<Ugoira::Frame>;

class UgoiraFile
{
public:
    UgoiraFile();
    UgoiraFile(QString file);
    ~UgoiraFile();
    bool IsNull() const;
    bool Open(QString file);
    Ugoira::Error GetLastError() const;
    FRAMES* GetFrames() const;
    void Close();

private:
    bool m_is_null = true;
    Ugoira::Error m_last_error = Ugoira::Error::NO_ERROR;
    FRAMES *m_frame_infos;

    bool m_zipentry_open(unzFile hzip, QString name, QByteArray* result);
};


#endif // UGOIRAFILE_H
