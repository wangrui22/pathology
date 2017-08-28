#ifndef RENCTANGLE_QT_ANNOTATION_H_
#define RENCTANGLE_QT_ANNOTATION_H_

#include "QtAnnotation.h"
#include <QPoint>

//为上海第一人民医院做的扩展，Rectangle表示疑似病变
class RectangleQtAnnotation : public QtAnnotation
{
    Q_OBJECT
public:
    RectangleQtAnnotation(const std::shared_ptr<Annotation>& annotation, QObject *parent, float scale = 1.0);

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget);

    QPainterPath getCurrentPath(const std::vector<Point>& coords) const;

    //hit test
    bool contains(const QPointF & point) const;
    bool collidesWithPath(const QPainterPath & path, Qt::ItemSelectionMode mode = Qt::IntersectsItemShape) const;

    virtual void moveCoordinateBy(unsigned int index, const Point& moveBy);
protected:
private:
    void onAnnotationChanged();

    QPainterPath _currentPath;
    QColor _rectColor;
    QColor _rectSelectedColor;
    float _lineThickness;
    float _lineAnnotationSelectedThickness;
    float _selectionSensitivity;
    float _currentLOD;
};


#endif 
