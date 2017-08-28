#ifndef RECTANGLE_ANNOTATION_TOOL_H_
#define RECTANGLE_ANNOTATION_TOOL_H_

#include "AnnotationTool.h"
#include "core/Point.h"

class AnnotationWorkstationExtensionPlugin;
class PathologyViewer;
class QGraphicsLineItem;

//Ϊ�Ϻ���һ����ҽԺ������չ��Rectangle��ʾ���Ʋ���
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
