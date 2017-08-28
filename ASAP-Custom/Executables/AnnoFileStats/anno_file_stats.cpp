#include <iostream>
#include <string>
#include <vector>
#include "boost/filesystem.hpp"
#include <QString>


#include "Annotation/AnnotationList.h"
#include "Annotation/Annotation.h"
#include "Annotation/AnnotationGroup.h"
#include "Annotation/BinaryRepository.h"

class CInvalidAnnotation
{
public:
    std::map<std::string, std::vector<std::string>> _annos;
public:
    CInvalidAnnotation()
    {}
    void add(const std::string& anno_name, const std::string& group_name)
    {
        if (_annos.find(group_name) == _annos.end())
        {
            _annos.insert(std::make_pair(group_name, std::vector<std::string>()));
        }
        _annos[group_name].push_back(anno_name);
    }
protected:
private:
};

//struct InvalidAnnotation
//{
//    std::string _anno_name;
//    std::string _group_name;
//    InvalidAnnotation():_group_name("NoGroup&&")
//    {}
//    InvalidAnnotation(const std::string& anno_name, const std::string& group_name) :_anno_name(anno_name) , _group_name(group_name)
//    {}
//    InvalidAnnotation(const std::string& anno_name):_anno_name(anno_name),_group_name("NoGroup&&")
//    {}
//};
static std::ofstream out_log;
static std::map<std::string, int > statistic_result;//可选列表的统计
static std::map < std::string, std::string > dic;//列表字典
static std::map<std::string, int>  statistic_no_anno;//没有勾选可选列表的项统计
static std::map<std::string, CInvalidAnnotation> statistic_invalid_file;//没有勾选可选列表的项的相信统计

class LogSheild
{
public:
    LogSheild()
    {
        out_log.open("anno_file_stats.log", std::ios::out);
        if (out_log.is_open())
        {
            out_log << "Annotation files statistic log:\n";
        }
    }
    ~LogSheild()
    {
        out_log.close();
    }
protected:
private:
};

void get_all_files(const std::string& root, unsigned int& num)
{
    if (root.empty())
    {
        return;
    }
    else
    {
        std::vector<std::string> dirs;
        for (boost::filesystem::directory_iterator it(root); it != boost::filesystem::directory_iterator(); ++it)
        {
            if (boost::filesystem::is_directory(*it))
            {
                dirs.push_back(it->path().filename().string());
            }
            else
            {
                const std::string ext = boost::filesystem::extension(*it);
                if (ext == ".araw")
                {
                    ++num;
                }
            }
        }

        for (unsigned int i = 0; i < dirs.size(); ++i)
        {
            const std::string next_dir(root + "/" + dirs[i]);

            get_all_files(next_dir, num);
        }
    }
}

void statistic_araw(const std::string& src_root)
{
    if (src_root.empty())
    {
        return;
    }

    std::vector<std::string> dirs;
    std::vector<std::string> files;
    for (boost::filesystem::directory_iterator it(src_root); it != boost::filesystem::directory_iterator(); ++it)
    {
        if (boost::filesystem::is_directory(*it))
        {
            dirs.push_back(it->path().filename().string());
        }
        else
        {
            const std::string ext = boost::filesystem::extension(*it);
            if (ext == ".araw")
            {
                files.push_back(it->path().filename().string());
            }
        }
    }

    for (unsigned int i = 0; i < dirs.size(); ++i)
    {
        const std::string cur_dir(src_root + "/" + dirs[i]);

        statistic_araw(cur_dir);
    }

    for (unsigned int i = 0; i < files.size(); ++i)
    {
        const std::string cur_file(src_root + "/" + files[i]);

        std::shared_ptr<AnnotationList> anno_list(new AnnotationList());
        std::shared_ptr<Repository> rep(new BinaryRepository(anno_list));
        rep->setSource(cur_file);
        if (rep->load())
        {
            std::vector<std::shared_ptr<Annotation>> annos = anno_list->getAnnotations();
            for (auto it = annos.begin(); it != annos.end(); ++it)
            {
                const std::set<std::string>tumor_types = (*it)->getTumorTypes();
                if (tumor_types.empty())
                {
                    const std::string anno_name = std::string(QString((*it)->getName().c_str()).toLocal8Bit());
                    std::shared_ptr<AnnotationGroup> group = (*it)->getGroup();
                    std::string group_name = group == nullptr ? "NoGroup$$" : std::string(QString(group->getName().c_str()).toLocal8Bit());


                    //Invalid annotation number + 1
                    statistic_result["NoAnno"] += 1;


                    //Invalid annotation infos in certain file
                    if (statistic_invalid_file.find(cur_file) == statistic_invalid_file.end())
                    {
                        statistic_invalid_file.insert(std::make_pair(cur_file, CInvalidAnnotation()));
                    }
                    statistic_invalid_file[cur_file].add(anno_name, group_name);


                    //Invalid annotations in all files
                    auto it_no_anno = statistic_no_anno.find(anno_name);
                    if (it_no_anno != statistic_no_anno.end())
                    {
                        it_no_anno->second += 1;
                    }
                    else
                    {
                        statistic_no_anno.insert(std::make_pair(anno_name, 1));
                    }
                }
                else
                {
                    for (auto it2 = tumor_types.begin(); it2 != tumor_types.end(); ++it2)
                    {
                        auto it_tu = statistic_result.find(*it2);
                        if (it_tu != statistic_result.end())
                        {
                            it_tu->second += 1;
                        }
                        else
                        {
                            statistic_result.insert(std::make_pair(*it2, 1));
                        }
                    }
                }

            }
        }
    }
}

void init_dic()
{
    //const static std::string TUMOR_TYPES[TUMOR_TYPE_NUM] =
    //{
    //    "LGIEN", //低级别上皮内瘤变 腺瘤
    //    "HGIEN", //高级别上皮内瘤变 腺瘤
    //    "PRE_LGIEN",//低级别上皮内瘤变 上皮内瘤变
    //    "PRE_HGIEN",//高级别上皮内瘤变 上皮内瘤变
    //    "papillary_adenoma", //乳头状腺癌
    //    "tubular_adenoma", //管状腺癌
    //    "mucinous_adenoma", //粘液腺癌
    //    "low_adhesion_adenoma", //低粘附性癌
    //    "mixed_adenoma", //混合性腺癌
    //    "adenosquamous_carcinoma", //腺鳞癌
    //    "medullary_carcinoma", //伴有淋巴样间质的癌（髓样癌）
    //    "hepatoid_adenocarcinoma", //肝样腺癌
    //    "squamous-cell_carcinoma", //鳞状细胞癌
    //    "undifferentiated_carcinoma", //未分化癌
    //    "NET1", //NET1级（类癌）
    //    "NET2", //NET2级
    //    "NEC_big_cell", //大细胞NEC
    //    "NEC_small_cell", //小细胞NEC
    //    "MIX_NET" ,//混合性腺神经内分泌癌
    //    "NO_GEIEN",//无上皮内瘤变
    //    "Uncertain_GEIEN",//不确定的上皮内瘤变
    //    "Uncertain",//不确定的病变
    //};

    dic["LGIEN"] = std::string("低级别上皮内瘤变(腺瘤)");
    dic["HGIEN"] = std::string("高级别上皮内瘤变(腺瘤)");
    dic["PRE_LGIEN"] = std::string("低级别上皮内瘤变(上皮内瘤变)");
    dic["PRE_HGIEN"] = std::string("高级别上皮内瘤变(上皮内瘤变)");
    dic["papillary_adenoma"] = std::string("乳头状腺癌");
    dic["tubular_adenoma"] = std::string("管状腺癌");
    dic["mucinous_adenoma"] = std::string("粘液腺癌");
    dic["low_adhesion_adenoma"] = std::string("低粘附性癌");
    dic["mixed_adenoma"] = std::string("混合性腺癌");
    dic["adenosquamous_carcinoma"] = std::string("腺鳞癌");
    dic["medullary_carcinoma"] = std::string("伴有淋巴样间质的癌(髓样癌)");
    dic["hepatoid_adenocarcinoma"] = std::string("肝样腺癌");
    dic["squamous-cell_carcinom"] = std::string("鳞状细胞癌");
    dic["undifferentiated_carcinoma"] = std::string("未分化癌");
    dic["NET1"] = std::string("NET1级（类癌）");
    dic["NET2"] = std::string("NET2级");
    dic["NEC_big_cell"] = std::string("大细胞NEC");
    dic["NEC_small_cell"] = std::string("小细胞NEC");
    dic["MIX_NET"] = std::string("混合性腺神经内分泌癌");
    dic["NO_GEIEN"] = std::string("无上皮内瘤变");
    dic["Uncertain_GEIEN"] = std::string("不确定的上皮内瘤变");
    dic["Uncertain"] = std::string("不确定的病变");
    dic["NoAnno"] = std::string("！没有标注！");
    dic["SuspectedObject"] = std::string("疑似病变");
}


int main(int argc, char* argv[])
{
    init_dic();

    LogSheild log_sheild;
    if (argc != 2)
    {
        out_log << "Invalid input!";
        return -1;
    }

    std::string root(argv[1]);
    if (!boost::filesystem::is_directory(root))
    {
        out_log << "invalid direction.\n";
        return -1;
    }

    unsigned int araw_file_num = 0;
    get_all_files(root, araw_file_num);
    out_log << "Annotation file(.araw) number : " << araw_file_num << std::endl;

    statistic_result["NoAnno"] = 0;

    statistic_araw(root);


    out_log << "Statistic result : \n";
    for (auto it = statistic_result.begin(); it != statistic_result.end(); ++it)
    {
        auto it2 = dic.find(it->first);
        if (it2 == dic.end())
        {
            out_log << "Invalid tumor type!";
            return-1;
        }
        out_log << it2->second << " " << it->second << std::endl;
    }
    out_log << std::endl;

    out_log << "None choose tumor type list : \n";
    for (auto it = statistic_no_anno.begin(); it != statistic_no_anno.end(); ++it)
    {
        out_log << it->first << " " << it->second << std::endl;
    }
    out_log << std::endl;

    out_log << "Has none choose tumor type file list: \n";
    for (auto it = statistic_invalid_file.begin(); it != statistic_invalid_file.end(); ++it)
    {
        out_log << it->first << std::endl;
    }
    out_log << std::endl;

    out_log << "Has none choose tumor type file specifications: \n";
    for (auto it = statistic_invalid_file.begin(); it != statistic_invalid_file.end(); ++it)
    {
        out_log << "File name :" << it->first << std::endl;
        std::map<std::string, std::vector<std::string>>invalid_anno = it->second._annos;
        for (auto it2 = invalid_anno.begin(); it2 != invalid_anno.end(); ++it2)
        {
            out_log << "\t" << "Group name : " << it2->first << std::endl;
            for (auto it3 = it2->second.begin(); it3 != it2->second.end(); ++it3)
            {
                out_log << "\t\t" << "Annotation name : " << *it3 << std::endl;
            }
        }
    }
    out_log << std::endl;

    out_log << "Log end.\n";

    return 0;
}