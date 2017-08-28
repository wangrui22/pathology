#ifndef SEGMNET_ANALYSIS_H
#define SEGMNET_ANALYSIS_H

#include <memory>
#include <limits>
#include <iostream>
#include <stack>


class SegmentAnalysis
{
public:
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
    unsigned char get_threshold_otus_low(unsigned char* img_gray, int width , int height , AABB<int> aabb)
    {
        //get threshold
        const int gray_level = 256;
        unsigned int gray_hist[gray_level];
        memset(gray_hist, 0, sizeof(gray_hist));
        float sum = 0;

        int pixel_num = 0;
        for (int y = aabb._min[1] ; y < aabb._max[1] ; ++y)
        {
            for (int x = aabb._min[0] ; x < aabb._max[0] ; ++x)
            {
                unsigned char tmp = 255 - img_gray[y*width + x];
                gray_hist[tmp] += 1;
                sum += tmp * 0.001f;
                pixel_num += 1;
            }
        }

        //calculate mean
        const float pixel_num_f = (float)pixel_num;
        const float mean = sum / (float)pixel_num * 1000.0f;
        float max_icv = std::numeric_limits<float>::min();
        int max_gray_scalar = -1;
        for (int i = 1; i < gray_level - 1; ++i)
        {
            //PA MA
            unsigned int pixel_a = 0;
            float sum_a = 0.0f;
            for (int j = 0; j <= i; ++j)
            {
                pixel_a += gray_hist[j];
                sum_a += (float)j*gray_hist[j] * 0.001f;
            }
            float pa = (float)pixel_a / pixel_num_f;
            float mean_a = sum_a / (float)pixel_a * 1000.0f;

            //PB MB
            float pixel_b_f = (float)(pixel_num - pixel_a);
            float pb = pixel_b_f / pixel_num_f;
            float mean_b = (sum - sum_a) / pixel_b_f * 1000.0f;

            //ICV
            float icv = pa * (mean_a - mean)*(mean_a - mean) +
                pb * (mean_b - mean)*(mean_b - mean);

            if (icv > max_icv)
            {
                max_icv = icv;
                max_gray_scalar = i;
            }
        }   
        return (unsigned char)max_gray_scalar;
    }

    void segment_low(unsigned char* img_gray, unsigned char* mask, int width, int height , unsigned char th)
    {
        const int length = width*height;
        memset(mask, 0, sizeof(unsigned char)*length);
        for (int i = 0; i < length; ++i)
        {
            if (255 - img_gray[i] > th)
            {
                mask[i] = 1;
            }
        }
    }

    void remove_border_9_n(unsigned char* img_gray , int width, int height , unsigned char* mask , const int x, const int y, std::stack<POS>& s , unsigned char similar , unsigned char th)
    {
        const int yyy = y + 2 >height ? height : y + 2;
        const int xxx = x + 2 >width ? width : x + 2;

        for (int yy = y - 1 < 0 ? 0 : y - 1; yy < yyy; ++yy)
        {
            for (int xx = x - 1 < 0 ? 0 : x - 1; xx < xxx; ++xx)
            {
                if (xx != x || yy != y)
                {
                    int idx = yy*width + xx;
                    if (mask[idx] != 0 && 255 - img_gray[idx] > th && abs(img_gray[idx] - img_gray[y*width + x]) < similar)
                    {
                        mask[idx] = 0;
                        s.push(POS(xx, yy));
                    }
                }
            }
        }
    }

    void remove_border(unsigned char* img_gray, int width, int height , unsigned char* mask, unsigned char similar , unsigned char th)
    {
        //corner seeds
        std::stack<POS> seeds;
        seeds.push(POS(0, 0));
        seeds.push(POS(width-1, 0));
        seeds.push(POS(0, height-1));
        seeds.push(POS(width-1, height-1));
        /*for (int y = height/4 ; y<height ; y +=height/4)
        {
            for (int x = width/ 4; x < width; x+= width/ 4)
            {
                seeds.push(POS(x, y));
            }
        }*/

        while (!seeds.empty())
        {
            POS ps = seeds.top();
            seeds.pop();
            remove_border_9_n(img_gray, width, height, mask, ps.x, ps.y, seeds , similar , th);
        }
    }

protected:
private:
};

#endif