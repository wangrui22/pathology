#ifndef TUMOR_TYPE_RUIJIN_H
#define TUMOR_TYPE_RUIJIN_H

/***********************************************************/
//Version 1.0 Hengdao& Ruijin
//Version 1.1 增加 Normal_gland(正常腺体)  AD_classic(经典型)（癌）
/***********************************************************/
#include <string>

#define TUMOR_TYPE_NUM 26

//////////////////////////////////////////////////////////////////////////
//这个list一定要和UI的带checkbox的项顺序一致，要保持字符串值和Annotation_define.h中的值相同，顺序可以不同
//////////////////////////////////////////////////////////////////////////
const static std::string TUMOR_TYPES_TREE[TUMOR_TYPE_NUM] =//For listing
{
    //1
    "Uncertain",//不确定的病变
    //2 正常腺体
    "Normal_gland",
    //3.	腺瘤
    "LGIEN", //低级别上皮内瘤变 腺瘤
    "HGIEN", //高级别上皮内瘤变 腺瘤
    //4
    "NO_GEIEN",//无上皮内瘤变
    //5
    "Uncertain_GEIEN",//不确定的上皮内瘤变
    //6 上皮内瘤变
    "PRE_LGIEN",//低级别上皮内瘤变 上皮内瘤变
    "PRE_HGIEN",//高级别上皮内瘤变 上皮内瘤变
    //7 癌症
    //7.1 腺癌
    "AD_diablastic_comedo", //筛状-粉刺样
    "AD_medullary", //髓样癌
    "AD_micropapillary", //微乳头状腺癌
    "AD_slime", //黏液腺癌
    "AD_zigzag", //锯齿状腺癌
    "AD_signet_ring_cell",//印戒细胞癌
    "AD_classic",//经典型癌
    //7.2
    "adenosquamous_carcinoma", //腺鳞癌
    //7.3
    "Spindle-cell_carcinoma", //梭形细胞癌
    //7.4
    "squamous-cell_carcinoma", //鳞状细胞癌
    //7.5
    "undifferentiated_carcinoma", //未分化癌
    //8 神经内分泌肿瘤
    //8.1 神经内分泌瘤 NET
    "NET1", //NET1级（类癌）
    "NET2", //NET2级
    //8.2神经内分泌瘤 NEC
    "NEC_big_cell", //大细胞NEC
    "NEC_small_cell", //小细胞NEC
    //8.3
    "MIX_NET", //混合性腺神经内分泌癌
    //8.4
    "EC-cell-serotonin-NET",//EC细胞，分泌5-羟色胺NET
    //8.5
    "L-cell-secretion-glucagon-PP_PYY-NET"//L细胞，分泌胰高血糖素样肽和PP/PYY NET
};


//////////////////////////////////////////////////////////////////////////
//这个list一定只能在底层扩展好保证对之前的标注结果兼容！！！
//////////////////////////////////////////////////////////////////////////
const std::string TUMOR_TYPES[TUMOR_TYPE_NUM] =
{
    //1
    "Uncertain",//不确定的病变
    //2.	腺瘤
    "LGIEN", //低级别上皮内瘤变 腺瘤
    "HGIEN", //高级别上皮内瘤变 腺瘤
    //3
    "NO_GEIEN",//无上皮内瘤变
    //4
    "Uncertain_GEIEN",//不确定的上皮内瘤变
    //5 上皮内瘤变
    "PRE_LGIEN",//低级别上皮内瘤变 上皮内瘤变
    "PRE_HGIEN",//高级别上皮内瘤变 上皮内瘤变
    //6 癌症
    //6.1 腺癌
    "AD_diablastic_comedo", //筛状-粉刺样
    "AD_medullary", //髓样癌
    "AD_micropapillary", //微乳头状腺癌
    "AD_slime", //黏液腺癌
    "AD_zigzag", //锯齿状腺癌
    "AD_signet_ring_cell",//印戒细胞癌
    //6.2
    "adenosquamous_carcinoma", //腺鳞癌
    //6.3
    "Spindle-cell_carcinoma", //梭形细胞癌
    //6.4
    "squamous-cell_carcinoma", //鳞状细胞癌
    //6.5
    "undifferentiated_carcinoma", //未分化癌
    //7 神经内分泌肿瘤
    //7.1 神经内分泌瘤 NET
    "NET1", //NET1级（类癌）
    "NET2", //NET2级
    //7.2神经内分泌瘤 NEC
    "NEC_big_cell", //大细胞NEC
    "NEC_small_cell", //小细胞NEC
    //7.3
    "MIX_NET", //混合性腺神经内分泌癌
    //7.4
    "EC-cell-serotonin-NET",//EC细胞，分泌5-羟色胺NET
    //7.5
    "L-cell-secretion-glucagon-PP_PYY-NET",//L细胞，分泌胰高血糖素样肽和PP/PYY NET
    //////////////////////////////////////////////////////////////////////////
    //version 1.1 adding
    "Normal_gland",//2 正常腺体
    "AD_classic"//经典型癌
    //////////////////////////////////////////////////////////////////////////
};

#endif // !TUMOR_TYPE_SHANGHAI_NO1_H
