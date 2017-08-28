#include "QtAnnotation.h"
#include "Annotation.h"
#include "AnnotationGroup.h"

using namespace std;

QtAnnotation::QtAnnotation(const std::shared_ptr<Annotation>& annotation, QObject* parent, float scale) :
QGraphicsItem(),
QObject(parent),
_annotation(annotation),
_scale(scale),
_editable(true),
_activeSeedPoint(-1),
_finished(false),
_lastClickedFirstCoordinateIndex(-1),
_lastClickedSecondCoordinateIndex(-1)
{
  // We consider the first point to act as the 'center' of the annotation
  if (_annotation) {
    Point center = _annotation->getCoordinate(0);
    this->setPos(center.getX()*_scale, center.getY()*_scale);
  }
  this->setFlag(QGraphicsItem::ItemIsSelectable);
}

QtAnnotation::~QtAnnotation() {
  _annotation = NULL;
}

std::shared_ptr<Annotation> QtAnnotation::getAnnotation() const {
  return _annotation;
}

void QtAnnotation::finish() {
  _finished = true;
  onAnnotationChanged();
  emit annotationChanged(this);
}

std::pair<int, int> QtAnnotation::getLastClickedCoordinateIndices() const {
  return std::pair<int, int>(_lastClickedFirstCoordinateIndex, _lastClickedSecondCoordinateIndex);
}

QColor QtAnnotation::getDrawingColor() {
  QColor drawingColor("#F4FA58");
  if (_annotation) {
    std::shared_ptr<AnnotationGroup> grp = _annotation->getGroup();
    if (grp) {
      while (grp->getGroup()) {
        grp = grp->getGroup();
      }
      return QColor(grp->getColor().c_str());
    }
    else {
      return QColor(_annotation->getColor().c_str());
    }
  }
  return drawingColor;
}

void QtAnnotation::addCoordinate(const float& x, const float& y) {
  prepareGeometryChange();
  if (_annotation) {
    _annotation->addCoordinate(x, y);
  }
  onAnnotationChanged();
  emit annotationChanged(this);
}

void QtAnnotation::addCoordinate(const Point& xy) {
  prepareGeometryChange();
  if (_annotation) {
    _annotation->addCoordinate(xy);
  }
  onAnnotationChanged();
  emit annotationChanged(this);
}

void QtAnnotation::insertCoordinate(const int& index, const float& x, const float& y) {
  prepareGeometryChange();
  if (_annotation) {
    _annotation->insertCoordinate(index, x, y);
  }
  onAnnotationChanged();
  emit annotationChanged(this);
}

void QtAnnotation::insertCoordinate(const int& index, const Point& xy){
  prepareGeometryChange();
  if (_annotation) {
    _annotation->insertCoordinate(index, xy);
  }
  onAnnotationChanged();
  emit annotationChanged(this);
}

void QtAnnotation::removeCoordinate(const int& index) {
  prepareGeometryChange();
  if (_annotation) {
    _annotation->removeCoordinate(index);
    // Recenter annotation when anchorpoint is removed
    if (index == 0 && _annotation->getCoordinates().size() > 0) {
      Point center = _annotation->getCoordinate(0);
      this->setPos(center.getX()*_scale, center.getY()*_scale);
    }
  }
  onAnnotationChanged();
  emit annotationChanged(this);
}

void QtAnnotation::setCoordinates(const std::vector<Point>& coordinates) {
  prepareGeometryChange();
  if (_annotation) {
    _annotation->setCoordinates(coordinates);
    // Recenter annotation when anchorpoint is removed
    if (_annotation->getCoordinates().size() > 0) {
      Point center = _annotation->getCoordinate(0);
      this->setPos(center.getX()*_scale, center.getY()*_scale);
    }
  }
  onAnnotationChanged();
  emit annotationChanged(this);
}

void QtAnnotation::moveCoordinateBy(unsigned int index, const Point& moveBy) {
  std::vector<Point> coords = _annotation->getCoordinates();
  if (index < coords.size()) {
    prepareGeometryChange();
    coords[index].setX(coords[index].getX() + moveBy.getX() / _scale);
    coords[index].setY(coords[index].getY() + moveBy.getY() / _scale);
    _annotation->setCoordinates(coords);
    if (index == 0) {
      this->setPos(QPointF(coords[0].getX()*_scale, coords[0].getY()*_scale));
    }
  }
  onAnnotationChanged();
  emit annotationChanged(this);
}

void QtAnnotation::moveCoordinatesBy(const Point& moveBy) {
  std::vector<Point> coords = _annotation->getCoordinates();
  for (std::vector<Point>::iterator it = coords.begin(); it != coords.end(); ++it) {
    it->setX(it->getX() + moveBy.getX() / _scale);
    it->setY(it->getY() + moveBy.getY() / _scale);
  }
  _annotation->setCoordinates(coords);
  this->setPos(QPointF(coords[0].getX()*_scale, coords[0].getY()*_scale));
  onAnnotationChanged();
  emit annotationChanged(this);
}

void QtAnnotation::onAnnotationChanged() {
  return;
}

void QtAnnotation::setActiveSeedPoint(const unsigned int seedPointIndex) {
  prepareGeometryChange();
  if (seedPointIndex < _annotation->getCoordinates().size()) {
    _activeSeedPoint = seedPointIndex;
  }
}

void QtAnnotation::clearActiveSeedPoint() {
  prepareGeometryChange();
  _activeSeedPoint = -1;
}

int QtAnnotation::getActiveSeedPoint() {
  return _activeSeedPoint;
}

void QtAnnotation::setEditable(const bool editable) {
  _editable = editable;
}

bool QtAnnotation::getEditable() const {
  return _editable;
}

QDataStream &operator<<(QDataStream &out, QtAnnotation* const &rhs) {
  out.writeRawData(reinterpret_cast<const char*>(&rhs), sizeof(rhs));
  return out;
}

QDataStream &operator>>(QDataStream &in, QtAnnotation* &rhs) {
  in.readRawData(reinterpret_cast<char*>(&rhs), sizeof(rhs));
  return in;
}