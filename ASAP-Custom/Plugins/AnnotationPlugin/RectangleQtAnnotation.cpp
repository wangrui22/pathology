#include "RectangleQtAnnotation.h"
#include "Annotation.h"
#include <QPainter>
#include <QPainterPath>
#include <QPainterPathStroker>
#include <QStyleOptionGraphicsItem>
#include <iostream>
#include "Annotation_define.h"


RectangleQtAnnotation::RectangleQtAnnotation(const std::shared_ptr<Annotation>& annotation, QObject *parent, float scale /*= 1.0*/):
    QtAnnotation(annotation, parent, scale),
    _lineThickness(3),
    _lineAnnotationSelectedThickness(4.5),
    _rectColor(QColor("blue")),
    _rectSelectedColor(QColor("red")),
    _selectionSensitivity(100.0),
    _currentLOD(1.0f)
{
#ifdef SHANGHAI_NO1
    annotation->addTumorType(TUMOR_TYPES[22]);// "SuspectedObject"ÒÉËÆ²¡±ä
#endif // SHANGHAI_NO1
}

QRectF RectangleQtAnnotation::boundingRect() const
{
    QRectF bRect;
    if (_annotation) {
        QRectF cpRect = _currentPath.controlPointRect();
        QPointF tl = cpRect.topLeft() - QPointF(3 * _lineAnnotationSelectedThickness, 3 * _lineAnnotationSelectedThickness);
        QPointF br = cpRect.bottomRight() + QPointF(3 * _lineAnnotationSelectedThickness, 3 * _lineAnnotationSelectedThickness);
        bRect = bRect.united(QRectF(tl, br));
    }
    return bRect;
}

void RectangleQtAnnotation::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (!_annotation) {
        return;
    }

    //QColor lineColor = this->getDrawingColor();
    QColor lineColor = QColor("cyan");
    QColor fillColor = this->getDrawingColor();
    fillColor.setAlphaF(0.3);
    _currentLOD = option->levelOfDetailFromTransform(painter->worldTransform());
    std::vector<Point> coords = _annotation->getCoordinates();
    if (coords.size() <= 1) {
        return;
    }

    //Draw line
    if (isSelected()) {
        painter->setPen(QPen(QBrush(lineColor.lighter(150)), _lineAnnotationSelectedThickness / _currentLOD));
    }
    else {
        painter->setPen(QPen(QBrush(lineColor), _lineThickness / _currentLOD));
    }

    painter->drawPath(_currentPath);


    //Draw points
    if (isSelected()) {
        painter->setPen(QPen(QBrush(_rectColor.lighter(150)), 3 * _lineThickness / _currentLOD, Qt::PenStyle::SolidLine, Qt::PenCapStyle::SquareCap));
    }
    else {
        painter->setPen(QPen(QBrush(_rectColor), 3 * _lineThickness / _currentLOD, Qt::PenStyle::SolidLine, Qt::PenCapStyle::SquareCap));
    }

    for (unsigned int i = 0; i < coords.size(); ++i) {
        if (i == _activeSeedPoint) {
            painter->save();
            painter->setPen(QPen(QBrush(_rectSelectedColor), 3 * _lineAnnotationSelectedThickness / _currentLOD , Qt::PenStyle::SolidLine, Qt::PenCapStyle::SquareCap));
            painter->drawPoint(this->mapFromScene(coords[i].getX()*_scale, coords[i].getY()*_scale));
            painter->restore();
        }
        else {
            painter->drawPoint(this->mapFromScene(coords[i].getX()*_scale, coords[i].getY()*_scale));
        }
    }
}

QPainterPath RectangleQtAnnotation::getCurrentPath(const std::vector<Point>& coords) const
{
    QPainterPath pth;
    if (coords.size() > 1) {
        
        pth.moveTo(this->mapFromScene(coords[0].getX()*_scale, coords[0].getY()*_scale));
        for (unsigned int i = 0; i < coords.size() - 1; ++i) {
            pth.lineTo(this->mapFromScene(coords[i + 1].getX()*_scale, coords[i + 1].getY()*_scale));
        }
        pth.lineTo(this->mapFromScene(coords[0].getX()*_scale, coords[0].getY()*_scale));
    }
    return pth;
}

void RectangleQtAnnotation::onAnnotationChanged()
{
     _currentPath = getCurrentPath(_annotation->getCoordinates());
}

bool RectangleQtAnnotation::contains(const QPointF & point) const
{
    if (!shape().controlPointRect().contains(point)){
        return false;
    } 

    QPointF imgPoint = this->mapToScene(point) / _scale;
    double curSelectionSensitivity = (_selectionSensitivity * _lineAnnotationSelectedThickness / _currentLOD);
    double curSelectionSensitivitySquared = curSelectionSensitivity * curSelectionSensitivity;
    double imgX = imgPoint.x();
    double imgY = imgPoint.y();
    std::vector<Point> coords = _annotation->getCoordinates();
    double minDist = std::numeric_limits<double>::max();
    _lastClickedFirstCoordinateIndex = -1;
    _lastClickedSecondCoordinateIndex = -1;

    // Quickly check if a seed point was hit
    for (unsigned int i = 0; i < coords.size(); ++i) {
        Point pt1 = coords[i];
        double coord1X = pt1.getX(); double coord1Y = pt1.getY();
        double distSquared = pow(imgX - coord1X, 2) + pow(imgY - coord1Y, 2);
        if (distSquared < curSelectionSensitivitySquared && distSquared < minDist) {
            _lastClickedFirstCoordinateIndex = i;
            _lastClickedSecondCoordinateIndex = -1;
            minDist = distSquared;
        }
    }
    if (_lastClickedFirstCoordinateIndex >= 0) {
        return true;
    }

    minDist = std::numeric_limits<double>::max();
    // If not, check if a line was hit
    std::vector<QPointF> polyInImgCoords;
    unsigned int polyIndex = 0;
    for (unsigned int i = 0; i < coords.size(); ++i) {
        Point pt1 = coords[i];
        Point pt2 = i == coords.size() - 1 ? coords[0] : coords[i + 1];
        double coord1X = pt1.getX(); double coord1Y = pt1.getY();
        double coord2X = pt2.getX(); double coord2Y = pt2.getY();
        QRectF hitbox(imgX - curSelectionSensitivity / 2., imgY - curSelectionSensitivity / 2., curSelectionSensitivity * 2., curSelectionSensitivity * 2.);
        QRectF lineBox(QPointF(std::min(coord1X, coord2X) - curSelectionSensitivity / 2., std::max(coord1Y, coord2Y) + curSelectionSensitivity / 2.), QPointF(std::max(coord1X, coord2X) + curSelectionSensitivity / 2., std::min(coord1Y, coord2Y) - curSelectionSensitivity / 2.));
        if (hitbox.intersects(lineBox)) {
            double lineLengthSquared = pow(coord1X - coord2X, 2) + pow(coord1Y - coord2Y, 2);
            double t = ((imgX - coord2X) * (coord1X - coord2X) + (imgY - coord2Y) * (coord1Y - coord2Y)) / lineLengthSquared;
            double projX = coord2X + t * (coord1X - coord2X);
            double projY = coord2Y + t * (coord1Y - coord2Y);
            double distSquared = pow(imgX - projX, 2) + pow(imgY - projY, 2);
            if (distSquared < curSelectionSensitivitySquared && distSquared < minDist) {
                _lastClickedFirstCoordinateIndex = i;
                _lastClickedSecondCoordinateIndex = i + 1 == coords.size() ? 0 : i + 1;
            }
        }
    }
    if (_lastClickedFirstCoordinateIndex < 0) {
        return false;
    }
    else {
        return true;
    }

    return false;
}

bool RectangleQtAnnotation::collidesWithPath(const QPainterPath & path, Qt::ItemSelectionMode mode /*= Qt::IntersectsItemShape*/) const
{
    return contains(path.currentPosition());
}

void RectangleQtAnnotation::moveCoordinateBy(unsigned int index, const Point& moveBy)
{
    if (index > 3){
        return;
    }

    unsigned int fixSeedPoint = index + 2;
    if (fixSeedPoint > 3){
        fixSeedPoint -= 4;
    }
    std::vector<Point> coords = _annotation->getCoordinates();
    const float activeX = coords[index].getX() + moveBy.getX() / _scale;
    const float activeY = coords[index].getY() + moveBy.getY() / _scale;
    coords[index].setX(activeX);
    coords[index].setY(activeY);


    if (0 == index){
        coords[1].setY(activeY);
        coords[3].setX(activeX);
    }
    else if (1 ==index){
        coords[0].setY(activeY);
        coords[2].setX(activeX);
    }
    else if (2 == index){
        coords[3].setY(activeY);
        coords[1].setX(activeX);
    }
    else if (3 == index){
        coords[2].setY(activeY);
        coords[0].setX(activeX);
    }

    _annotation->setCoordinates(coords);

    onAnnotationChanged();
    emit annotationChanged(this);
}
