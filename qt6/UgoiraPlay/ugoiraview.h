#ifndef UGOIRAVIEW_H
#define UGOIRAVIEW_H

#include <QWidget>
#include <QPixmap>
#include <QTimer>
#include "ugoirafile.h"

QT_BEGIN_NAMESPACE
namespace Ui { class UgoiraView; }
QT_END_NAMESPACE

class UgoiraView : public QWidget
{
    Q_OBJECT

public:
    UgoiraView(QWidget *parent = nullptr);
    ~UgoiraView();
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);
    void resizeEvent(QResizeEvent* event);

private:
    QPixmap *now_pixm;
    QTimer *now_timer;
    UgoiraFile *now_ugoira;
    Ui::UgoiraView *ui;
    int m_now_frame_index = 0;

public slots:
    void timerUpdate();
};
#endif // UGOIRAVIEW_H
