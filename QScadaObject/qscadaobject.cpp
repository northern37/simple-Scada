#include "qscadaobject.h"

#include <QMouseEvent>
#include <QDebug>
#include <QApplication>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QPen>
#include <QEvent>

#define RESIZE_FIELD_SIZE 10
#define MIN_WIDGET_SIZE 30
#define RESIZE_AREA(x, y) ((geometry().width() - RESIZE_FIELD_SIZE) < x) && ((geometry().height() - RESIZE_FIELD_SIZE) < y)

QScadaObject::QScadaObject(QWidget *parent) :
    QWidget(parent),
    mInfo{new QScadaObjectInfo(this)},
    mEffect{new QGraphicsDropShadowEffect}
{
    this->initUI();
}

QScadaObject::QScadaObject(QScadaObjectInfo *info, QWidget *parent):
    QWidget(parent),
    mInfo{new QScadaObjectInfo(info)},
    mEffect{new QGraphicsDropShadowEffect}
{
    this->initUI();
}

QScadaObject::~QScadaObject()
{
    delete mInfo;
    delete mEffect;
}

void QScadaObject::setGeometry(int x, int y, int width, int height)
{
    setGeometry(QRect(x, y, width, height));
}

void QScadaObject::setGeometry(const QRect &r)
{
    info()->setGeometry(r);

    QWidget::setGeometry(r);
}

QRect QScadaObject::geometry()
{
    return info()->geometry();
}

void QScadaObject::mouseMoveEvent(QMouseEvent *event)
{
    if (mIsEditable) {
        switch (action()) {
        case QObjectActionMove:{
            move(event->x(), event->y());
            break;
        }
        case QObjectActionResize:{
            resize(event->x(), event->y());
            break;
        }
        case QObjectActionNone: {
            if (RESIZE_AREA(event->x(), event->y())) {
                QApplication::changeOverrideCursor(Qt::SizeFDiagCursor);
            } else if (underMouse()) {
                QApplication::changeOverrideCursor(Qt::OpenHandCursor);
            }
        }
        }
    } else {
        QWidget::mouseMoveEvent(event);
    }
}

void QScadaObject::mousePressEvent(QMouseEvent *event)
{
    if (mIsEditable) {
        if (event->button() == Qt::LeftButton) {
            int lX = event->x();
            int lY = event->y();

            if (RESIZE_AREA(lX, lY)) {
                setAction(QObjectActionResize);
            } else {
                setSelected(true);
                QApplication::changeOverrideCursor(Qt::ClosedHandCursor);
                setAction(QObjectActionMove);

                mPosition.setX(lX);
                mPosition.setY(lY);
            }
        }
    } else {
        QWidget::mousePressEvent(event);
    }
}

void QScadaObject::mouseReleaseEvent(QMouseEvent *event)
{
    if (mIsEditable) {
        (void)event;
        setAction(QObjectActionNone);
        // 松手后恢复为掌状（而非 pop 栈回到箭头）
        QApplication::changeOverrideCursor(Qt::OpenHandCursor);

        // 释放时吸附到 10px 网格
        QRect r = geometry();
        int snapX = ((r.x() + 5) / 10) * 10;
        int snapY = ((r.y() + 5) / 10) * 10;
        if (snapX != r.x() || snapY != r.y()) {
            setGeometry(snapX, snapY, r.width(), r.height());
            info()->setGeometry(QRect(snapX, snapY, r.width(), r.height()));
        }
    } else {
        QWidget::mouseReleaseEvent(event);
    }
}

void QScadaObject::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        emit objectDoubleClicked(this);
    }
}

void QScadaObject::paintEvent(QPaintEvent *e)
{
    QPainter lPainter(this);
    QPen lLinepen(Qt::black);
    lLinepen.setCapStyle(Qt::RoundCap);
    lPainter.setRenderHint(QPainter::Antialiasing,true);
    lPainter.setPen(lLinepen);

    //draw title
    int lX;
    int lY;

    //draw resize dots
    lX = geometry().width()-RESIZE_FIELD_SIZE;
    lY = geometry().height();

    lLinepen.setColor(Qt::darkGray);
    lLinepen.setWidth(1);
    lPainter.setPen(lLinepen);

    if (mIsEditable) {
        for (int i=1; i<=RESIZE_FIELD_SIZE; i++) {
            for (int j=1; j<=i; j++) {
                lPainter.drawPoint(QPoint(lX + 2*i, lY - 2*j));
            }
        }
    }

    if (info()->showBackground()) {
        lLinepen.setWidth(2);
        lPainter.setPen(lLinepen);
        lPainter.drawRoundedRect(0,0,width(), height(),3,3);
    }

    QWidget::paintEvent(e);
}

bool QScadaObject::isEditable() const
{
    return mIsEditable;
}

void QScadaObject::setIsEditable(bool isEditable)
{
    if (!isEditable && mIsEditable) {
        // 退出编辑模式时清除选中态
        mSelected = false;
        if (mEffect) {
            mEffect->setBlurRadius(10);
        }
    }
    mIsEditable = isEditable;
}

void QScadaObject::update()
{
    QWidget::update();

    if (info()->showBackground()) {
        setPalette(QPalette(Qt::white));
        setAutoFillBackground(true);
    } else {
        setPalette(QPalette(Qt::transparent));
        setAutoFillBackground(true);
    }

    setGeometry(info()->geometry());
}

bool QScadaObject::selected() const
{
    return mSelected;
}

void QScadaObject::setSelected(bool selected)
{
    if (mIsEditable) {
        mSelected = selected;

        if (mSelected) {
            emit objectSelected(mInfo->id());

            mEffect->setBlurRadius(50);
        } else {
            mEffect->setBlurRadius(10);
        }
    }
}

QScadaObjectInfo *QScadaObject::info() const
{
    return mInfo;
}

void QScadaObject::setInfo(QScadaObjectInfo *info)
{
    mInfo = info;
}

QScadaObjectAction QScadaObject::action() const
{
    return mAction;
}

void QScadaObject::setAction(const QScadaObjectAction &action)
{
    mAction = action;
}

void QScadaObject::move(int x, int y)
{
    int lX = geometry().x() + x - mPosition.x();
    int lY = geometry().y() + y - mPosition.y();

    // 边界约束：不能拖出父组件（板卡）可视区域
    QWidget *parent = parentWidget();
    if (parent) {
        lX = qBound(0, lX, parent->width()  - geometry().width());
        lY = qBound(0, lY, parent->height() - geometry().height());
    }

    setGeometry(lX, lY, geometry().width(), geometry().height());

    if (lX != 0 || lY != 0) {
        emit objectMove(lX, lY);
    }
}

void QScadaObject::resize(int x, int y)
{
    int newW = std::max(MIN_WIDGET_SIZE, x);
    int newH = std::max(MIN_WIDGET_SIZE, y);

    int lX = newW - geometry().width();
    int lY = newH - geometry().height();

    setGeometry(geometry().x(),
                geometry().y(),
                newW,
                newH);

    repaint();

    if (lX != 0 || lY != 0) {
        emit objectResize(lX, lY);
    }
}

bool QScadaObject::eventFilter(QObject *obj, QEvent *event)
{
    if (mIsEditable && obj->isWidgetType()) {
        QWidget *child = qobject_cast<QWidget*>(obj);
        if (child && child->parentWidget() == this) {
            switch (event->type()) {
            case QEvent::MouseButtonPress: {
                QMouseEvent *me = static_cast<QMouseEvent*>(event);
                QMouseEvent localEvent(me->type(),
                    child->mapToParent(me->pos()), me->globalPosition(),
                    me->button(), me->buttons(), me->modifiers());
                mousePressEvent(&localEvent);
                child->grabMouse();   // 手动抓取，确保后续 Move/Release 到达此 child（return true 阻断了 Qt 隐式抓取）
                return true;
            }
            case QEvent::MouseMove: {
                QMouseEvent *me = static_cast<QMouseEvent*>(event);
                QMouseEvent localEvent(me->type(),
                    child->mapToParent(me->pos()), me->globalPosition(),
                    me->button(), me->buttons(), me->modifiers());
                mouseMoveEvent(&localEvent);
                return true;
            }
            case QEvent::MouseButtonRelease: {
                QMouseEvent *me = static_cast<QMouseEvent*>(event);
                QMouseEvent localEvent(me->type(),
                    child->mapToParent(me->pos()), me->globalPosition(),
                    me->button(), me->buttons(), me->modifiers());
                mouseReleaseEvent(&localEvent);
                child->releaseMouse(); // 释放手动抓取
                return true;
            }
            default: break;
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}

void QScadaObject::initUI()
{
    setGeometry(info()->geometry());
    if (info()->showBackground()) {
        setPalette(QPalette(Qt::transparent));
        setAutoFillBackground(true);
    }

    mEffect->setOffset(0);
    setGraphicsEffect(mEffect);

    setAction(QObjectActionNone);
    setMouseTracking(true);//this not mouseMoveEven is called everytime mouse is moved

    setGeometry(info()->geometry());
}
