#ifndef TUMOR_TYPE_SHANGHAI_NO1_H
#define TUMOR_TYPE_SHANGHAI_NO1_H

#include <string>

#define TUMOR_TYPE_NUM 23

//////////////////////////////////////////////////////////////////////////
//���listһ��Ҫ��UI�Ĵ�checkbox����˳��һ�£�Ҫ�����ַ���ֵ��Annotation_define.h�е�ֵ��ͬ��˳����Բ�ͬ
//////////////////////////////////////////////////////////////////////////
const static std::string TUMOR_TYPES_TREE[TUMOR_TYPE_NUM] =//For listing
{
    "SuspectedObject",//���Ʋ���
    "Uncertain",//��ȷ���Ĳ���
    "LGIEN", //�ͼ�����Ƥ������ ����
    "HGIEN", //�߼�����Ƥ������ ����
    "NO_GEIEN",//����Ƥ������
    "Uncertain_GEIEN",//��ȷ������Ƥ������
    "PRE_LGIEN",//�ͼ�����Ƥ������ ��Ƥ������
    "PRE_HGIEN",//�߼�����Ƥ������ ��Ƥ������
    "papillary_adenoma", //��ͷ״�ٰ�
    "tubular_adenoma", //��״�ٰ�
    "mucinous_adenoma", //ճҺ�ٰ�
    "low_adhesion_adenoma", //��ճ���԰�
    "mixed_adenoma", //������ٰ�
    "adenosquamous_carcinoma", //���۰�
    "medullary_carcinoma", //�����ܰ������ʵİ�����������
    "hepatoid_adenocarcinoma", //�����ٰ�
    "squamous-cell_carcinoma", //��״ϸ����
    "undifferentiated_carcinoma", //δ�ֻ���
    "NET1", //NET1�����఩��
    "NET2", //NET2��
    "NEC_big_cell", //��ϸ��NEC
    "NEC_small_cell", //Сϸ��NEC
    "MIX_NET" //����������ڷ��ڰ�
};


//////////////////////////////////////////////////////////////////////////
//���listһ��ֻ���ڵײ���չ�ñ�֤��֮ǰ�ı�ע������ݣ�����
//////////////////////////////////////////////////////////////////////////
const std::string TUMOR_TYPES[TUMOR_TYPE_NUM] =
{
    "LGIEN", //�ͼ�����Ƥ������ ����
    "HGIEN", //�߼�����Ƥ������ ����
    "PRE_LGIEN",//�ͼ�����Ƥ������ ��Ƥ������
    "PRE_HGIEN",//�߼�����Ƥ������ ��Ƥ������
    "papillary_adenoma", //��ͷ״�ٰ�
    "tubular_adenoma", //��״�ٰ�
    "mucinous_adenoma", //ճҺ�ٰ�
    "low_adhesion_adenoma", //��ճ���԰�
    "mixed_adenoma", //������ٰ�
    "adenosquamous_carcinoma", //���۰�
    "medullary_carcinoma", //�����ܰ������ʵİ�����������
    "hepatoid_adenocarcinoma", //�����ٰ�
    "squamous-cell_carcinoma", //��״ϸ����
    "undifferentiated_carcinoma", //δ�ֻ���
    "NET1", //NET1�����఩��
    "NET2", //NET2��
    "NEC_big_cell", //��ϸ��NEC
    "NEC_small_cell", //Сϸ��NEC
    "MIX_NET" ,//����������ڷ��ڰ�
    "NO_GEIEN",//����Ƥ������
    "Uncertain_GEIEN",//��ȷ������Ƥ������
    "Uncertain",//��ȷ���Ĳ���
    "SuspectedObject",//���Ʋ���
};

#endif // !TUMOR_TYPE_SHANGHAI_NO1_H
