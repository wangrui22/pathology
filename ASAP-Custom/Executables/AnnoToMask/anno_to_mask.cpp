#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <vector>
#include <list>

#include "gl/glew.h"
#include "gl/freeglut.h"
#include "gl/GLU.h"

#include "boost/thread/mutex.hpp"
#include "boost/filesystem.hpp"

#include "Annotation/AnnotationList.h"
#include "Annotation/Annotation.h"
#include "Annotation/AnnotationGroup.h"
#include "Annotation/BinaryRepository.h"

#include "io/multiresolutionimageinterface/MultiResolutionImage.h"
#include "io/multiresolutionimageinterface/MultiResolutionImageFactory.h"
#include "io/openslidefileformat/OpenSlideImageFactory.h"
#include "io/openslidefileformat/OpenSlideImage.h"

#include "scan_line_analysis.h"
#include "arithmetic.h"
#include "connected_domain_analysis.h"
#include "segment_analysis.h"
#include "morphology.h"
#include "jpeg_parser.h"

#include <time.h>

//#include "Ext/pugixml/pugiconfig.hpp"
//#include "Ext/pugixml/pugixml.hpp"

int _width = 512;
int _height = 512;
int _img_width = 0;
int _img_height = 0;

unsigned char* _vis_img_buffer = nullptr;
double _pp_ratio = 1;

std::vector<std::shared_ptr<Annotation>> _vis_annos;
std::vector<AABB<int>> _vis_annos_aabbs;
std::vector<AABB<int>> _vis_c_aabbs;
std::vector<AABB<int>> _vis_e_aabbs;
std::vector<AABB<int>> _vis_img_cd_aabbs;
std::vector<AABB<int>> _vis_extract_aabbs;
bool _vis_annos_flag = true;
bool _vis_annos_aabbs_flag = true;
bool _vis_c_aabbs_flag = true;
bool _vis_e_aabbs_flag = true;
bool _vis_img_cd_aabbs_flag = true;
bool _vis_extract_aabbs_flag = true;


static std::ofstream out_log;
class LogSheild
{
public:
    LogSheild(const std::string& log_file, const std::string& start_word)
    {
        out_log.open("anon_to_mask.log", std::ios::out);
        if (out_log.is_open())
        {
            out_log << start_word;
        }
    }
    ~LogSheild()
    {
        out_log.close();
    }
protected:
private:
};

boost::mutex _mutex;

#define LOG_OUT(info) \
{\
    boost::mutex::scoped_lock locker(_mutex);\
    std::cout << info; out_log << info;\
}\

//////////////////////////////////////////////////////////////////////////

void get_all_files(const std::string& root, std::vector<std::string>&file_paths , const std::set< std::string >&postfix)
{
    if (root.empty())
    {
        return;
    }
    else
    {
        std::vector<std::string> dirs;
        for (boost::filesystem::directory_iterator it(root) ; it != boost::filesystem::directory_iterator() ; ++it)
        {
            if (boost::filesystem::is_directory(*it))
            {
                dirs.push_back((*it).path().filename().string());
            }
            else
            {
                const std::string ext = boost::filesystem::extension(*it);
                if (postfix.find(ext) != postfix.end())
                {
                    file_paths.push_back(root + "/" + (*it).path().filename().string());
                }
            }
        }

        for (unsigned int i = 0; i < dirs.size(); ++i)
        {
            const std::string next_dir(root + "/" + dirs[i]);
            get_all_files(next_dir, file_paths, postfix);
        }
    }
}

int load_image(const std::string& file, MultiResolutionImage*& img)
{
    img = MultiResolutionImageFactory::openImage(file);
    if (img)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

int load_anno(const std::string& file , std::shared_ptr<AnnotationList>& anno_list)
{
    anno_list.reset(new AnnotationList());
    BinaryRepository rep(anno_list);
    rep.setSource(file);
    if (rep.load())
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

int get_proper_image(MultiResolutionImage* img , 
    int &pp_level , int (&pp_dim)[2] , double& pp_ratio , unsigned char* &pp_img_gray)
{
    const double pp_size = 800;
    pp_level = 0;
    double dis = std::numeric_limits<double>::max();
    for (int level = 0; level< img->getNumberOfLevels() ; ++level)
    {
        std::vector<unsigned long long> dims = img->getLevelDimensions(level);
        unsigned long long max_dim = (std::max)(dims[0], dims[1]);
        const double cur_dis = fabs(static_cast<double>(max_dim) - static_cast<double>(pp_size));
        if (cur_dis < dis)
        {
            dis = cur_dis;
            pp_level = level;
            pp_dim[0] = static_cast<int>(dims[0]);
            pp_dim[1] = static_cast<int>(dims[1]);
        }
    }

    pp_ratio = img->getLevelDownsample(pp_level);

    if ((std::max)(pp_dim[0], pp_dim[1]) > 4096)
    {
        LOG_OUT("WARNING : get proper level image dim is larget than 4096!");
    }

    unsigned char* pp_img_rgb = nullptr;
    img->getRawRegion(0, 0 , pp_dim[0], pp_dim[1], pp_level, pp_img_rgb);
    if (nullptr == pp_img_rgb)
    {
        LOG_OUT("ERROR : get proper level image failed!");
        return -1;
    }

    if (pp_img_gray != nullptr)
    {
        delete[] pp_img_gray;
        pp_img_gray = nullptr;
    }
    pp_img_gray = new unsigned char[pp_dim[0] * pp_dim[1]];
    for (int i = 0 ; i< pp_dim[0]*pp_dim[1] ; ++ i)
    {
        //0.2989 * R + 0.5870 * G + 0.1140 * B 
        double gray = static_cast<double>(pp_img_rgb[i * 3]) *0.2989 +
            static_cast<double>(pp_img_rgb[i * 3 + 1])*0.5870 +
            static_cast<double>(pp_img_rgb[i * 3 + 2])*0.1140;
        gray = gray > 255.0 ? 255.0 : gray;
        pp_img_gray[i] = static_cast<unsigned char>(gray);
    }

    //TODO DEBUG
    //write_raw("D:/temp/pimg.rgb", (char*)pp_img_rgb, pp_dim[0] * pp_dim[1] * 3);
    //write_raw("D:/temp/pimg.gray", (char*)pp_img_gray, pp_dim[0] * pp_dim[1]);

    delete[] pp_img_rgb;
    pp_img_rgb = nullptr;
    return 0;
}

AABB<int> get_anno_aabb(std::shared_ptr<Annotation> anno, double ratio)
{
    std::vector<Point> img_bounding = anno->getImageBoundingBox();

    AABB<int> aabb;
    aabb._min[0] = static_cast<int>(img_bounding[0].getX() / ratio ) -1;
    aabb._min[1] = static_cast<int>(img_bounding[0].getY() / ratio ) -1;
    aabb._max[0] = static_cast<int>(img_bounding[1].getX() / ratio ) + 1;
    aabb._max[1] = static_cast<int>(img_bounding[1].getY() / ratio ) + 1;

    return aabb;
}

std::vector <AABB<int>> combine_aabb(const std::vector<AABB<int>>& aabbs , int sensitive_border)
{
    std::vector<AABB<int>> c_aabbs = aabbs;
    //for (int i = 0; i < (int)aabbs.size(); ++i)
    //{
    //    std::cout << "w : " << aabbs[i]._max[0] - aabbs[i]._min[0] << "  h : " << aabbs[i]._max[1] - aabbs[i]._min[1] << std::endl;
    //}
    bool no_combine = false;
    while (!no_combine)
    {
        no_combine = true;

        for (int i = 0 ; i < (int)c_aabbs.size() ; ++i)
        {
            for (int j = i +1 ; j < (int)c_aabbs.size(); ++j)
            {
                AABB<int> com_aabb;
                if (aabb_to_aabb_combine<int>(c_aabbs[i], c_aabbs[j], com_aabb))
                {
                    std::vector<AABB<int>> new_aabbs;
                    new_aabbs.push_back(com_aabb);
                    for (int k= 0 ; k < (int)c_aabbs.size() ; ++k)
                    {
                        if (k == i || k== j)
                        {
                            continue;
                        }
                        new_aabbs.push_back(c_aabbs[k]);
                    }
                    std::swap(c_aabbs, new_aabbs);
                    no_combine = false;
                    goto HAS_COMBINE;
                }
                else
                {
                    float c0[2] = {
                        (c_aabbs[i]._min[0] + c_aabbs[i]._max[0])*0.5f,
                        (c_aabbs[i]._min[1] + c_aabbs[i]._max[1])*0.5f};
                    float c1[2] ={
                        (c_aabbs[j]._min[0] + c_aabbs[j]._max[0])*0.5f,
                        (c_aabbs[j]._min[1] + c_aabbs[j]._max[1])*0.5f };
                    const float dis = sqrt((c0[0] - c1[0])*(c0[0] - c1[0]) + (c0[1] - c1[1])*(c0[1] - c1[1]));
                    if (dis < (float)sensitive_border)
                    {
                        for (int q = 0; q< 2; ++q)
                        {
                            com_aabb._max[q] = (std::max)(c_aabbs[i]._max[q], c_aabbs[j]._max[q]);
                            com_aabb._min[q] = (std::min)(c_aabbs[i]._min[q], c_aabbs[j]._min[q]);
                        }

                        std::vector<AABB<int>> new_aabbs;
                        new_aabbs.push_back(com_aabb);
                        for (int k = 0; k < (int)c_aabbs.size(); ++k)
                        {
                            if (k == i || k == j)
                            {
                                continue;
                            }
                            new_aabbs.push_back(c_aabbs[k]);
                        }
                        std::swap(c_aabbs, new_aabbs);
                        no_combine = false;
                        goto HAS_COMBINE;
                    }
                }
                
            }
        }

    HAS_COMBINE:;

    }

    return c_aabbs;
}

bool op_less(const AABB<int>& l, const AABB<int>& r)
{
    return l.area() < r.area();
}

std::vector <AABB<int>> combine_aabb_sort(const std::vector<AABB<int>>& aabbs, int sensitive_border, double area_limit)
{
    std::vector<AABB<int>> c_aabbs;
    for (int i = 0 ; i<aabbs.size() ; ++i)
    {
        c_aabbs.push_back(aabbs[i]);
    }
    std::sort(c_aabbs.begin(), c_aabbs.end(), op_less);

    bool no_combine = false;
    while (!no_combine)
    {
        no_combine = true;

        for (int i = 0; i < (int)c_aabbs.size(); ++i)
        {
            for (int j = i + 1; j < (int)c_aabbs.size(); ++j)
            {
                AABB<int> com_aabb;
                if (aabb_to_aabb_combine<int>(c_aabbs[i], c_aabbs[j], com_aabb))
                {
                    if (com_aabb.area() > area_limit)
                    {
                        continue;
                    }

                    std::vector<AABB<int>> new_aabbs;
                    new_aabbs.push_back(com_aabb);
                    for (int k = 0; k < (int)c_aabbs.size(); ++k)
                    {
                        if (k == i || k == j)
                        {
                            continue;
                        }
                        new_aabbs.push_back(c_aabbs[k]);
                    }
                    std::swap(c_aabbs, new_aabbs);
                    std::sort(c_aabbs.begin(), c_aabbs.end(), op_less);
                    no_combine = false;
                    goto HAS_COMBINE;
                }
                else
                {
                    float c0[2] = {
                        (c_aabbs[i]._min[0] + c_aabbs[i]._max[0])*0.5f,
                        (c_aabbs[i]._min[1] + c_aabbs[i]._max[1])*0.5f };
                    float c1[2] = {
                        (c_aabbs[j]._min[0] + c_aabbs[j]._max[0])*0.5f,
                        (c_aabbs[j]._min[1] + c_aabbs[j]._max[1])*0.5f };
                    const float dis = sqrt((c0[0] - c1[0])*(c0[0] - c1[0]) + (c0[1] - c1[1])*(c0[1] - c1[1]));
                    if (dis < (float)sensitive_border)
                    {
                        for (int q = 0; q < 2; ++q)
                        {
                            com_aabb._max[q] = (std::max)(c_aabbs[i]._max[q], c_aabbs[j]._max[q]);
                            com_aabb._min[q] = (std::min)(c_aabbs[i]._min[q], c_aabbs[j]._min[q]);
                        }

                        if (com_aabb.area() > area_limit)
                        {
                            continue;
                        }

                        std::vector<AABB<int>> new_aabbs;
                        new_aabbs.push_back(com_aabb);
                        for (int k = 0; k < (int)c_aabbs.size(); ++k)
                        {
                            if (k == i || k == j)
                            {
                                continue;
                            }
                            new_aabbs.push_back(c_aabbs[k]);
                        }
                        std::swap(c_aabbs, new_aabbs);
                        no_combine = false;
                        goto HAS_COMBINE;
                    }
                }

            }
        }

    HAS_COMBINE:;

    }

    return c_aabbs;

}

//std::vector <AABB<int>> expand_aabb(const std::vector<AABB<int>>& aabbs, int (&dim)[2] , double expand_ratio)
//{
//    std::vector<AABB<int>> ex_aabbs;
//    ex_aabbs.reserve(aabbs.size());
//    for (size_t i = 0 ; i< aabbs.size() ; ++i)
//    {
//        const AABB<int>& aabb = aabbs[i];
//        double c0[2] = {
//            (aabbs[i]._min[0] + aabbs[i]._max[0])*0.5,
//            (aabbs[i]._min[1] + aabbs[i]._max[1])*0.5 };
//        double w = aabbs[i]._max[0] - aabbs[i]._min[0];
//        double h = aabbs[i]._max[1] - aabbs[i]._min[1];
//        w *= expand_ratio;
//        h *= expand_ratio;
//
//        AABB<int> e_aabb;
//        e_aabb._min[0] = (int)c0[0] - (int)(w*0.5);
//        e_aabb._max[0] = (int)c0[0] + (int)(w*0.5);
//        e_aabb._min[1] = (int)c0[1] - (int)(h*0.5);
//        e_aabb._max[1] = (int)c0[1] + (int)(h*0.5);
//
//        e_aabb._min[0] = e_aabb._min[0] < 0 ? 0 : e_aabb._min[0];
//        e_aabb._min[1] = e_aabb._min[1] < 0 ? 0 : e_aabb._min[1];
//
//        e_aabb._max[0] = e_aabb._max[0] > dim[0]-1 ? dim[0] - 1 : e_aabb._max[0];
//        e_aabb._max[1] = e_aabb._max[1] > dim[1] - 1 ? dim[1] - 1 : e_aabb._max[1];
//
//        ex_aabbs.push_back(e_aabb);
//    }
//
//    return ex_aabbs;
//}

 int get_image_connected_domain_aabb(int(&dim)[2] , unsigned char* img_gray , std::vector<AABB<int>>c_aabb ,
    int sensitive_cd_border, int filter_cd_th , double cd_border_ratio , std::vector<AABB<int>>&  c_img_aabb)
{
    //1 get mask
    SegmentAnalysis seg;
    double th= 0;
    for (auto it = c_aabb.begin() ; it != c_aabb.end() ; ++it )
    {
        th += (double)(seg.get_threshold_otus_low(img_gray, dim[0], dim[1], *it));
    }
    th = th / (double)(c_aabb.size());
    if (th > 255)
    {
        //ERROR
    }
    std::cout << "thresold is : " << th << std::endl;

    std::unique_ptr<unsigned char[]> mask_t(new unsigned char[dim[0] * dim[1]]);
    unsigned char* mask = mask_t.get();
    seg.segment_low(img_gray, mask, dim[0], dim[1] , (unsigned char)th);

    Morphology mor;
    mor.erose(mask, dim[0], dim[1], 1);
    mor.dilate(mask, dim[0], dim[1], 1);

    mor.erose(mask, dim[0], dim[1], 1);
    mor.dilate(mask, dim[0], dim[1], 1);

    mor.erose(mask, dim[0], dim[1], 1);
    mor.dilate(mask, dim[0], dim[1], 1);

    //mor.dilate(mask, dim[0], dim[1], 3);

    //TODO DEBUG
    //write_raw("D:/temp/pmask.raw", (char*)mask, dim[0] * dim[1]);

    seg.remove_border(img_gray, dim[0], dim[1], mask, 10 , th);

    //write_raw("D:/temp/pmask_remove_border.raw", (char*)mask, dim[0] * dim[1]);

    //2 get connected_domain
    ConnectedDomainAnalysis cd;
    cd.set_dim(dim);
    cd.set_mask_ref(mask);

    int roi_min[2] = { 0,0 };
    int roi_max[2] = {dim[0],dim[1]};
    cd.set_roi(roi_min, roi_max);
    std::vector<AABB<int>> c_img_aabb_tmp = cd.get_connected_domain_aabb(filter_cd_th , cd_border_ratio);

    c_img_aabb = combine_aabb(c_img_aabb_tmp, sensitive_cd_border);
    if ((double)c_img_aabb.size() / (double)c_img_aabb_tmp.size() < 0.2 || c_img_aabb.size() < 10)
    {
        //combine to much
        LOG_OUT("WARNING : combine image connecetd domain disaster . the image may be bias . using trick way.")
        c_img_aabb = combine_aabb_sort(c_img_aabb_tmp, sensitive_cd_border, dim[0] * dim[1] / 16.0);
    }

    //check border removal failed
    if (c_img_aabb.size() == 1)
    {
        if((c_img_aabb[0]._min[0] == 0 && c_img_aabb[0]._min[1] == 0) ||
            (c_img_aabb[0]._max[0] == dim[0]-1 && c_img_aabb[0]._min[1] == dim[1]-1) )
        {
            LOG_OUT("ERROR : image border disaster")
            return -1;
        }
    }

    return 0;
}

int get_extract_region_aabbs( const std::vector<AABB<int>>& anno_aabbs , const std::vector<AABB<int>>& c_aabbs , std::vector<AABB<int>>& img_cd_aabbs , std::vector<AABB<int>>& ext_aabbs)
{
    ext_aabbs.clear();
    for (auto it = c_aabbs.begin() ; it != c_aabbs.end() ; ++it)
    {
        const AABB<int>& t_aabb = *it;
        double anno_img_ratio = 0;
        std::vector<AABB<int>*> cross_cd_aabbs;
        AABB<int>* major_cross_aabb = nullptr;
        for (auto it2 = img_cd_aabbs.begin(); it2 != img_cd_aabbs.end() ; ++it2)
        {
            AABB<int> result;
            if (aabb_to_aabb_cross(t_aabb , *it2 , result))
            {
                double cur_ratio0 = (double)result.area() / (double)(*it2).area();
                //double cur_ratio1 = (double)result.area() / (double)(t_aabb).area();
                if (cur_ratio0 > anno_img_ratio )
                {
                    major_cross_aabb = &(*it2);
                    anno_img_ratio = cur_ratio0;
                }
                cross_cd_aabbs.push_back(&(*it2));//TODO Here combine annotation with image cd will be better ?
            }
        }

        if (major_cross_aabb == nullptr)
        {
            return -1;
        }
        else
        {
            AABB<int> c_aabb = t_aabb;
            for (auto it3 = cross_cd_aabbs.begin() ; it3 != cross_cd_aabbs.end() ; ++it3)
            {
                c_aabb.combine(*(*it3));
            }
            
            //remove repeated
            bool has_repeat = false;
            for (auto it4 = ext_aabbs.begin() ; it4 != ext_aabbs.end() ; ++it4)
            {
                if (c_aabb == *it4)
                {
                    has_repeat = true;
                    break;
                }
            }
            if (!has_repeat)
            {
                ext_aabbs.push_back(c_aabb);
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //check 
    std::set<int> anno_record;
    for (auto it = ext_aabbs.begin(); it != ext_aabbs.end(); ++it)
    {
        const AABB<int>& aabb = *it;
        if (!aabb.valid())
        {
            LOG_OUT("WARNING : zero extract aabb.");
            continue;
        }

        for (int i_anno = 0; i_anno < (int)anno_aabbs.size(); ++i_anno)
        {
            AABB<int> aabb_anno = anno_aabbs[i_anno];
            if (aabb_contain_aabb(aabb, aabb_anno))
            {
                anno_record.insert(i_anno);
            }
        }
    }

    if (anno_record.size() != anno_aabbs.size())
    {
        std::cout << "WARNING : just process " << anno_record.size() << " , all annotation is " << anno_aabbs.size() << std::endl;
    }


    return 0;
}

void set_vis_image_buffer(int(&pp_dim)[2],  unsigned char* pp_img_gray)
{
    _img_width = pp_dim[0];
    _img_height = pp_dim[1];

    if (nullptr != _vis_img_buffer)
    {
        delete[] _vis_img_buffer;
        _vis_img_buffer = nullptr;
    }

    const int img_size = _img_width * _img_height;
    _vis_img_buffer = new unsigned char[img_size * 3];
    for (int i = 0; i < img_size; ++i)
    {
        _vis_img_buffer[i * 3] = pp_img_gray[i];
        _vis_img_buffer[i * 3 + 1] = pp_img_gray[i];
        _vis_img_buffer[i * 3 + 2] = pp_img_gray[i];
    }
}

std::vector<Point> catmull_rom_to_bezier(const std::vector<Point>& pts)
{
    std::vector<Point> bezier_pts;
    for (int i = 0; i < (int)pts.size(); ++i)
    {
        Point p1 = pts[i];
        Point p2;
        if (i == (int)(pts.size())-1)
        {
            p2 = pts[0];
        }
        else
        {
            p2 = pts[i + 1];
        }

        Point p0 = p1 - (p2 - p1);
        if (i == 0 )
        {
            p0 = pts[pts.size() - 1];
        }
        else
        {
            p0 = pts[i - 1];
        }

        Point p3 = p2 + (p2 - p1);
        if (i == (int)(pts.size()) - 1)
        {
            p3 = pts[1];
        }
        else if (i == (int)(pts.size()) - 2)
        {
            p3 = pts[0];
        }
        else
        {
            p3 = pts[i + 2];
        }

        //bezier_pts.push_back(p1);

        float d1 = sqrt(pow(p1.getX() - p0.getX(), 2) + pow(p1.getY() - p0.getY(), 2));
        float d2 = sqrt(pow(p2.getX() - p1.getX(), 2) + pow(p2.getY() - p1.getY(), 2));
        float d3 = sqrt(pow(p3.getX() - p2.getX(), 2) + pow(p3.getY() - p2.getY(), 2));
        if (fabs(d1)< 1e-6 || fabs(d2)< 1e-6 || fabs(d3)< 1e-6)
        {
            bezier_pts.push_back(p2);
            continue;
        }

        float rd1 = sqrt(d1);
        float rd2 = sqrt(d2);
        float rd3 = sqrt(d3);
        float B1X = (d1*p2.getX() - d2*p0.getX() + (2 * d1 + 3 * rd1*rd2 + d2)*p1.getX()) / (3 * rd1*(rd1 + rd2));
        float B1Y = (d1*p2.getY() - d2*p0.getY() + (2 * d1 + 3 * rd1*rd2 + d2)*p1.getY()) / (3 * rd1*(rd1 + rd2));
        float B2X = (d3*p1.getX() - d2*p3.getX() + (2 * d3 + 3 * rd3*rd2 + d2)*p2.getX()) / (3 * rd3*(rd3 + rd2));
        float B2Y = (d3*p1.getY() - d2*p3.getY() + (2 * d3 + 3 * rd3*rd2 + d2)*p2.getY()) / (3 * rd3*(rd3 + rd2));

        bezier_pts.push_back(Point(B1X, B1Y));
        bezier_pts.push_back(Point(B2X, B2Y));
        bezier_pts.push_back(p2);
    }

    return bezier_pts;
}

int extract_mask_image(MultiResolutionImage* img , std::vector<std::shared_ptr<Annotation>> annos , const std::string& output_dir , const std::string& base_name,
    std::vector<AABB<int>> extract_aabbs  , int pp_level , double pp_ratio)
{
    int error_flag = 0;
#pragma omp parallel for
    for (int i = 0; i < img->getNumberOfLevels(); ++i)
    {
        std::set<int> anno_record;

        const int level = i;
        const double ratio = img->getLevelDownsample(level);
        std::vector<unsigned long long> dim = img->getLevelDimensions(level);

        //////////////////////////////////////////////////////////////////////////
        //whole raw image for extract image no error
        unsigned char* raw_image_cur_level = nullptr;
        if (level != 0)
        {
            img->getRawRegion(0, 0, dim[0], dim[1], level, raw_image_cur_level);
            if (raw_image_cur_level == nullptr)
            {
                LOG_OUT("ERROR : get raw region failed!");
                //return -1;
            }
        }
        //////////////////////////////////////////////////////////////////////////

        for (auto it = extract_aabbs.begin(); it != extract_aabbs.end(); ++it)
        {
            const AABB<int>& aabb_pp = *it;
            if (!aabb_pp.valid())
            {
                LOG_OUT("WARNING : zero extract aabb.");
                continue;
            }
            AABB<int> aabb = aabb_pp;
            aabb._min[0] = int(aabb._min[0] * pp_ratio);
            aabb._min[1] = int(aabb._min[1] * pp_ratio);
            aabb._max[0] = int(aabb._max[0] * pp_ratio);
            aabb._max[1] = int(aabb._max[1] * pp_ratio);

            const int src_x0 = int(aabb._min[0]  / ratio );
            const int src_x1 = int(aabb._max[0] / ratio);
            const int src_y0 = int(aabb._min[1]  / ratio);
            const int src_y1 = int(aabb._max[1] / ratio);
            const int width = src_x1 - src_x0;
            const int height = src_y1 - src_y0;

            std::stringstream ss;
            ss << output_dir << "/" << base_name << "_"<<level << "_" <<
                src_x0 << "_" << src_y0 << "_" << width << "_" << height;
            const std::string name = ss.str();

            std::stringstream ss2;
            ss2 << "Level : " << level << "'s region min : " << src_x0 << " " << src_y0 << " "
                << "  dimension : " << width << " " << height << std::endl;
            LOG_OUT(ss2.str());

            //////////////////////////////////////////////////////////////////////////
            //Image
            //TODO Here openslide interface result of other level is wrong ! TODO check
            unsigned char* img_data = nullptr;
            if (level == 0)
            {
                /*unsigned char* */img_data = nullptr;
                img->getRawRegion(src_x0, src_y0, width, height, level, img_data);
                if (img_data == nullptr)
                {
                    LOG_OUT("ERROR : get raw region  failed!");
                    error_flag -= 1;
                    continue;
                    //return -1;
                }
                write_raw(name + std::string(".rbg"), (char*)img_data, width*height * 3);
                //delete[] img_data;
                //img_data = nullptr;
            }
            else
            {
                /*unsigned char* */img_data = new unsigned char[width*height * 3];
                if (0 != extract_image(raw_image_cur_level, dim[0], dim[1], src_x0, src_y0, width, height, img_data))
                {
                    delete[] raw_image_cur_level;
                    LOG_OUT("ERROR : extract image from other level failed!");
                    error_flag -= 1;
                    continue;
                    //return -1;
                }

                write_raw(name + std::string(".rbg"), (char*)img_data, width*height * 3);
                

                /*delete[] img_data;
                img_data = nullptr;*/
            }

            //////////////////////////////////////////////////////////////////////////
            //Mask
            std::unique_ptr<unsigned char[]> mask_t(new unsigned char[width*height]);
            unsigned char* mask = mask_t.get();
            memset(mask, 0, width*height);
            for (int i_anno = 0 ; i_anno <(int)annos.size() ; ++i_anno)
            {
                std::shared_ptr<Annotation> anno = annos[i_anno];
                AABB<int> aabb_anno = get_anno_aabb(anno, 1);
                if (aabb_contain_aabb(aabb, aabb_anno))
                {
                    typedef ScanLineAnalysis<unsigned char>::Pt2 PointS;
                    std::vector<Point>pts = anno->getCoordinates();
                    if (pts.size() < 2)
                    {
                        continue;
                    }
                    if (anno->getType() == Annotation::SPLINE)
                    {
                        pts = catmull_rom_to_bezier(pts);
                    }

                    std::vector <PointS > pts2;
                    for (int jj = 0 ; jj <(int)pts.size() ; ++jj )
                    {
                        int coord_x = int(pts[jj].getX() / ratio) - src_x0;
                        int coord_y = int(pts[jj].getY() / ratio) - src_y0;
                        if (coord_x < -9 || coord_y < -9 || coord_x > width + 8 || coord_y > height + 8)
                        {
                            LOG_OUT("WARNING : scan coordinate overflow more than 10 pixel!\n");
                        }
                        if (coord_x< 0)
                        {
                            coord_x = 0;
                        }
                        if (coord_y < 0)
                        {
                            coord_y = 0;
                        }
                        if (coord_x > width-1)
                        {
                            coord_x = width - 1;
                        }
                        if (coord_y > height - 1)
                        {
                            coord_y = height - 1;
                        }
                        //if (coord_x < 0 || coord_y < 0)
                        //{
                        //    coord_x = 0;
                        //    coord_y = 0;
                        //    LOG_OUT("ERROR : scan coordinate overflow!");
                        //    error_flag -= 1;
                        //    continue;
                        //    //return -1;
                        //}
                        pts2.push_back(PointS(coord_x , coord_y));
                    }
                    ScanLineAnalysis<unsigned char> scan;
                    scan.fill(mask, width, height, pts2, 1);
                    anno_record.insert(i_anno);
                }
            }
            write_raw(name + std::string(".mask"), (char*)mask, width*height);


            //DEBUG ouput to check
            if (level !=0 && level != img->getNumberOfLevels()-1)
            {
                std::unique_ptr<unsigned char[]> img_overlay_t(new unsigned char[width*height * 3]);
                unsigned char* img_overlay = img_overlay_t.get();
                for (int i = 0 ; i< width*height ; ++i)
                {
                    if (mask[i] == 0)
                    {
                        img_overlay[i * 3] = img_data[i * 3];
                        img_overlay[i * 3+1] = img_data[i * 3+1];
                        img_overlay[i * 3+2] = img_data[i * 3+2];
                    }
                    else
                    {
                        img_overlay[i * 3] = img_data[i * 3];
                        img_overlay[i * 3+2] = img_data[i * 3+2];

                        double g = 0.5*img_data[i * 3 + 1] + 255;
                        if (g > 255.0)
                        {
                            g = 255.0;
                        }
                        img_overlay[i * 3 + 1] = (unsigned char)g;
                    }
                }
                JpegParser::write_to_jpeg(name + std::string(".jpeg"), (char*)img_overlay, width, height);
            }


            if (img_data)
            {
                delete[] img_data;
                img_data = nullptr;
            }

        }

        //check annotation
        if (anno_record.size() !=  annos.size())
        {
            std::cout << "WARNING : just process " << anno_record.size() << " , all annotation is " << annos.size() << std::endl;
        }

        if (raw_image_cur_level)
        {
            delete[] raw_image_cur_level;
            raw_image_cur_level = nullptr;
        }

    }

    if (error_flag < 0)
    {
        return -1;
    }

    return 0;
}

int process_one_anno(const std::string& anno_file, const std::string& img_file,  const std::string& output_dir , const std::string& base_name , 
    int sensitive_anno_border, int sensitive_cd_border, int filter_cd_th , double cd_border_ratio)
{
    clock_t t0 = clock();

    MultiResolutionImage* img = nullptr;
    if (0 != load_image(img_file, img))
    {
        LOG_OUT("ERROR : image file : " + img_file + std::string(" load failed!"));
        return -1;
    }

    clock_t t1 = clock();
    //std::cout << "TIME : load image cost : " << (double)(t1 - t0) << " ms\n";

    std::shared_ptr<AnnotationList> anno_list;
    if (0 != load_anno(anno_file , anno_list))
    {
        LOG_OUT("ERROR : annotation file : " + anno_file+ std::string(" load failed!"));
        return -1;
    }
    std::vector<std::shared_ptr<Annotation>> annos = anno_list->getAnnotations();

    clock_t t2 = clock();
    //std::cout << "TIME : load annotation cost : " << (double)(t2 - t1) << " ms\n";

    int pp_level = 0;
    double pp_ratio = 0;
    int pp_dim[2] = { 0,0 };
    unsigned char* pp_img_gray = nullptr;
    get_proper_image(img, pp_level, pp_dim, pp_ratio, pp_img_gray);

    //adjust sensitive border
    sensitive_anno_border = int(sensitive_anno_border / 1024.0 * (std::max)(pp_dim[0], pp_dim[1]));
    sensitive_cd_border = int(sensitive_cd_border / 1024.0 * (std::max)(pp_dim[0], pp_dim[1]));
    filter_cd_th = int(filter_cd_th / 1024.0 * (std::max)(pp_dim[0], pp_dim[1]));

    std::vector<AABB<int>> annos_aabbs;
    for (int i = 0; i<annos.size() ; ++i)
    {
        annos_aabbs.push_back(get_anno_aabb(annos[i], pp_ratio));
    }
    std::vector<AABB<int>> combine_aabbs = combine_aabb(annos_aabbs , sensitive_anno_border);

    clock_t t3 = clock();
    //std::cout << "TIME : combine annotation aabb cost : " << (double)(t3 - t2) << " ms\n";

    std::vector<AABB<int>> expand_aabbs;// = expand_aabb(combine_aabbs, pp_dim, expand_ratio);

    std::vector<AABB<int>> img_cd_aabbs;
    if (0 != get_image_connected_domain_aabb(pp_dim, pp_img_gray, combine_aabbs, sensitive_cd_border, filter_cd_th, cd_border_ratio, img_cd_aabbs))
    {
        LOG_OUT("ERROR : calculate image connected domain aabb failed!");
        return -1;
    }

    clock_t t4 = clock();
    //std::cout << "TIME : calculate image connected domain cost : " << (double)(t4 - t3) << " ms\n";

    std::vector<AABB<int>> extract_aabbs;
    if (0 != get_extract_region_aabbs(annos_aabbs, combine_aabbs, img_cd_aabbs, extract_aabbs))
    {
        LOG_OUT("ERROR : calculate extract aabb failed!");
        return -1;
    }
    clock_t t5 = clock();
    //std::cout << "TIME : calculate  extract aabbs cost : " << (double)(t5 - t4) << " ms\n";

    //////////////////////////////////////////////////////////////////////////
    //Expand 2 pixel
    for (auto it = extract_aabbs.begin(); it != extract_aabbs.end(); ++it)
    {
        AABB<int>& aabb = *it;
        for (int i = 0; i < 2; ++i)
        {
            if (aabb._min[i] > 2)
            {
                aabb._min[i] -= 2;
            }
            else
            {
                aabb._min[i] = 0;
            }

            if (aabb._max[i] < pp_dim[i]-2)
            {
                aabb._max[i] += 2;
            }
            else
            {
                aabb._max[i] = pp_dim[i] - 1;
            }
        }
    }

    if (0 != extract_mask_image(img , annos , output_dir , base_name , extract_aabbs , pp_level , pp_ratio))
    {
        LOG_OUT("ERROR : extract mask image failed!");
        return -1;
    }

    clock_t t6 = clock();
    //std::cout << "TIME : extract mask and image cost : " << (double)(t6 - t5) << " ms\n";

    //////////////////////////////////////////////////////////////////////////
    _pp_ratio = pp_ratio;
    _vis_annos = annos;
    _vis_annos_aabbs = annos_aabbs;
    _vis_c_aabbs = combine_aabbs;
    _vis_e_aabbs = expand_aabbs;
    _vis_img_cd_aabbs = img_cd_aabbs;
    _vis_extract_aabbs = extract_aabbs;
    set_vis_image_buffer(pp_dim, pp_img_gray);
    //////////////////////////////////////////////////////////////////////////

    delete[] pp_img_gray;

    return 0;
}

int anno_to_mask_image(
    const std::vector<std::string>& anno_files,
    const std::vector<std::string>& img_files,
    const std::string& output_dir,
    int sensitive_anno_border, 
    int sensitive_cd_border,
    int filter_cd_th,
    double border_cd_ratio)
{
    for (auto it = anno_files.begin(); it != anno_files.end(); ++it)
    {
        const std::string& anno_file = *it;
        const std::string anno_base =  boost::filesystem::basename(anno_file);
        std::string img_file;
        for (auto it2 = img_files.begin(); it2 != img_files.end() ; ++it2)
        {
            const std::string img_base = boost::filesystem::basename(*it2);
            if (img_base == anno_base)
            {
                img_file = *it2;
                break;
            }
        }

        if (!img_file.empty())
        {
            LOG_OUT("process : " + anno_base + "start.\n");
            if (0 != process_one_anno(anno_file, img_file , output_dir , anno_base , sensitive_anno_border , sensitive_cd_border , filter_cd_th , border_cd_ratio))
            {
                LOG_OUT("ERROR : annotation file : " + anno_file + std::string(" processing failed!"));
                continue;
            }
            LOG_OUT("process : " + anno_base + " done.\n");
        }
        else
        {
            LOG_OUT("WARNING : annotation file : " + anno_file + std::string(" match image failed!"));
            continue;
        }
    }


    return 0;
}


unsigned int _fbo = 0;
unsigned int _tex = 0;
unsigned int _depth = 0;
void display()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
    glViewport(0, 0, _img_width, _img_height);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawPixels(_img_width, _img_height, GL_RGB, GL_UNSIGNED_BYTE, _vis_img_buffer);

    glPushMatrix();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, _img_width, 0, _img_height);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    if (_vis_annos_flag)
    {
        glLineWidth(5.0);
        glColor3d(0.0, 0.2, 0.9);
        for (auto it = _vis_annos.begin(); it != _vis_annos.end(); ++it)
        {
            std::shared_ptr<Annotation> anno = *it;
            const std::vector<Point>& pts = anno->getCoordinates();
            glBegin(GL_LINE_STRIP);
            for (auto it2 = pts.begin(); it2 != pts.end(); ++it2)
            {
                double x = (*it2).getX() / _pp_ratio;
                double y = (*it2).getY() / _pp_ratio;
                glVertex2d(x, y);
            }
            glEnd();
        }
    }

    if (_vis_annos_aabbs_flag)
    {
        glLineWidth(5.0);
        glColor3d(0.8, 0.3, 0.3);
        for (auto it = _vis_annos_aabbs.begin(); it != _vis_annos_aabbs.end(); ++it)
        {
            const AABB<int> aabb = *it;
            glBegin(GL_QUADS);
            glVertex2i(aabb._min[0], aabb._min[1]);
            glVertex2i(aabb._max[0], aabb._min[1]);
            glVertex2i(aabb._max[0], aabb._max[1]);
            glVertex2i(aabb._min[0], aabb._max[1]);
            glEnd();
        }
    }

    if (_vis_c_aabbs_flag)
    {
        glLineWidth(7.0);
        glColor3d(0.2, 0.9, 0.2);
        for (auto it = _vis_c_aabbs.begin(); it != _vis_c_aabbs.end(); ++it)
        {
            const AABB<int> aabb = *it;
            glBegin(GL_QUADS);
            glVertex2i(aabb._min[0], aabb._min[1]);
            glVertex2i(aabb._max[0], aabb._min[1]);
            glVertex2i(aabb._max[0], aabb._max[1]);
            glVertex2i(aabb._min[0], aabb._max[1]);
            glEnd();
        }
    }

    if (_vis_e_aabbs_flag)
    {
        glLineWidth(8.0);
        glColor3d(0.8, 0.9, 0.2);
        for (auto it = _vis_e_aabbs.begin(); it != _vis_e_aabbs.end(); ++it)
        {
            const AABB<int> aabb = *it;
            glBegin(GL_QUADS);
            glVertex2i(aabb._min[0], aabb._min[1]);
            glVertex2i(aabb._max[0], aabb._min[1]);
            glVertex2i(aabb._max[0], aabb._max[1]);
            glVertex2i(aabb._min[0], aabb._max[1]);
            glEnd();
        }
    }

    if (_vis_img_cd_aabbs_flag)
    {
        glLineWidth(4.0);
        glColor3d(1.0, 0.5, 0.25);
        for (auto it = _vis_img_cd_aabbs.begin(); it != _vis_img_cd_aabbs.end(); ++it)
        {
            const AABB<int> aabb = *it;
            glBegin(GL_QUADS);
            glVertex2i(aabb._min[0], aabb._min[1]);
            glVertex2i(aabb._max[0], aabb._min[1]);
            glVertex2i(aabb._max[0], aabb._max[1]);
            glVertex2i(aabb._min[0], aabb._max[1]);
            glEnd();
        }
    }

    if (_vis_extract_aabbs_flag)
    {
        glLineWidth(4.0);
        glColor3d(1.0, 0.0, 0.0);
        for (auto it = _vis_extract_aabbs.begin(); it != _vis_extract_aabbs.end(); ++it)
        {
            const AABB<int> aabb = *it;
            glBegin(GL_QUADS);
            glVertex2i(aabb._min[0], aabb._min[1]);
            glVertex2i(aabb._max[0], aabb._min[1]);
            glVertex2i(aabb._max[0], aabb._max[1]);
            glVertex2i(aabb._min[0], aabb._max[1]);
            glEnd();
        }
    }

    glPopMatrix();

    glBindFramebuffer(GL_READ_FRAMEBUFFER, _fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glDrawBuffer(GL_BACK);
    glBlitFramebuffer(0, 0, _img_width, _img_height, 0, _height, _width, 0 , GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glutSwapBuffers();
}

void reshape(int width , int height)
{
    if (width == 0 || height == 0)
    {
        return;
    }

    _width = width;
    _height = height;
    glutPostRedisplay();
}

void keybord(unsigned char key, int x, int y)
{
    switch (key)
    {
        /*bool _vis_annos_flag = true;
        bool _vis_annos_aabbs_flag = true;
        bool _vis_c_aabb_flag = true;
        bool _vis_e_aabbs_flag = true;
        bool _vis_img_cd_aabbs_flag = true;*/
    case '1':
    {
        _vis_annos_flag = !_vis_annos_flag;
        std::cout << "annotation contour flag " << _vis_annos_flag << std::endl;
        break;
    }
    case '2':
    {
        _vis_annos_aabbs_flag = !_vis_annos_aabbs_flag;
        std::cout << "annotation aabb flag " << _vis_annos_flag << std::endl;
        break;
    }
    case '3':
    {
        _vis_c_aabbs_flag = !_vis_c_aabbs_flag;
        std::cout << "annotation combine aabb flag " << _vis_annos_flag << std::endl;
        break;
    }
    case '4':
    {
        _vis_e_aabbs_flag = !_vis_e_aabbs_flag;
        std::cout << "annotation expand aabb flag " << _vis_annos_flag << std::endl;
        break;
    }
    case '5':
    {
        _vis_img_cd_aabbs_flag = !_vis_img_cd_aabbs_flag;
        std::cout << "image conneted domain aabb flag " << _vis_annos_flag << std::endl;
        break;
    }
    case '6':
    {
        _vis_extract_aabbs_flag = !_vis_extract_aabbs_flag;
        std::cout << "image/mask extract aabb flag " << _vis_extract_aabbs_flag << std::endl;
        break;
    }
    default:
        break;
    }

    glutPostRedisplay();
}

void init()
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenFramebuffers(1, &_fbo);
    glGenTextures(1, &_tex);
    glGenTextures(1, &_depth);

    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    glBindTexture(GL_TEXTURE_2D, _tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _img_width, _img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _tex, 0);

    glBindTexture(GL_TEXTURE_2D, _depth);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, _img_width, _img_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depth, 0);
}


int main(int argc, char* argv[])
{
    LogSheild log_sheild("anno2mask.log", "Extracting mask/image from annotation files>>> \n");

    /*std::string image_direction;
    std::string annotation_direction;
    std::string output_direction;
    bool encryption = true;*/

    std::string image_dir = "";
    std::string annotation_dir = image_dir;
    std::string output_dir = image_dir;
    int sensitive_anno_border = 50;
    int sensitive_cd_border = 20;
    int filter_cd_th = 50;
    double cd_border_ratio = 0.3;
    bool vis = false;

    if (argc == 1)
    {
        LOG_OUT("invalid arguments!\n");
        LOG_OUT("targuments list:\n");
        LOG_OUT("\t-data <path> : image data root(.dcm)\n");
        LOG_OUT("\t-annotation <path> : annotation root(.xml)\n");
        LOG_OUT("\t-output <path] : save mask root\n");
        LOG_OUT("\t-sens_anno : the distance between two annotation's aabb combine to one. default is 50 pixel in around 1024 image level \n");
        LOG_OUT("\t-sens_cd : the distance between two image connected domain aabb combine to one. default is 20 pixel in around 1024 image level \n");
        LOG_OUT("\t-filter_cd : the threshold of connected domain which be discard . default is 50 pixel in around 1024 image level \n");
        LOG_OUT("\t-border_cd : the border ratio of connected domain's aabb(less than this value will be consider as border and discard). default is 0.3\n");
        LOG_OUT("\t-vis: if vis debug\n");
        return -1;
    }
    else
    {
        for (int i = 1; i < argc; ++i)
        {
            if (std::string(argv[i]) == "-help")
            {
                LOG_OUT("targuments list:\n");
                LOG_OUT("\t-data <path> : image data root(.dcm)\n");
                LOG_OUT("\t-annotation <path> : annotation root(.xml)\n");
                LOG_OUT("\t-output <path] : save mask root\n");
                LOG_OUT("\t-sens_anno : the distance between two annotation's aabb combine to one. default is 50 pixel in around 1024 image level \n");
                LOG_OUT("\t-sens_cd : the distance between two image connected domain aabb combine to one. default is 20 pixel in around 1024 image level \n");
                LOG_OUT("\t-filter_cd : the threshold of connected domain which be discard . default is 50 pixel in around 1024 image level \n");
                LOG_OUT("\t-border_cd : the border ratio of connected domain's aabb(less than this value will be consider as border and discard). default is 0.3\n");
                LOG_OUT("\t-vis: if vis debug\n");
                return 0;
            }
            if (std::string(argv[i]) == "-data")
            {
                if (i + 1 > argc - 1)
                {
                    LOG_OUT("invalid arguments!\n");
                    return -1;
                }
                image_dir = std::string(argv[i + 1]);

                ++i;
            }
            else if (std::string(argv[i]) == "-annotation")
            {
                if (i + 1 > argc - 1)
                {
                    LOG_OUT("invalid arguments!\n");
                    return -1;
                }

                annotation_dir = std::string(argv[i + 1]);
                ++i;
            }
            else if (std::string(argv[i]) == "-output")
            {
                if (i + 1 > argc - 1)
                {
                    LOG_OUT("invalid arguments!\n");
                    return -1;
                }

                output_dir = std::string(argv[i + 1]);
                ++i;
            }
            else if (std::string(argv[i]) == "-sens_anno")
            {
                if (i + 1 > argc - 1)
                {
                    LOG_OUT("invalid arguments!\n");
                    return -1;
                }
                std::string arg = std::string(argv[i + 1]);
                std::stringstream ss(arg);
                ss >> sensitive_anno_border;
                ++i;
            }
            else if (std::string(argv[i]) == "-sens_cd")
            {
                if (i + 1 > argc - 1)
                {
                    LOG_OUT("invalid arguments!\n");
                    return -1;
                }
                std::string arg = std::string(argv[i + 1]);
                std::stringstream ss(arg);
                ss >> sensitive_cd_border;
                ++i;
            }
            else if (std::string(argv[i]) == "-filter_cd")
            {
                if (i + 1 > argc - 1)
                {
                    LOG_OUT("invalid arguments!\n");
                    return -1;
                }
                std::string arg = std::string(argv[i + 1]);
                std::stringstream ss(arg);
                ss >> filter_cd_th;
                ++i;
            }
            else if (std::string(argv[i]) == "-border_cd")
            {
                if (i + 1 > argc - 1)
                {
                    LOG_OUT("invalid arguments!\n");
                    return -1;
                }
                std::string arg = std::string(argv[i + 1]);
                std::stringstream ss(arg);
                ss >> cd_border_ratio;
                ++i;
            }
            else if (std::string(argv[i]) == "-vis")
            {
                vis = true;
            }
        }
    }

    std::stringstream ss;
    ss << "annotation to mask parameter :" << std::endl;
    ss << "\timage direction : " << image_dir << std::endl;
    ss << "\tannotation direction : " << annotation_dir << std::endl;
    ss << "\toutput direction : " << output_dir << std::endl;
    ss << "\tannotation sensitity : " << sensitive_anno_border << std::endl;
    ss << "\timage connected domain sensitity : " << sensitive_cd_border << std::endl;
    ss << "\timage connected filter threshold : " << filter_cd_th << std::endl;
    ss << "\timage connexted border ratio : " << cd_border_ratio << std::endl;
    ss << "extracting >>>>>>\n";
    LOG_OUT(ss.str());

    if (image_dir.empty() || annotation_dir.empty() || output_dir.empty())
    {
        LOG_OUT("invalid empty direction!\n");
        return -1;
    }

    std::set<std::string> anno_post_fix;
    anno_post_fix.insert(".araw");
    std::vector<std::string> anno_files;
    get_all_files(annotation_dir, anno_files, anno_post_fix);
    if(anno_files.empty())
    {
        LOG_OUT("ERROR : annotation file is empty !");
        return -1;
    }

    std::set<std::string> img_post_fix;
    img_post_fix.insert(".tif");
    img_post_fix.insert(".svs");
    img_post_fix.insert(".vms");
    img_post_fix.insert(".vmu");
    img_post_fix.insert(".ndpi");
    img_post_fix.insert(".scn");
    img_post_fix.insert(".mrxs");
    img_post_fix.insert(".tiff");
    img_post_fix.insert(".svslide");
    img_post_fix.insert(".bif");
    std::vector<std::string> img_files;
    get_all_files(image_dir, img_files, img_post_fix);
    if (img_files.empty())
    {
        LOG_OUT("ERROR : image file is empty!");
        return -1;
    }

    if (0 != anno_to_mask_image(anno_files ,img_files, output_dir , sensitive_anno_border , sensitive_cd_border , filter_cd_th , cd_border_ratio))
    {
        LOG_OUT("ERROR : annotation to mask image faild!");
        return -1;
    }

    //////////////////////////////////////////////////////////////////////////
    //VIS part
    //////////////////////////////////////////////////////////////////////////

    if (!vis)
    {
        return 0;
    }

    if (_img_width > 1080 || _img_height > 1080)
    {
        _width = 1080;
        _height = (int)((double)_width / (double)_img_width * (double)_img_height);
    }
    else
    {
        _width = _img_width;
        _height = _img_height;
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(_width, _height);

    glutCreateWindow("Annotation-Vis");

    if (GLEW_OK != glewInit())
    {
        std::cout << "Init glew failed!\n";
        return -1;
    }

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keybord);

    glutMainLoop();

    return 0;
}
