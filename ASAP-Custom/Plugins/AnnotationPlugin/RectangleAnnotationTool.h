#ifndef RECTANGLE_ANNOTATION_TOOL_H_
#define RECTANGLE_ANNOTATION_TOOL_H_

#include "AnnotationTool.h"
#include "core/Point.h"

class AnnotationWorkstationExtensionPlugin;
class PathologyViewer;
class QGraphicsLineItem;

//为上海第一人民医院做的扩展，Rectangle表示疑似病变
class RectangleAnnotationTool : public AnnotationTool
{
    Q_OBJECT
public:
    RectangleAnnotationTool(AnnotationWorkstationExtensionPlugin* annotationPlugin, PathologyViewer* viewer);
    virtual std::string name();
    virtual void mouseMoveEvent(QMouseEvent *event);
    //virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual QAction* getToolButton();
protected:
private:
};

#endif 
