#ifndef QTANNOTATION_H
#define QTANNOTATION_H
#include <string>
#include <vector>
#include <memory>
#include <QGraphicsItem>

//#include "annotationplugin_export.h"

class Annotation;
class Point;

class /*ANNOTATIONPLUGIN_EXPORT*/ QtAnnotation : public QObject, public QGraphicsItem
{
  Q_OBJECT
  Q_INTERFACES(QGraphicsItem)
public:
  QtAnnotation(const std::shared_ptr<Annotation>& annotation, QObject* parent, float scale = 1.);
  virtual ~QtAnnotation();
  void addCoordinate(const float& x, const float& y);
  void addCoordinate(const Point& xy);
  void insertCoordinate(const int& index, const float& x, const float& y);
  void insertCoordinate(const int& index, const Point& xy);
  void removeCoordinate(const int& index);
  void setCoordinates(const std::vector<Point>& coordinates);
  virtual void moveCoordinateBy(unsigned int index, const Point& moveBy);//����ط���virtual��Ϊ����RectangleQtAnnotation�޸Ŀ��Ƶ���߼�����������Annotation
  void moveCoordinatesBy(const Point& moveBy);

	std::shared_ptr<Annotation> getAnnotation() const;
  
  virtual QRectF boundingRect() const = 0;

  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
    QWidget *widget) = 0;

  virtual void finish();

  int getLastClickedPointIndex() const;
  std::pair<int, int> getLastClickedCoordinateIndices() const;

  void setActiveSeedPoint(const unsigned int seedPointIndex);
  void clearActiveSeedPoint();
  int getActiveSeedPoint();

  void setEditable(const bool editable);
  bool getEditable() const;

  QColor getDrawingColor();

signals:
  void annotationChanged(QtAnnotation* annotation);

protected:
	std::shared_ptr<Annotation> _annotation;
  float _scale;
  int _activeSeedPoint;
  bool _editable;
  bool _finished;

  mutable int _lastClickedFirstCoordinateIndex;
  mutable int _lastClickedSecondCoordinateIndex;

  virtual void onAnnotationChanged();

};

QDataStream &operator<<(QDataStream &out, QtAnnotation* const &rhs);
QDataStream &operator>>(QDataStream &in, QtAnnotation* &rhs);
#endif