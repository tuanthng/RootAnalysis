#ifndef SCROLLAREA_H
#define SCROLLAREA_H

#include <QScrollArea>
#include "pixmapwidget.hpp"
#include "imageviewer.hpp"

namespace ofeli
{

class ScrollArea : public QScrollArea
{
    Q_OBJECT

public:

    ScrollArea(QWidget* parent = 0);

signals :

    void changed(int val,ScrollArea* obj);

protected:

    bool eventFilter(QObject* obj, QEvent* event);
};

}

#endif // SCROLLAREA_H
