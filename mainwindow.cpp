#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qwgraphicsview.h"
#include <QTime>
#include <QGraphicsEllipseItem>
#include <QGraphicsItemGroup>
#include <QColorDialog>
#include <QFontDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>

//泛型函数模板
template<class T>
void MainWindow::setBrushColor( T *item)
{
    QColor color = item->brush().color();
    color = QColorDialog::getColor(color,nullptr,"选择颜色");
    if(color.isValid())
        item->setBrush(QBrush(color));
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("GraphicsView绘图基本操作");

    labViewCord = new QLabel("View 坐标:");
    labViewCord->setMinimumWidth(150);
    labSceneCord = new QLabel("Scene 坐标:");
    labSceneCord->setMinimumWidth(150);
    labItemCord=new QLabel("Item 坐标:");
    labItemCord->setMinimumWidth(150);
    labItemInfo=new QLabel("ItemInfo: ");
    labItemInfo->setMinimumWidth(150);

    ui->statusBar->addWidget(labViewCord);
    ui->statusBar->addWidget(labSceneCord);
    ui->statusBar->addWidget(labItemCord);
    ui->statusBar->addWidget(labItemInfo);

    //创建场景，场景坐标原点在中心
    scene = new QGraphicsScene(-300,-200,600,200);

    //视图关联到场景
    ui->graphicsView->setScene(scene);

    //光标设为十字
    ui->graphicsView->setCursor(Qt::CrossCursor);

    //使鼠标不点击也可以跟踪事件，否则mousemove事件无效
    ui->graphicsView->setMouseTracking(true);

    //使在视图中拖拽时出现矩形框选状态
    ui->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);

    connect(ui->graphicsView,SIGNAL(mouseMovePoint(QPoint)),this,SLOT(on_mouseMovePoint(QPoint)));
    connect(ui->graphicsView,SIGNAL(mouseClicked(QPoint)),this,SLOT(on_mouseClicked(QPoint)));
    connect(ui->graphicsView,SIGNAL(mouseDoubleClick(QPoint)),this,SLOT(on_mouseDoubleClick(QPoint)));
    connect(ui->graphicsView,&QWGraphicsView::keyPress,this,&MainWindow::on_keyPress);
    //使图形项坐标随机摆放
    qsrand(static_cast<uint>(QTime::currentTime().second()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_mouseMovePoint(QPoint point)
{
    labViewCord->setText(QString::asprintf("View 坐标：%d,%d ",point.x(),point.y()));
    //转换到场景坐标
    QPointF pointScene = ui->graphicsView->mapToScene(point);

    labSceneCord->setText(QString::asprintf("Scene 坐标：%.0f,%.0f",pointScene.x(),pointScene.y()));


}

void MainWindow::on_mouseClicked(QPoint point)
{
    //转换到场景坐标
    QPointF pointScene = ui->graphicsView->mapToScene(point);
    //光标下的图形项
    QGraphicsItem *item = scene->itemAt(pointScene,ui->graphicsView->transform());
    if(item!=nullptr)
    {
        //图形项自身坐标系
        QPointF pointItem = item->mapFromScene(pointScene);
        labItemCord->setText(QString::asprintf("Item 坐标: %.0f,%.0f",pointItem.x(),pointItem.y()));

        labItemInfo->setText(item->data(ITemDescription).toString()+",  ItemId="+item->data(ItemId).toString());


    }
}

void MainWindow::on_mouseDoubleClick(QPoint point)
{
    //视图坐标转换为场景坐标
    QPointF pointScene = ui->graphicsView->mapToScene(point);

    //得到点击选中的图形项
    QGraphicsItem *item = scene->itemAt(pointScene,ui->graphicsView->transform());

    if(item==nullptr) return;
    switch (item->type())
    {
    case QGraphicsRectItem::Type:  //矩形

    {
        QGraphicsRectItem *theItem = qgraphicsitem_cast<QGraphicsRectItem *>(item);
        setBrushColor(theItem);
        break;
    }
    case QGraphicsEllipseItem::Type:  //椭圆和圆都是此类型
    {
        QGraphicsEllipseItem *theItem =qgraphicsitem_cast<QGraphicsEllipseItem *>(item);
        setBrushColor(theItem);
        break;
    }
    case QGraphicsPolygonItem::Type: //梯形和三角
    {
        QGraphicsPolygonItem *theItem = qgraphicsitem_cast<QGraphicsPolygonItem *>(item);
        setBrushColor(theItem);
        break;
    }
    case QGraphicsLineItem::Type: //直线
    {
        QGraphicsLineItem *theItem = qgraphicsitem_cast<QGraphicsLineItem *>(item);
        QPen pen = theItem->pen();
        QColor color = theItem->pen().color();
        color=QColorDialog::getColor(color,this,"选择颜色");
        if(color.isValid())
        {
            pen.setColor(color);
            theItem->setPen(pen);
        }
        break;
    }
    case QGraphicsTextItem::Type:  //文字
    {
        QGraphicsTextItem *theItem = qgraphicsitem_cast<QGraphicsTextItem *>(item);
        QFont font = theItem->font();

        bool ok = false;
        font = QFontDialog::getFont(&ok,font,this,"设置字体");
        if(ok)
            theItem->setFont(font);

        break;
    }
    }

}

void MainWindow::on_keyPress(QKeyEvent *event)
{

    if(scene->items().count()==0) return;

    switch (event->key())
    {
    case Qt::Key_Delete:
        foreach(QGraphicsItem *item ,scene->selectedItems())
            scene->removeItem(item);
        break;
    case Qt::Key_Space:
        foreach(QGraphicsItem *item ,scene->selectedItems())
            item->setRotation(90+item->rotation());
        break;
    case Qt::Key_PageUp:
        foreach(QGraphicsItem *item ,scene->selectedItems())
            item->setScale(0.1+item->scale());
        break;
    case Qt::Key_PageDown:
        foreach(QGraphicsItem *item ,scene->selectedItems())
            item->setScale(-0.1+item->scale());
        break;
    case Qt::Key_Left:
        foreach(QGraphicsItem *item ,scene->selectedItems())
            item->setX(-1+item->x());
        break;
    case Qt::Key_Right:
        foreach(QGraphicsItem *item ,scene->selectedItems())
            item->setX(1+item->x());
        break;
    case Qt::Key_Up:
        foreach(QGraphicsItem *item ,scene->selectedItems())
            item->setY(-1+item->y());
        break;
    case Qt::Key_Down:
        foreach(QGraphicsItem *item ,scene->selectedItems())
            item->setY(1+item->y());
        break;

    default:
        //ctrl+A
        if((event->modifiers()== Qt::ControlModifier) && event->key() ==Qt::Key_A)
        {

            foreach(QGraphicsItem *item ,scene->items())
            {
                item->setSelected(true);
            }
        }
        break;

    }

}


void MainWindow::on_ZoomInAction_triggered()
{
    foreach(QGraphicsItem *item ,scene->selectedItems())
    {
        item->setScale(item->scale() + 0.1);
    }

}

void MainWindow::on_ZoomOutAction_triggered()
{
    foreach(QGraphicsItem *item ,scene->selectedItems())
    {
        item->setScale(item->scale() - 0.1);
    }


}


void MainWindow::on_GroupAction_triggered()
{
    //选中的图形项合并

    int cont = scene->selectedItems().count();
    if(cont>1)
    {
        QGraphicsItemGroup *group = new QGraphicsItemGroup();
        scene->addItem(group);
        for(int i=0;i<cont;i++)
        {
            QGraphicsItem *item = scene->selectedItems().at(0);
            item->setSelected(false);
            item->clearFocus();
            group->addToGroup(item);

        }
        group->setFlags(QGraphicsItem::ItemIsMovable
                        |QGraphicsItem::ItemIsSelectable
                        |QGraphicsItem::ItemIsFocusable);
        group->setZValue(++frontZ);
        scene->clearSelection();
        group->setSelected(true);
    }
}




void MainWindow::on_EllipseAction_triggered()
{
    QGraphicsEllipseItem *item = new QGraphicsEllipseItem(-50,-30,100,60);
    item->setFlags(QGraphicsItem::ItemIsMovable
                   |QGraphicsItem::ItemIsSelectable
                   |QGraphicsItem::ItemIsFocusable);

    item->setBrush(QBrush(Qt::blue));
    item->setZValue(++frontZ);
    item->setPos(-50+(qrand()%100),-50+(qrand()%100));
    item->setData(ItemId,++seqNum);
    item->setData(ITemDescription,"椭圆");
    scene->addItem(item);
    scene->clearSelection();
    item->setSelected(true);

}

void MainWindow::on_CircularAction_triggered()
{
    QGraphicsEllipseItem *item = new QGraphicsEllipseItem(-50,-50,100,100);
    item->setFlags(QGraphicsItem::ItemIsMovable
                   |QGraphicsItem::ItemIsSelectable
                   |QGraphicsItem::ItemIsFocusable);

    item->setBrush(QBrush(Qt::red));
    item->setZValue(++frontZ);
    item->setPos(-50+(qrand()%100),-50+(qrand()%100));
    item->setData(ItemId,++seqNum);
    item->setData(ITemDescription,"圆");
    scene->addItem(item);
    scene->clearSelection();
    item->setSelected(true);
}

void MainWindow::on_TrianAction_triggered()
{
    QVector<QPoint> v;
    v<<QPoint(0,-60)<<QPoint(-30,0)<<QPoint(30,0);
    QGraphicsPolygonItem *item = new QGraphicsPolygonItem(QPolygonF(v));

    item->setFlags(QGraphicsItem::ItemIsMovable
                   |QGraphicsItem::ItemIsSelectable
                   |QGraphicsItem::ItemIsFocusable);

    item->setBrush(QBrush(Qt::yellow));
    item->setZValue(++frontZ);
    item->setPos(-50+(qrand()%100),-50+(qrand()%100));
    item->setData(ItemId,++seqNum);
    item->setData(ITemDescription,"三角形");
    scene->addItem(item);
    scene->clearSelection();
    item->setSelected(true);
}

void MainWindow::on_TrapAction_triggered()
{

    QVector<QPoint> v;
    v<<QPoint(20,-60)<<QPoint(0,0)<<QPoint(60,0)<<QPoint(40,-60);
    QGraphicsPolygonItem *item = new QGraphicsPolygonItem(QPolygonF(v));

    item->setFlags(QGraphicsItem::ItemIsMovable
                   |QGraphicsItem::ItemIsSelectable
                   |QGraphicsItem::ItemIsFocusable);

    item->setBrush(QBrush(QColor("#51E1BD")));
    item->setZValue(++frontZ);
    item->setPos(-50+(qrand()%100),-50+(qrand()%100));
    item->setData(ItemId,++seqNum);
    item->setData(ITemDescription,"梯形");
    scene->addItem(item);
    scene->clearSelection();
    item->setSelected(true);

}

void MainWindow::on_LineAction_triggered()
{
    QGraphicsLineItem *item = new QGraphicsLineItem(-50,-50,100,100);
    item->setFlags(QGraphicsItem::ItemIsMovable
                   |QGraphicsItem::ItemIsSelectable
                   |QGraphicsItem::ItemIsFocusable);


    item->setPen(QPen(Qt::black));
    item->setZValue(++frontZ);
    item->setPos(-50+(qrand()%100),-50+(qrand()%100));
    item->setData(ItemId,++seqNum);
    item->setData(ITemDescription,"直线");
    scene->addItem(item);
    scene->clearSelection();
    item->setSelected(true);
}

void MainWindow::on_TextAction_triggered()
{

    QString txt = QInputDialog::getText(this,"输入文字","填写文字");
    if(txt.length()<1) return;

    QGraphicsTextItem *item = new QGraphicsTextItem(txt);
    item->setFlags(QGraphicsItem::ItemIsMovable
                   |QGraphicsItem::ItemIsSelectable
                   |QGraphicsItem::ItemIsFocusable);

    item->setFont(QFont("宋体",15));
    item->setZValue(++frontZ);
    item->setPos(-50+(qrand()%100),-50+(qrand()%100));
    item->setData(ItemId,++seqNum);
    item->setData(ITemDescription,"文字");
    scene->addItem(item);
    scene->clearSelection();
    item->setSelected(true);
}

void MainWindow::on_RectAction_triggered()
{
    QGraphicsRectItem *item = new QGraphicsRectItem(-50,-50,100,100);
    item->setFlags(QGraphicsItem::ItemIsMovable
                   |QGraphicsItem::ItemIsSelectable
                   |QGraphicsItem::ItemIsFocusable);

    item->setBrush(QBrush(Qt::gray));
    item->setZValue(++frontZ);
    item->setPos(-50+(qrand()%100),-50+(qrand()%100));
    item->setData(ItemId,++seqNum);
    item->setData(ITemDescription,"矩形");
    scene->addItem(item);
    scene->clearSelection();
    item->setSelected(true);
}

void MainWindow::on_ImageAction_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "open image file",
                                                    ".",
                                                    "Image files (*.bmp *.jpg *.pbm *.pgm *.png *.ppm *.xbm *.xpm);;All files (*.*)");
    if(fileName.isEmpty()) return;

    QPixmap map(fileName);

    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(map);
    item->setFlags(QGraphicsItem::ItemIsMovable
                   |QGraphicsItem::ItemIsSelectable
                   |QGraphicsItem::ItemIsFocusable);


    item->setZValue(++frontZ);
    item->setPos(-50+(qrand()%100),-50+(qrand()%100));
    item->setData(ItemId,++seqNum);
    item->setData(ITemDescription,"图片");
    scene->addItem(item);
    scene->clearSelection();
    item->setSelected(true);


}

void MainWindow::on_DelAction_triggered()
{
    foreach(QGraphicsItem *item,scene->selectedItems())
    {
        scene->removeItem(item);
    }


}

void MainWindow::on_BreakAction_triggered()
{



