#include "ugoiraview.h"
#include "ui_ugoiraview.h"
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDebug>
#include <QMessageBox>
#include <QImageReader>
#include <QImage>

UgoiraView::UgoiraView(QWidget *parent)
    : QWidget(parent)
    , now_pixm(new QPixmap)
    , now_timer(new QTimer(this))
    , now_ugoira(new UgoiraFile)
    , ui(new Ui::UgoiraView)
{
    ui->setupUi(this);
    setAcceptDrops(true);
    connect(now_timer, &QTimer::timeout, this, &UgoiraView::timerUpdate);
}

UgoiraView::~UgoiraView()
{
    delete ui;
}

void UgoiraView::dragEnterEvent(QDragEnterEvent *e)
{
    if(e->mimeData()->hasUrls())
    {
        qDebug() << e->mimeData()->urls().first().toLocalFile();
        e->acceptProposedAction();
    }
}

void UgoiraView::dropEvent(QDropEvent *e){
    QString path = e->mimeData()->urls().first().toLocalFile();
    qDebug() << "input path : " << path;
    // todo : little memory leak?
    // todo : support non-ascii (example: ひらがな) file name
    UgoiraFile *utf = new UgoiraFile(path);
    if(utf->IsNull()){
        qDebug() << "Not Ugoira! : " << utf->GetLastError();
        if(utf->GetLastError() == Ugoira::Error::META_NOT_FOUND ||
                utf->GetLastError() == Ugoira::Error::FILE_ENTRY_ERROR ){
            delete utf;
            QMessageBox *msg_b = new QMessageBox();
            msg_b->setText("Invalid ugoira.");
            msg_b->setWindowTitle("error!");
            msg_b->setStandardButtons(QMessageBox::Ok);
            msg_b->setIcon(QMessageBox::Warning);
            msg_b->exec();
            return;
        }
    } else{
        qDebug() << "Ugoira!";
        now_timer->stop();
        delete now_ugoira;
        now_ugoira = utf;
        m_now_frame_index = 0;
        timerUpdate(); // init
        now_timer->start();
        ui->render_label->setText("");
        return;
    }
    QImage *img = new QImage(path);
    if(img->isNull()){
        delete  img;
        QMessageBox *msg_b = new QMessageBox();
        QString fmts = "ugoira";
        foreach(auto str, QImageReader::supportedImageFormats()){
            fmts.append(", ");
            fmts.append(str);
        }
        msg_b->setText("Unsupported format!\nSupported formats list:\n" + fmts);
        msg_b->setWindowTitle("error!");
        msg_b->setStandardButtons(QMessageBox::Ok);
        msg_b->setIcon(QMessageBox::Warning);
        msg_b->exec();
        return;
    }
    now_timer->stop();
    now_ugoira->Close();
    ui->render_label->clear();
    *now_pixm = QPixmap::fromImage(*img);
    ui->render_label->setPixmap(now_pixm->scaled(ui->render_label->size(), Qt::KeepAspectRatio));
    ui->render_label->setText("");
    delete img;
}

void UgoiraView::resizeEvent(QResizeEvent* event){
    if(!now_pixm->isNull()){
        ui->render_label->clear();
        ui->render_label->setPixmap(now_pixm->scaled(ui->render_label->size(), Qt::KeepAspectRatio));
    }
}

void UgoiraView::timerUpdate(){
    if(now_ugoira->IsNull()){return;}
    now_timer->setInterval(now_ugoira->GetFrames()->at(m_now_frame_index).frame_delay);

    ui->render_label->clear();
    // memory reduce
    now_pixm->loadFromData(*now_ugoira->GetFrames()->at(m_now_frame_index).pix_data);
    ui->render_label->setPixmap(now_pixm->scaled(ui->render_label->size(), Qt::KeepAspectRatio));

    m_now_frame_index++;
    if(m_now_frame_index >= now_ugoira->GetFrames()->length()){
        m_now_frame_index = 0;
    }
}
