#include "RectangleAnnotationTool.h"
#include "QtAnnotation.h"
#include "AnnotationWorkstationExtensionPlugin.h"
#include <QAction>
#include <QPen>
#include <QGraphicsLineItem>
#include <QGraphicsItem>
#include "RectangleQtAnnotation.h"
#include "Annotation.h"
#include "ASAP/PathologyViewer.h"
#include <math.h>
#include <numeric>
#include <iostream>
#include <QTimeLine>


RectangleAnnotationTool::RectangleAnnotationTool(
    AnnotationWorkstationExtensionPlugin* annotationPlugin, PathologyViewer* viewer):
    AnnotationTool(annotationPlugin, viewer)
{
}

//mouse Move �� Keyboard ���߼��ͻ��಻ͬ
void RectangleAnnotationTool::mouseMoveEvent(QMouseEvent *event)
{
    if (!_viewer) {
        return;
    }

    //////////////////////////////////////////////////////////////////////////
    //Rectangle �����ɺ���ͨ��polygon��ͬ
    if (_generating) {
        const unsigned int  point_num = _annotationPlugin->getGeneratedAnnotation()->getAnnotation()->getNumberOfPoints();
        if (point_num == 4) {

            _annotationPlugin->getGeneratedAnnotation()->removeCoordinate(1);
            _annotationPlugin->getGeneratedAnnotation()->removeCoordinate(1);
            _annotationPlugin->getGeneratedAnnotation()->removeCoordinate(1);
        }

        Point prePos = _annotationPlugin->getGeneratedAnnotation()->getAnnotation()->getCoordinate(0);
        QPointF scenePos = _viewer->mapToScene(event->pos());
        Point curPos(scenePos.x() / _viewer->getSceneScale(), scenePos.y() / _viewer->getSceneScale());
        _annotationPlugin->getGeneratedAnnotation()->addCoordinate(Point(curPos.getX(), prePos.getY()));
        _annotationPlugin->getGeneratedAnnotation()->addCoordinate(curPos);
        _annotationPlugin->getGeneratedAnnotation()->addCoordinate(Point(prePos.getX(), curPos.getY()));
    }
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //������߼��ͻ���һ�������˾���Դ���и������ǵ�ǰ�����QtAnnotation���û��ڻ�ͼ����ײ�õ���  �� QtAnnotationTool���û���Menu��ѡ���Action�����Բ�һ��
    else if (_startSelectionMove && event->modifiers() == Qt::AltModifier) {
        QPointF scenePos = _viewer->mapToScene(event->pos());
        QSet<QtAnnotation*> selected = _annotationPlugin->getSelectedAnnotations();
        for (QSet<QtAnnotation*>::iterator it = selected.begin(); it != selected.end(); ++it) {
            QPointF delta = (scenePos - _moveStart);
            (*it)->moveCoordinatesBy(Point(delta.x(), delta.y()));
        }
        _moveStart = scenePos;
    }
    else if (_startSelectionMove) {
        QPointF scenePos = _viewer->mapToScene(event->pos());
        QtAnnotation* active = _annotationPlugin->getActiveAnnotation();
        if (active && active->getEditable()) {
            int activeSeedPoint = active->getActiveSeedPoint();
            if (activeSeedPoint >= 0) {
                QPointF delta = (scenePos - _moveStart);
                active->moveCoordinateBy(activeSeedPoint, Point(delta.x(), delta.y()));
                _moveStart = scenePos;
            }
        }
        _viewer->scene()->update(_viewer->sceneRect());
    }
    event->accept();

}

void RectangleAnnotationTool::mouseReleaseEvent(QMouseEvent *event)
{
    if (!_viewer)
    {
        return;
    }

    if (_generating) {
        if (_annotationPlugin->getGeneratedAnnotation()->getAnnotation()->getCoordinates().size() == 4)
        {
            _annotationPlugin->finishAnnotation();

            _generating = false;
        }
        //ֻ��һ���㣬����Ҳ��Ϊһ��point��ʽ��rectangle
        else if(_annotationPlugin->getGeneratedAnnotation()->getAnnotation()->getCoordinates().size ()== 1)
        {
            Point prePos = _annotationPlugin->getGeneratedAnnotation()->getAnnotation()->getCoordinate(0);
            QPointF scenePos = _viewer->mapToScene(event->pos());
            Point curPos(scenePos.x() / _viewer->getSceneScale(), scenePos.y() / _viewer->getSceneScale());
            _annotationPlugin->getGeneratedAnnotation()->addCoordinate(Point(curPos.getX(), prePos.getY()));
            _annotationPlugin->getGeneratedAnnotation()->addCoordinate(curPos);
            _annotationPlugin->getGeneratedAnnotation()->addCoordinate(Point(prePos.getX(), curPos.getY()));
            _annotationPlugin->finishAnnotation();

            _generating = false;
        }
        else
        {
            std::cout << "Error\n";
        }

        if (_startSelectionMove) {
            _startSelectionMove = false;
            _moveStart = QPointF(-1, -1);
        }

        event->accept();
    }

    if (_startSelectionMove) {
        _startSelectionMove = false;
        _moveStart = QPointF(-1, -1);
    }

    event->accept();
}

void RectangleAnnotationTool::mouseDoubleClickEvent(QMouseEvent *event)
{

}


QAction* RectangleAnnotationTool::getToolButton()
{
    if (!_button) {
        _button = new QAction("RectangleAnnotation", this);
        _button->setObjectName(QString::fromStdString(name()));
        _button->setIcon(QIcon(QPixmap(":/AnnotationWorkstationExtensionPlugin_icons/rectangle.png")));
    }
    return _button;
}


std::string RectangleAnnotationTool::name()
{
    return std::string("rectangleannotation");
}

