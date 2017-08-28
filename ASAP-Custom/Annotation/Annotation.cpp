#include "Annotation.h"
#include "AnnotationGroup.h"
#include "psimpl.h"
#include <limits>
#include <cmath>

const char* Annotation::_typeStrings[7] = { "None", "Dot", "Polygon", "Spline", "PointSet", "Measurement", "Rectangle" };

Annotation::Annotation() :
    _type(Annotation::NONE),
    _coordinates()
{

}

std::string Annotation::getTypeAsString() const {
    return _typeStrings[_type];
}

void Annotation::setTypeFromString(const std::string& type) {
    for (unsigned int i = 0; i < 6; ++i) {
        if (type == std::string(_typeStrings[i])) {
            _type = (Annotation::Type)i;
        }
    }
    _modified = true;
}

float Annotation::getArea() const {
    if (!_coordinates.empty()) {
        double  area = 0.;
        int j = _coordinates.size() - 1;
        for (int i = 0; i < _coordinates.size(); i++) {
            area += (_coordinates[j].getX() + _coordinates[i].getX())*(_coordinates[j].getY() - _coordinates[i].getY());
            j = i;
        }
        return std::abs(area*.5);
    }
    else {
        return 0.0;
    }
}

unsigned int Annotation::getNumberOfPoints() const {
    return _coordinates.size();
}

void Annotation::addTumorType(const std::string& s)
{
    _tumourTypes.insert(s);
}

void Annotation::removeTumorType(const std::string& s)
{
    auto it = _tumourTypes.find(s);
    if (it != _tumourTypes.end())
    {
        _tumourTypes.erase(it);
    }
}

void Annotation::setTumorTypes(const std::set<std::string >& types)
{
    _tumourTypes = types;
}

const std::set<std::string >& Annotation::getTumorTypes() const
{
    return _tumourTypes;
}

void Annotation::addCoordinate(const float& x, const float& y)
{
    _coordinates.push_back(Point(x, y));
    _modified = true;
}

void Annotation::addCoordinate(const Point& xy)
{
    _coordinates.push_back(xy);
    _modified = true;
}

void Annotation::insertCoordinate(const int& index, const Point& xy) {
    if (index < 0) {
        _coordinates.insert(_coordinates.end() - abs(index), xy);
    }
    else {
        _coordinates.insert(_coordinates.begin() + index, xy);
    }
    _modified = true;
}

void Annotation::insertCoordinate(const int& index, const float& x, const float& y) {
    this->insertCoordinate(index, Point(x, y));
    _modified = true;
}

void Annotation::removeCoordinate(const int& index) {
    if (index < 0) {
        _coordinates.erase(_coordinates.end() - abs(index));
    }
    else {
        _coordinates.erase(_coordinates.begin() + index);
    }
    _modified = true;
}

Point Annotation::getCoordinate(const int& index) const
{
    if (index < 0) {
        return *(_coordinates.end() - abs(index));
    }
    else {
        return *(_coordinates.begin() + index);
    }
}

std::vector<Point> Annotation::getCoordinates() const
{
    return _coordinates;
}

void Annotation::setCoordinates(const std::vector<Point>& coordinates)
{
    _coordinates = coordinates;
    _modified = true;
}

void Annotation::clearCoordinates() {
    _coordinates.clear();
    _modified = true;
}

void Annotation::setType(const Annotation::Type& type)
{
    _type = type;
    _modified = true;
}

Annotation::Type Annotation::getType() const
{
    return _type;
}

std::vector<Point> Annotation::getImageBoundingBox() const {
    std::vector<Point> bbox;
    Point topLeft(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    Point bottomRight(std::numeric_limits<float>::min(), std::numeric_limits<float>::min());

    if (_coordinates.empty()) {
        topLeft = Point(0, 0);
        bottomRight = Point(0, 0);
    }
    else {
        for (std::vector<Point>::const_iterator it = _coordinates.begin(); it != _coordinates.end(); ++it) {
            if (it->getX() > bottomRight.getX()) {
                bottomRight.setX(it->getX());
            }
            if (it->getY() > bottomRight.getY()) {
                bottomRight.setY(it->getY());
            }
            if (it->getX() < topLeft.getX()) {
                topLeft.setX(it->getX());
            }
            if (it->getY() < topLeft.getY()) {
                topLeft.setY(it->getY());
            }
        }
    }

    bbox.push_back(topLeft);
    bbox.push_back(bottomRight);
    return bbox;
}

void Annotation::simplify(unsigned int nrPoints, float epsilon) {
    std::vector<float> inPoints;
    std::vector<float> outPoints;
    for (std::vector<Point>::iterator it = _coordinates.begin(); it != _coordinates.end(); ++it) {
        inPoints.push_back(it->getX());
        inPoints.push_back(it->getY());
    }
    std::vector<float>::const_iterator begin = inPoints.begin();
    std::vector<float>::const_iterator end = inPoints.end();
    if (nrPoints > 0) {
        psimpl::simplify_douglas_peucker_n<2>(begin, end, nrPoints, std::back_inserter(outPoints));
    }
    else {
        psimpl::simplify_douglas_peucker<2>(begin, end, epsilon, std::back_inserter(outPoints));
    }
    _coordinates.clear();
    for (std::vector<float>::iterator it = outPoints.begin(); it != outPoints.end(); ++it) {
        float x = *it;
        ++it;
        float y = *it;
        _coordinates.push_back(Point(x, y));
    }
    _modified = true;
}

Point Annotation::getCenter() {
    Point center(0, 0);
    if (!_coordinates.empty()) {
        std::vector<Point> bbox = getImageBoundingBox();
        center.setX((bbox[0].getX() + bbox[1].getX()) / 2.);
        center.setY((bbox[0].getY() + bbox[1].getY()) / 2.);
    }
    return center;
}

std::vector<Point> Annotation::getLocalBoundingBox() {
    Point center = getCenter();
    std::vector<Point> bbox = getImageBoundingBox();
    bbox[0].setX(bbox[0].getX() - center.getX());
    bbox[1].setX(bbox[1].getX() - center.getX());
    bbox[0].setY(bbox[0].getY() - center.getY());
    bbox[1].setY(bbox[1].getY() - center.getY());
    return bbox;
}
