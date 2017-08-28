#ifndef CONNECTED_DIMAIN_LABEL_H
#define CONNECTED_DIMAIN_LABEL_H

#include <memory>
#include <set>
#include <map>
#include <stack>
#include <vector>
#include <iostream>
#include <cassert>

#include "arithmetic.h"

class ConnectedDomainAnalysis
{
    struct POS
    {
        int x;
        int y;
        POS()
        {

        }
        POS(int xx, int yy) :x(xx), y(yy)
        {

        }
    };

public:
    ConnectedDomainAnalysis()
    {
        memset(_min, 0, sizeof(_min));
        memset(_max, 0, sizeof(_max));
        memset(_dim, 0, sizeof(_dim));
        memset(_roi_dim, 0, sizeof(_roi_dim));
        _mask_ref = nullptr;
        _target_label = 1;
    }

    ~ConnectedDomainAnalysis()
    {

    }

    void set_dim(const int(&dim)[2])
    {
        memcpy(_dim, dim, 2 * sizeof(unsigned int));
    }

    void set_mask_ref(unsigned char* mask_array)
    {
        _mask_ref = mask_array;
    }

    void set_target_label(unsigned char label)
    {
        _target_label = label;
    }

    void set_roi(const int(&min)[2], const int(&max)[2])
    {
        for (int i = 0; i < 2; ++i)
        {
            if (min[i] >= max[i])
            {
                //ARITHMETIC_THROW_EXCEPTION("invalid roi dimension");
            }
            _min[i] = min[i];
            _max[i] = max[i];
            _roi_dim[i] = max[i] - min[i];
        }
        _roi_cache.reset(new unsigned short[_roi_dim[0] * _roi_dim[1] ]);
        for (int y = 0; y < _roi_dim[1]; ++y)
        {
            int yy = y + _min[1];
            for (int x = 0 ; x < _roi_dim[0] ; ++x)
            {
                int xx = x + _min[0];
                _roi_cache.get()[y*_roi_dim[0] + x] = (unsigned short)(_mask_ref[yy*_dim[0] + xx]);
            }
        }

        for (int i = 0; i < _roi_dim[0] * _roi_dim[1] ; ++i)
        {
            if (_roi_cache[i] == _target_label)
            {
                _roi_cache[i] = 1;
            }
            else
            {
                _roi_cache[i] = 0;
            }
        }

        //////////////////////////////////////////////////////////////////////////
        //Test
        //std::cout << "roi dim : " << _roi_dim[0] << " " << _roi_dim[1] << " " << _roi_dim[2] << std::endl;
        //std::ofstream out("D:/temp/roi.raw" , std::ios::binary | std::ios::out);
        //if (out.is_open())
        //{
        //    out.write((char*)_roi_cache.get() , _roi_dim[0]*_roi_dim[1]*_roi_dim[2]);
        //    out.close();
        //}
        //////////////////////////////////////////////////////////////////////////

    };

    void get_9_n(const int x, const int y, std::stack<POS>& s, unsigned short label, int& add)
    {
        const int yyy = y + 2 > _roi_dim[1] ? _roi_dim[1] : y + 2;
        const int xxx = x + 2 > _roi_dim[0] ? _roi_dim[0] : x + 2;

        add = 0;
        std::vector<int> pos_tmp;
        pos_tmp.reserve(26);
        for (int yy = y - 1 < 0 ? 0 : y - 1; yy < yyy; ++yy)
        {
            for (int xx = x - 1 < 0 ? 0 : x - 1; xx < xxx; ++xx)
            {
                if (xx != x || yy != y )
                {
                    int idx =  yy*_roi_dim[0] + xx;;
                    if (_roi_cache.get()[idx] == 1)
                    {
                        s.push(POS(xx, yy));
                        pos_tmp.push_back(idx);
                    }
                }
            }
        }

        add += static_cast<int>(pos_tmp.size());
        for (auto it = pos_tmp.begin(); it != pos_tmp.end(); ++it)
        {
            _roi_cache.get()[*it] = label;
        }
    }

    std::vector<AABB<int>> get_connected_domain_aabb(unsigned int num_th, double border_ratio)
    {
        unsigned short* roi = _roi_cache.get();

        //Seed filing
        std::map<unsigned short, unsigned int> tmp_cd;
        std::stack<POS> cur_cd;
        unsigned int cur_num = 0;
        unsigned short cur_label = 2;
        for (int y = 0; y < _roi_dim[1]; ++y)
        {
            for (int x = 0; x < _roi_dim[0]; ++x)
            {
                unsigned int pos = y*_roi_dim[0] + x;
                unsigned short target_label = roi[pos];
                if (target_label == 1)
                {
                    cur_cd.push(POS(x, y ));
                    seed_filling(cur_cd, cur_label, cur_num);
                    //if (cur_num < num_th)//remove noise points
                    //{
                    //    //rolling back
                    //    for (int iii = 0 ; iii < _roi_dim[0]*_roi_dim[1] ; ++iii)
                    //    {
                    //        if (_roi_cache.get()[iii] == cur_label)
                    //        {
                    //            _roi_cache.get()[iii] = 0;
                    //        }
                    //    }
                    //}
                    //else
                    if(cur_num > 0)
                    {
                        tmp_cd[cur_label] = cur_num;
                        cur_num = 0;
                        cur_label++;
                    }
                }
            }
        }

        //TODO Debug
        //write_raw("D:/temp/cd.raw", (char*)(_roi_cache.get()), _roi_dim[0] * _roi_dim[1] * 2);

        //////////////////////////////////////////////////////////////////////////
        //Test
        //{
        //    int err_0 = 0;
        //    int err_1 = 0;
        //    int err_2 = 0;

        //    for (unsigned int z = 0 ; z< _roi_dim[2]; ++z)
        //    {
        //        for (unsigned int y = 0 ; y < _roi_dim[1] ; ++y)
        //        {
        //            for (unsigned int x = 0 ; x < _roi_dim[0] ; ++x)
        //            {
        //                unsigned int zz = z + _min[2];
        //                unsigned int yy = y + _min[1];
        //                unsigned int xx = x + _min[0];
        //                unsigned int idx0 = z*_roi_dim[0]*_roi_dim[1] + y*_roi_dim[0] +x;
        //                unsigned int idx1 = zz*_dim[0]*_dim[1] + yy*_dim[0] +xx;

        //                unsigned char l0 = _roi_cache.get()[idx0];
        //                unsigned char l1 = _mask_ref[idx1];
        //                if (l0 == 1)
        //                {
        //                    err_0 ++;
        //                    //std::cout << "ERROR 1 \n";
        //                }
        //                if (l0 == 0 && l1 != 0)
        //                {
        //                    err_1++; // Here is isolated point not error
        //                }
        //                if(l0 != 0 && l1 == 0)
        //                {
        //                    err_2 ++;
        //                    //std::cout << "ERROR 1\n";
        //                }
        //            }
        //        }
        //    }
        //    std::cout << "err : " << err_0 << " " << err_1 << " " << err_2 << std::endl;
        //}
        //////////////////////////////////////////////////////////////////////////

        if (tmp_cd.empty())
        {
            return std::vector<AABB<int>>();
        }

        //Calculate precise connect number
        for (auto it = tmp_cd.begin(); it != tmp_cd.end(); ++it)
        {
            it->second = 0;//clear number(has repeated idx)
        }

        for (int i = 0; i < _roi_dim[0] * _roi_dim[1] ; ++i)
        {
            unsigned short l = _roi_cache.get()[i];
            if (l != 0)
            {
                auto it = tmp_cd.find(l);
                if (it != tmp_cd.end())
                {
                    it->second += 1;
                }
            }
        }

        std::map<unsigned short, AABB<int>> aabbs_set;
        for (auto it = tmp_cd.begin(); it != tmp_cd.end();)
        {
            if (it->second < num_th)
            {
                it = tmp_cd.erase(it);
            }
            else
            {
                AABB<int> new_aabb;
                new_aabb._min[0] = std::numeric_limits<int>::max();
                new_aabb._min[1] = std::numeric_limits<int>::max();
                new_aabb._max[0] = std::numeric_limits<int>::min();
                new_aabb._max[1] = std::numeric_limits<int>::min();
                aabbs_set.insert(std::make_pair(it->first, new_aabb));
                ++it;
            }
        }


        for (int yy = 0; yy < _roi_dim[1] ; ++yy)
        {
            for (int xx = 0; xx < _roi_dim[0]; ++xx)
            {
                unsigned short l = _roi_cache.get()[yy*_roi_dim[0] + xx];
                if (l != 0)
                {
                    auto it = aabbs_set.find(l);
                    if (it != aabbs_set.end())
                    {
                        it->second.add(xx, yy);
                    }
                }
            }
        }

        std::vector<AABB<int>> aabbs;
        int discard_border = 0;
        for (auto it = aabbs_set.begin(); it != aabbs_set.end(); ++it)
        {
            double aabb_area = it->second.area();
            double area = (double)tmp_cd[it->first];
            if (area / aabb_area < border_ratio)
            {
                ++discard_border;
                continue;
            }

            //expand 1
            if (it->second._min[0] > 10)
            {
                it->second._min[0] -= 10;
            }
            if (it->second._min[1] > 10)
            {
                it->second._min[1] -= 10;
            }
            if (it->second._max[0] <_roi_dim[0]-11)
            {
                it->second._max[0] += 10;
            }
            if (it->second._min[1] > _roi_dim[1]-11)
            {
                it->second._max[1] += 10;
            }
            aabbs.push_back(it->second);
        }

        std::cout << "Discard border aabb : " << discard_border << std::endl;

        return aabbs;

        //////////////////////////////////////////////////////////////////////////
        //Test
        //std::cout << "roi dim : " << _roi_dim[0] << " " << _roi_dim[1] << " " << _roi_dim[2] << std::endl;
        //std::ofstream out("D:/temp/roi_cd.raw" , std::ios::binary | std::ios::out);
        //if (out.is_open())
        //{
        //    out.write((char*)_roi_cache.get() , _roi_dim[0]*_roi_dim[1]*_roi_dim[2]);
        //    out.close();
        //}
        //////////////////////////////////////////////////////////////////////////
    }

    void seed_filling(std::stack<POS>& s, unsigned short label, unsigned int& cd_num)
    {
        cd_num = 0;
        while (!s.empty())
        {
            POS pos = s.top();
            s.pop();
            int add = 0;
            get_9_n(pos.x, pos.y, s, label, add);
            if (s.empty() && cd_num == 0)
            {
                _roi_cache.get()[ pos.y*_roi_dim[0] + pos.x] = 0;//isolated point
                /*std::cout << "isolated point\n";*/
                break;
            }
            _roi_cache.get()[pos.y*_roi_dim[0] + pos.x] = label;
            cd_num += (add + 1);
        }
    }

protected:
private:
    int _dim[2];
    int _min[2];
    int _max[2];
    int _roi_dim[2];
    unsigned char* _mask_ref;
    unsigned char _target_label;
    std::unique_ptr<unsigned short[]> _roi_cache;
};

#endif