#include "ugoirafile.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QtGlobal>

using BYTE = char;

UgoiraFile::UgoiraFile():m_frame_infos(new FRAMES){
}
UgoiraFile::UgoiraFile(QString file):m_frame_infos(new FRAMES){
    UgoiraFile::Open(file);
}

UgoiraFile::~UgoiraFile(){
    Close();
}

bool UgoiraFile::IsNull() const{
    return m_is_null;
}

Ugoira::Error UgoiraFile::GetLastError() const{
    return m_last_error;
}

bool UgoiraFile::Open(QString file){
    Close();
    unzFile hUnz = unzOpen64(file.toUtf8().constData());
    if(hUnz == NULL){
        m_last_error = Ugoira::Error::ZIP_OPEN_ERROR;
        m_is_null = true;
        return false;
    }
    QByteArray *content = new QByteArray();
    if(!m_zipentry_open(hUnz, "ugoira_meta", content)){
        m_last_error = Ugoira::Error::META_NOT_FOUND;
        m_is_null = true;
        unzClose(hUnz);
        content->clear();
        return false;
    }else{ // meta found
        QJsonDocument jsonD(QJsonDocument::fromJson(*content));
        QJsonObject jsonObj(jsonD.object());
        if(jsonObj["body"].isUndefined() || jsonObj["body"].toObject()["frames"].isUndefined()){
            m_last_error = Ugoira::Error::FILE_ENTRY_ERROR;
            m_is_null = true;
            unzClose(hUnz);
            return false;
        }
        const auto frm_meta = jsonObj["body"].toObject()["frames"].toArray();
        for(auto framedata : frm_meta){
            QJsonObject frmInfo = framedata.toObject();
            if(frmInfo["file"].isUndefined() || frmInfo["delay"].isUndefined()){
                m_last_error = Ugoira::Error::FILE_ENTRY_ERROR;
                m_is_null = true;
                Close();
                unzClose(hUnz);
                return false;
            }else{
                QByteArray *file_ct = new QByteArray();
                if(!m_zipentry_open(hUnz, frmInfo["file"].toString(), file_ct)){
                    m_last_error = Ugoira::Error::FILE_ENTRY_ERROR;
                    m_is_null = true;
                    Close();
                    unzClose(hUnz);
                    file_ct->clear();
                    return false;
                }else{
                    Ugoira::Frame *frm = new Ugoira::Frame();
                    frm->file_name = frmInfo["file"].toString();
                    frm->frame_delay = frmInfo["delay"].toInt();
                    frm->pix_data = new QByteArray(*file_ct);
                    m_frame_infos->push_back(*frm);
                    file_ct->clear();
                }
            }
        }
        m_last_error = Ugoira::Error::NO_ERROR;
        m_is_null = false;
        unzClose(hUnz);
        return  true;
    }
}

FRAMES* UgoiraFile::GetFrames() const{
    return m_frame_infos;
}

void UgoiraFile::Close(){
    m_is_null = true;
    while (m_frame_infos->length() > 0) {
        m_frame_infos->last().pix_data->clear();
        m_frame_infos->pop_back();
    }
    m_frame_infos->clear();
}

bool UgoiraFile::m_zipentry_open(unzFile hzip, QString name, QByteArray *result){
    if(unzLocateFile(hzip, name.toUtf8().constData(), 0)){ // not found
        return false;
    }
    if(unzOpenCurrentFile(hzip) != UNZ_OK){ // not found
        return false;
    }
    BYTE szBuff[8192];
    int readed_bytes;
    while ((readed_bytes = unzReadCurrentFile(hzip, szBuff, sizeof szBuff))) {
        result->append(szBuff, readed_bytes);
    }
    return true;
}
