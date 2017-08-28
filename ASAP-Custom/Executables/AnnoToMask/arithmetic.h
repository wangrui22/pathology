#ifndef ARITHMETIC_H
#define ARITHMETIC_H

#include <iostream>
#include <fstream>

template<class T>
class AABB
{
public:
    T _min[2];//Lower Left Back
    T _max[2];//Upper Right Front

public:
    AABB() {};
    virtual ~AABB() {};

    bool operator == (const AABB& aabb) const
    {
        return (_min[0] == aabb._min[0] && _min[1] == aabb._min[1] &&
            _max[0] == aabb._max[0] && _max[1] == aabb._max[1] );
    }

    bool operator != (const AABB& aabb) const
    {
        return (_min[0] != aabb._min[0] || _min[1] != aabb._min[1] ||
            _max[0] != aabb._max[0] || _max[1] != aabb._max[1]);
    }

    void add(int x, int y)
    {
        _min[0] = _min[0] <= x ? _min[0] : x;
        _min[1] = _min[1] <= y ? _min[1] : y;

        _max[0] = _max[0] >= x ? _max[0] : x;
        _max[1] = _max[1] >= y ? _max[1] : y;
    }

    void combine(const AABB<T>& l)
    {
        for (int i = 0; i < 2; ++i)
        {
            this->_max[i] = (std::max)(l._max[i], this->_max[i]);
            this->_min[i] = (std::min)(l._min[i], this->_min[i]);
        }
    }

    double area() const
    {
        return fabs( double(_max[1] - _min[1])*double(_max[0] - _min[0]) );
    }

    bool valid()const
    {
        if (_max[1] <=_min[1] || _max[0] <= _min[0])
        {
            return false;
        }
        else
        {
            return true;
        }
    }
};

template<class T>
bool aabb_to_aabb_cross(const AABB<T>& l, const AABB<T>& r, AABB<T>& result)
{
    if ((l._min[0] > r._max[0] || l._min[1] > r._max[1] ) ||
        (r._min[0] > l._max[0] || r._min[1] > l._max[1] ))
    {
        return false;
    }

    for (int i = 0; i < 2; ++i)
    {
        result._max[i] = (std::min)(l._max[i], r._max[i]);
        result._min[i] = (std::max)(l._min[i], r._min[i]);
    }

    return true;
}

template<class T>
bool aabb_to_aabb_combine(const AABB<T>& l, const AABB<T>& r, AABB<T>& result)
{
    if ((l._min[0] > r._max[0] || l._min[1] > r._max[1]) ||
        (r._min[0] > l._max[0] || r._min[1] > l._max[1]))
    {
        return false;
    }

    for (int i = 0; i < 2; ++i)
    {
        result._max[i] = (std::max)(l._max[i], r._max[i]);
        result._min[i] = (std::min)(l._min[i], r._min[i]);
    }

    return true;
}

template<class T>
bool aabb_contain_aabb(const AABB<T>& larger, const AABB<T>& small)
{
    if (larger._min[0] <= small._min[0] && larger._min[1] <= small._min[1] &&
        larger._max[0] >=  small._max[0] && larger._max[1] >= small._max[1])
    {
        return true;
    }
    else
    {
        return false;
    }
}


int write_raw(const std::string& path, char* buffer, unsigned int length)
{
    if (nullptr == buffer || path.empty()) {
        return -1;
    }

    std::ofstream out(path.c_str(), std::ios::out | std::ios::binary);

    if (!out.is_open()) {
        return -1;
    }


    out.write(buffer, length);
    out.close();

    return 0;
}

int extract_image(unsigned char* raw, int width, int height, int src0, int src1, int w, int h , unsigned char* img_out)
{
    if (src0 +w > width || src1+h > height)
    {
        return -1;
    }

    if (nullptr == img_out)
    {
        return -1;
    }

    for (int y = 0; y < h; ++y)
    {
        int yy = y + src1;
        memcpy(img_out + y*w*3, raw + (yy*width + src0)*3, w*3);
    }

    return 0;
}

#endif