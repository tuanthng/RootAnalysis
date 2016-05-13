#include <QScrollBar>
#include <QEvent>
#include <QObject>
#include "scrollarea.hpp"
#include<iostream>
namespace ofeli
{

ScrollArea::ScrollArea(QWidget* parent)
    : QScrollArea(parent)
{
    verticalScrollBar()->installEventFilter(this);
    horizontalScrollBar()->installEventFilter(this);
    connect( this, SIGNAL(changed(int,ScrollArea*)), parentWidget(), SLOT(wheel_zoom(int,ScrollArea*)) );
}

bool ScrollArea::eventFilter(QObject* obj, QEvent* event)
{
    if( event->type() == QEvent::Wheel )
    {
        if( obj == verticalScrollBar() || obj == horizontalScrollBar() )
        {
            return QScrollArea::eventFilter(obj,event);
        }
        else
        {
            QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
            emit changed( wheelEvent->delta(), this );

            return true;
        }
    }
    else
    {
        return QScrollArea::eventFilter(obj,event);
    }
}

}
