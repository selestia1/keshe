#ifndef QWGRAPHICSVIEW_H
#define QWGRAPHICSVIEW_H
#include <QGraphicsView>
#include <QKeyEvent>
#include <QDebug>

class QWGraphicsView:public QGraphicsView
{
    Q_OBJECT

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

public:
    QWGraphicsView(QWidget *parent = nullptr);

signals:
    void mouseMovePoint(QPoint point);  //鼠标移动
    void mouseClicked(QPoint point);
    void mouseDoubleClick(QPoint point);
    void keyPress(QKeyEvent *event);

};

#endif // QWGRAPHICSVIEW_H
