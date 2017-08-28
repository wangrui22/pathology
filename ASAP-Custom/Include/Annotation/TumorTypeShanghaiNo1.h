#ifndef TUMOR_TYPE_SHANGHAI_NO1_H
#define TUMOR_TYPE_SHANGHAI_NO1_H

#include <string>

#define TUMOR_TYPE_NUM 23

//////////////////////////////////////////////////////////////////////////
//这个list一定要和UI的带checkbox的项顺序一致，要保持字符串值和Annotation_define.h中的值相同，顺序可以不同
//////////////////////////////////////////////////////////////////////////
const static std::string TUMOR_TYPES_TREE[TUMOR_TYPE_NUM] =//For listing
{
    "SuspectedObject",//疑似病变
    "Uncertain",//不确定的病变
    "LGIEN", //低级别上皮内瘤变 腺瘤
    "HGIEN", //高级别上皮内瘤变 腺瘤
    "NO_GEIEN",//无上皮内瘤变
    "Uncertain_GEIEN",//不确定的上皮内瘤变
    "PRE_LGIEN",//低级别上皮内瘤变 上皮内瘤变
    "PRE_HGIEN",//高级别上皮内瘤变 上皮内瘤变
    "papillary_adenoma", //乳头状腺癌
    "tubular_adenoma", //管状腺癌
    "mucinous_adenoma", //粘液腺癌
    "low_adhesion_adenoma", //低粘附性癌
    "mixed_adenoma", //混合性腺癌
    "adenosquamous_carcinoma", //腺鳞癌
    "medullary_carcinoma", //伴有淋巴样间质的癌（髓样癌）
    "hepatoid_adenocarcinoma", //肝样腺癌
    "squamous-cell_carcinoma", //鳞状细胞癌
    "undifferentiated_carcinoma", //未分化癌
    "NET1", //NET1级（类癌）
    "NET2", //NET2级
    "NEC_big_cell", //大细胞NEC
    "NEC_small_cell", //小细胞NEC
    "MIX_NET" //混合性腺神经内分泌癌
};


//////////////////////////////////////////////////////////////////////////
//这个list一定只能在底层扩展好保证对之前的标注结果兼容！！！
//////////////////////////////////////////////////////////////////////////
const std::string TUMOR_TYPES[TUMOR_TYPE_NUM] =
{
    "LGIEN", //低级别上皮内瘤变 腺瘤
    "HGIEN", //高级别上皮内瘤变 腺瘤
    "PRE_LGIEN",//低级别上皮内瘤变 上皮内瘤变
    "PRE_HGIEN",//高级别上皮内瘤变 上皮内瘤变
    "papillary_adenoma", //乳头状腺癌
    "tubular_adenoma", //管状腺癌
    "mucinous_adenoma", //粘液腺癌
    "low_adhesion_adenoma", //低粘附性癌
    "mixed_adenoma", //混合性腺癌
    "adenosquamous_carcinoma", //腺鳞癌
    "medullary_carcinoma", //伴有淋巴样间质的癌（髓样癌）
    "hepatoid_adenocarcinoma", //肝样腺癌
    "squamous-cell_carcinoma", //鳞状细胞癌
    "undifferentiated_carcinoma", //未分化癌
    "NET1", //NET1级（类癌）
    "NET2", //NET2级
    "NEC_big_cell", //大细胞NEC
    "NEC_small_cell", //小细胞NEC
    "MIX_NET" ,//混合性腺神经内分泌癌
    "NO_GEIEN",//无上皮内瘤变
    "Uncertain_GEIEN",//不确定的上皮内瘤变
    "Uncertain",//不确定的病变
    "SuspectedObject",//疑似病变
};

#endif // !TUMOR_TYPE_SHANGHAI_NO1_H
