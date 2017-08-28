#ifndef TUMOR_TYPE_RUIJIN_H
#define TUMOR_TYPE_RUIJIN_H

/***********************************************************/
//Version 1.0 Hengdao& Ruijin
//Version 1.1 ���� Normal_gland(��������)  AD_classic(������)������
/***********************************************************/
#include <string>

#define TUMOR_TYPE_NUM 26

//////////////////////////////////////////////////////////////////////////
//���listһ��Ҫ��UI�Ĵ�checkbox����˳��һ�£�Ҫ�����ַ���ֵ��Annotation_define.h�е�ֵ��ͬ��˳����Բ�ͬ
//////////////////////////////////////////////////////////////////////////
const static std::string TUMOR_TYPES_TREE[TUMOR_TYPE_NUM] =//For listing
{
    //1
    "Uncertain",//��ȷ���Ĳ���
    //2 ��������
    "Normal_gland",
    //3.	����
    "LGIEN", //�ͼ�����Ƥ������ ����
    "HGIEN", //�߼�����Ƥ������ ����
    //4
    "NO_GEIEN",//����Ƥ������
    //5
    "Uncertain_GEIEN",//��ȷ������Ƥ������
    //6 ��Ƥ������
    "PRE_LGIEN",//�ͼ�����Ƥ������ ��Ƥ������
    "PRE_HGIEN",//�߼�����Ƥ������ ��Ƥ������
    //7 ��֢
    //7.1 �ٰ�
    "AD_diablastic_comedo", //ɸ״-�۴���
    "AD_medullary", //������
    "AD_micropapillary", //΢��ͷ״�ٰ�
    "AD_slime", //�Һ�ٰ�
    "AD_zigzag", //���״�ٰ�
    "AD_signet_ring_cell",//ӡ��ϸ����
    "AD_classic",//�����Ͱ�
    //7.2
    "adenosquamous_carcinoma", //���۰�
    //7.3
    "Spindle-cell_carcinoma", //����ϸ����
    //7.4
    "squamous-cell_carcinoma", //��״ϸ����
    //7.5
    "undifferentiated_carcinoma", //δ�ֻ���
    //8 ���ڷ�������
    //8.1 ���ڷ����� NET
    "NET1", //NET1�����఩��
    "NET2", //NET2��
    //8.2���ڷ����� NEC
    "NEC_big_cell", //��ϸ��NEC
    "NEC_small_cell", //Сϸ��NEC
    //8.3
    "MIX_NET", //����������ڷ��ڰ�
    //8.4
    "EC-cell-serotonin-NET",//ECϸ��������5-��ɫ��NET
    //8.5
    "L-cell-secretion-glucagon-PP_PYY-NET"//Lϸ���������ȸ�Ѫ�������ĺ�PP/PYY NET
};


//////////////////////////////////////////////////////////////////////////
//���listһ��ֻ���ڵײ���չ�ñ�֤��֮ǰ�ı�ע������ݣ�����
//////////////////////////////////////////////////////////////////////////
const std::string TUMOR_TYPES[TUMOR_TYPE_NUM] =
{
    //1
    "Uncertain",//��ȷ���Ĳ���
    //2.	����
    "LGIEN", //�ͼ�����Ƥ������ ����
    "HGIEN", //�߼�����Ƥ������ ����
    //3
    "NO_GEIEN",//����Ƥ������
    //4
    "Uncertain_GEIEN",//��ȷ������Ƥ������
    //5 ��Ƥ������
    "PRE_LGIEN",//�ͼ�����Ƥ������ ��Ƥ������
    "PRE_HGIEN",//�߼�����Ƥ������ ��Ƥ������
    //6 ��֢
    //6.1 �ٰ�
    "AD_diablastic_comedo", //ɸ״-�۴���
    "AD_medullary", //������
    "AD_micropapillary", //΢��ͷ״�ٰ�
    "AD_slime", //�Һ�ٰ�
    "AD_zigzag", //���״�ٰ�
    "AD_signet_ring_cell",//ӡ��ϸ����
    //6.2
    "adenosquamous_carcinoma", //���۰�
    //6.3
    "Spindle-cell_carcinoma", //����ϸ����
    //6.4
    "squamous-cell_carcinoma", //��״ϸ����
    //6.5
    "undifferentiated_carcinoma", //δ�ֻ���
    //7 ���ڷ�������
    //7.1 ���ڷ����� NET
    "NET1", //NET1�����఩��
    "NET2", //NET2��
    //7.2���ڷ����� NEC
    "NEC_big_cell", //��ϸ��NEC
    "NEC_small_cell", //Сϸ��NEC
    //7.3
    "MIX_NET", //����������ڷ��ڰ�
    //7.4
    "EC-cell-serotonin-NET",//ECϸ��������5-��ɫ��NET
    //7.5
    "L-cell-secretion-glucagon-PP_PYY-NET",//Lϸ���������ȸ�Ѫ�������ĺ�PP/PYY NET
    //////////////////////////////////////////////////////////////////////////
    //version 1.1 adding
    "Normal_gland",//2 ��������
    "AD_classic"//�����Ͱ�
    //////////////////////////////////////////////////////////////////////////
};

#endif // !TUMOR_TYPE_SHANGHAI_NO1_H
