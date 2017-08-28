#include "BinaryRepository.h"

#include <iostream>
#include <fstream>
#include <time.h>
#include <stdio.h>
#include <set>

#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/bignum.h"
#include "mbedtls/x509.h"
#include "mbedtls/rsa.h"

#include "Annotation.h"
#include "AnnotationGroup.h"
#include "AnnotationList.h"
#include "Annotation_define.h"



namespace
{
    

    //////////////////////////////////////////////////////////////////////////
    //RSA related

    //////////////////////////////////////////////////////////////////////////
    //Key
    //Version 1.1 RSA
    static const char* S_N = "92A70F134D1248C95159A6B826D45E2D74A70A2E9BF49FCFF854C281CE68A9D24145CC4857D8913641DB68DB37D2D565BBFA5A73FDB675B486562F0B90CADB5211A1713A932E8B3FAE720E72584C11B6FE8012500F1C02EA8915B840357B5A068A21161DFBBB09C43704D2BAE40D4C9078A400D35704FDD7BAD9064A094F2F9EC36EF620D96A289EE50B7AF8285EC8BF186A89F26F89A93E9C7AD5A97A16109D8BD492133E72E55C5364AE10FFFBDE91A36A9D2BC9B3B260F4F8866A9C653C56199C0E8D4DC2FF4704DF1BA35E32FB64F2D4999ECE3042C5FB2565BFAA7C09078700341249E17318EE33206AB6ED8A13A419A3A4E139E1347A4EB301CC644499";
    static const char* S_E = "010001";
    static const char* S_D = "6563F6BA4368C6C5D8E680AB84BD02B7AE94E6273F4FAB45DFCCCA0570AD4FEBBB910421680AA4AABC7173574C8B095AA2D1FDC37FBFAC3539435097506DCFA037EC441A5933AEB642673CB1B576323130575F51D680694BB9A73891BBE8225FAF141CF471DBE86EAE13399658A1113439143A35E34DE22A4A564D50D68AD6C280569BE644A6E7B362EA7A2C7A1687A6134D53E8CD74DF914F3334E0D600659855684D1615CBBBEDF72B3606E46C1C52099EDAADA2D51A1BE34ADA9A8E593AEEDCE2A596664B8E1BC13A2BFBD5ECEAE5A286F1344F438CB5B9DB29CC011DD2770381CC1B7B1B6500FCD55F1807CDB2FA05D1A91F7E3C6FDC59B6F1897B6ADF01";
    static const char* S_P = "E03BAECFF8E7FCC7A4DDF3E6A0357069F66C1AFD4A7C3F8BC094B4ACFC2430150B0C05C690B87B9FEE73189EF020D8E40B91F185B9E51AC59C4C67E6B32E66BAD4BFDE25A09CBD7EBED1B39E4608E1CBC55137A1E9B4409F388733DD43B5075C3DD1EFFD71BB4222C5582564CEF5B894584D77A8AF3EA4DA42237D388B3A9E11";
    static const char* S_Q = "A76DBE3B41D11F34B29A0BC7E3840A30A99B96083333A5652AE7B8C1664F48DC832CE6644779E172F8F18DEE363691EE9A1CD47BDEDCE19DA4C5F0ACBE5AD1444E43B19D2313680957A877EE1F4492330D5F791C9BD9670A9332CA7BBF6A3F6B22E4580D6F9EAE8BFF64D714D4095713496D2535BFDA3A64A109420E338B5609";
    static const char* S_DP = "614943B533D6E313737260E9C4C4AABB2E9104BCB45E67EBA8685C63591A124A6EFA8E5EF257383BD166DA5DCAED556CCBA04C5BAAA5BE11D2984BD40A15ADB88DA999265622EB0C5C99FB064678800EBAD35C01C1B5D4FBBFB8D945BCA48446FF0360CE6353CD83EC7F2BEA2AE8ED665F516D0320D55B0CDEF92AD127870D21";
    static const char* S_DQ = "61B1EE9BA327043F7AABDA8DF39B1B5B13CCBE79E06E9566F7E7286E6965800D73D962424756BE0F996551BD1B133289F8913407679E9963A43EFFD5BC693FCCCCA2416480901557C6377E6B8B013711E50EBD9BCCF0C9686945ECD2D2798BB1D735488FE67EC038DE7897EC0661C0D67734E1CE803935737C67EB8C5FD54109";
    static const char* S_QP = "01C442068CDC045E29096E5854B4299BC889FC983CD51F29832BC9F67BA589DB6BCD24AFEEC5BFB74E87218600348B2920BBD3045968BD4DE8C6D2EB57A228ABD7066924BF3DA931C6AC26285CA326A2FA4F89024DE37931916C0DF0B6C2E186F6C02D8055C6A09EC36F85614E97D92C5981A4F8BB0C0F9A4A13EF0132931659";

    //////////////////////////////////////////////////////////////////////////

    int to_rsa_context(
        const char* n,
        const char* e,
        const char* d,
        const char* p,
        const char* q,
        const char* dp,
        const char* dq,
        const char* qp,
        mbedtls_rsa_context& rsa)
    {
        mbedtls_rsa_init(&rsa, MBEDTLS_RSA_PKCS_V15, 0);

        int status = -1;
        if ((status = mbedtls_mpi_read_string(&rsa.N, 16, n)) != 0 ||
            (status = mbedtls_mpi_read_string(&rsa.E, 16, e)) != 0 ||
            (status = mbedtls_mpi_read_string(&rsa.D, 16, d)) != 0 ||
            (status = mbedtls_mpi_read_string(&rsa.P, 16, p)) != 0 ||
            (status = mbedtls_mpi_read_string(&rsa.Q, 16, q)) != 0 ||
            (status = mbedtls_mpi_read_string(&rsa.DP, 16, dp)) != 0 ||
            (status = mbedtls_mpi_read_string(&rsa.DQ, 16, dq)) != 0 ||
            (status = mbedtls_mpi_read_string(&rsa.QP, 16, qp)) != 0)
        {
            return -1;
        }

        rsa.len = (mbedtls_mpi_bitlen(&rsa.N) + 7) >> 3;

        return 0;
    }

    int entrypt(const mbedtls_rsa_context& rsa, size_t len_context, unsigned char* context, unsigned char(&output)[512])
    {
        int return_val;
        const char *pers = "rsa_encrypt";

        mbedtls_entropy_context entropy;
        mbedtls_ctr_drbg_context ctr_drbg;

        mbedtls_ctr_drbg_init(&ctr_drbg);
        mbedtls_entropy_init(&entropy);

        return_val = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func,
            &entropy, (const unsigned char *)pers, strlen(pers));

        if (return_val != 0)
        {
            return -1;
        }

        return_val = mbedtls_rsa_pkcs1_encrypt(const_cast<mbedtls_rsa_context*>(&rsa), mbedtls_ctr_drbg_random,
            &ctr_drbg, MBEDTLS_RSA_PUBLIC, len_context, context, output);
        if (return_val != 0)
        {
            return -1;
        }

        return 0;
    }

    int detrypt(const mbedtls_rsa_context& rsa, size_t len_context, unsigned char* context, unsigned char(&output)[1024])
    {
        int return_val;
        const char *pers = "rsa_decrypt";

        mbedtls_entropy_context entropy;
        mbedtls_ctr_drbg_context ctr_drbg;

        mbedtls_ctr_drbg_init(&ctr_drbg);
        mbedtls_entropy_init(&entropy);

        return_val = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func,
            &entropy, (const unsigned char *)pers, strlen(pers));

        if (return_val != 0)
        {
            return -1;
        }

        return_val = mbedtls_rsa_pkcs1_decrypt(const_cast<mbedtls_rsa_context*>(&rsa), mbedtls_ctr_drbg_random,
            &ctr_drbg, MBEDTLS_RSA_PRIVATE, &len_context, context, output, 1024);
        if (return_val != 0)
        {
            return -1;
        }

        return 0;
    }

    //////////////////////////////////////////////////////////////////////////
    struct PointUnitRSA
    {
        int entrypt_id;
        int pos_num;//full is 12 -> 12*4*2 = 96 bytes < rsa_N
        unsigned char entrypt_pos[512];

        PointUnitRSA()
        {
            pos_num = 0;
            entrypt_id = 0;//0 for no entrypt
            memset(entrypt_pos, 0, sizeof(entrypt_pos));
        }
    };

    int get_rand_entrypt_id()
    {
        static int tick = 0;
        tick += (int(clock()) % 13 + 1);

        int id = 0;
        if (tick > 41)
        {
            id = 1;
            tick = 0;
        }
        else
        {
            id = 0;
        }

        return id;
    }
}

BinaryRepository::BinaryRepository(const std::shared_ptr<AnnotationList>& list):Repository(list), _rsa(true)
{

}

BinaryRepository::~BinaryRepository()
{

}

bool BinaryRepository::save() const
{
    int status = 0;
    mbedtls_rsa_context rsa;
    status = to_rsa_context(S_N, S_E, S_D, S_P, S_Q, S_DP, S_DQ, S_QP, rsa);
    if (status != 0)
    {
        return false;
    }

    std::ofstream out(_source.c_str(), std::ios::binary | std::ios::out);
    if (!out.is_open())
    {
        return false;
    }

    const std::vector<std::shared_ptr<Annotation> >& annos = _list->getAnnotations();
    const std::vector<std::shared_ptr<AnnotationGroup> >& groups = _list->getGroups();

    const int group_num = static_cast<int>(groups.size());
    const int anno_num = static_cast<int>(annos.size());

    unsigned int point_num(0);
    for (int i = 0 ; i<annos.size() ; ++i)
    {
        point_num += annos[i]->getNumberOfPoints();
    }

    //1 write header
    AnnotationFileHeader header;
    header.group_num = group_num;
    header.anno_num = anno_num;
    header.valid = 0;//set to 1 in the end
    out.write((char*)(&header), sizeof(header));
    if (!out.good())
    {
        out.close();
        return false;
    }

    //2 write groups info
    for (int i = 0; i<group_num ; ++i)
    {
        std::string name = groups[i]->getName();
        if (name.size() > 255)
        {
            out.close();
            return false;
        }

        std::string group_name = "";
        if (groups[i]->getGroup())
        {
            group_name = groups[i]->getGroup()->getName();
        }
        if (group_name.size() > 255)
        {
            out.close();
            return false;
        }

        std::string color = groups[i]->getColor();

        GroupUnit group;
        memcpy(group.name_str, name.c_str(), name.size());
        memcpy(group.group_name_str, group_name.c_str(), group_name.size());
        memcpy(group.color_str, color.c_str(), color.size());

        group.name_str[name.size()] = '\0';
        group.group_name_str[group_name.size()] = '\0';
        group.color_str[color.size()] = '\0';

        out.write((char*)(&group), sizeof(group));
        if (!out.good())
        {
            out.close();
            return false;
        }
    }

    //3 write annotation
    for (int i = 0; i <anno_num ; ++i)
    {
        std::string name = annos[i]->getName();
        if (name.size() > 255)
        {
            out.close();
            return false;
        }

        std::string group_name = "";
        if (annos[i]->getGroup())
        {
            group_name = annos[i]->getGroup()->getName();
        }
        if (group_name.size() > 255)
        {
            out.close();
            return false;
        }

        std::string color = annos[i]->getColor();

        AnnotationUnit unit;
        memcpy(unit.name_str, name.c_str(), name.size());
        memcpy(unit.group_name_str, group_name.c_str(), group_name.size());
        memcpy(unit.color_str, color.c_str(), color.size());

        unit.name_str[name.size()] = '\0';
        unit.group_name_str[group_name.size()] = '\0';
        unit.color_str[color.size()] = '\0';

        unit.point_num = annos[i]->getNumberOfPoints();

        //tumor types
        unit.anno_type_id = static_cast<int>( annos[i]->getType());
        std::set<std::string> tumor_types =  annos[i]->getTumorTypes();

        unsigned char tumor_type_id[TUMOR_TYPE_NUM];
        memset(tumor_type_id, 0, sizeof(tumor_type_id));
        for (auto it = tumor_types.begin() ; it != tumor_types.end() ; ++it)
        {
            int id = get_tumor_type_id(*it);
            if (-1 != id)
            {
                tumor_type_id[id] = 1;
            }
        }

        status = entrypt(rsa, sizeof(tumor_type_id), tumor_type_id, unit.entrypt_tumor_type_id);
        if (status != 0)
        {
            out.close();
            return false;
        }


        out.write((char*)(&unit), sizeof(unit));
        if (!out.good())
        {
            out.close();
            return false;
        }
    }

    //4 write coordinate
    const int pos_cluster_num = 12;
    float pos_array[pos_cluster_num*2];
    int current_pos_num = 0;

    memset(pos_array, 0, sizeof(pos_array));

    for (int i = 0 ; i < anno_num ; ++i)
    {
        const std::vector<Point>& pos_set = annos[i]->getCoordinates();
        for (int j = 0;  j < pos_set.size() ; ++j)
        {
            pos_array[current_pos_num*2] = pos_set[j].getX();
            pos_array[current_pos_num * 2+1] = pos_set[j].getY();
            ++current_pos_num;

            if (current_pos_num == 12)
            {
                int entrypt_id = _rsa ? get_rand_entrypt_id() : 0;
                PointUnitRSA pos_ras;
                pos_ras.pos_num = 12;
                if (0 != entrypt_id)
                {
                    pos_ras.entrypt_id = 1;
                    status = entrypt(rsa, sizeof(pos_array), (unsigned char*)(pos_array), pos_ras.entrypt_pos);
                    if (status != 0)
                    {
                        out.close();
                        return false;
                    }
                }
                else
                {
                    memcpy((char*)pos_ras.entrypt_pos , pos_array , sizeof(pos_array));
                }

                out.write((char*)(&pos_ras), sizeof(pos_ras));
                if (!out.good())
                {
                    out.close();
                    return false;
                }
                current_pos_num = 0;

                memset(pos_array, 0, sizeof(pos_array));
            }
        }
    }

    if (current_pos_num != 0)
    {
        PointUnitRSA pos_ras;
        pos_ras.entrypt_id = 1;
        pos_ras.pos_num = current_pos_num;
        status = entrypt(rsa, sizeof(pos_array), (unsigned char*)(pos_array), pos_ras.entrypt_pos);
        if (status != 0)
        {
            out.close();
            return false;
        }

        out.write((char*)(&pos_ras), sizeof(pos_ras));
        if (!out.good())
        {
            out.close();
            return false;
        }
        current_pos_num = 0;

        memset(pos_array, 0, sizeof(pos_array));
    }

    //write valid flag
    out.seekp(0);
    header.valid = 1;//set to 1 in the end
    out.write((char*)(&header), sizeof(header));
    if (!out.good())
    {
        out.close();
        return false;
    }

    out.close();

    std::cout << "save binary repository done \n";
    return true;
}

bool BinaryRepository::loadFromRepo()
{
    int status = 0;
    mbedtls_rsa_context rsa;
    status = to_rsa_context(S_N, S_E, S_D, S_P, S_Q, S_DP, S_DQ, S_QP, rsa);
    if (status != 0)
    {
        return false;
    }

    if (!_list) {
        return false;
    }

    _list->removeAllAnnotations();
    _list->removeAllGroups();

    std::ifstream in(_source.c_str(), std::ios::in | std::ios::binary);
    if (!in.is_open())
    {
        return false;
    }

    //read header
    AnnotationFileHeader header;
    in.read((char*)(&header), sizeof(header));
    if (!in.good() || 0 == header.valid)
    {
        in.close();
        return false;
    }

    const int anno_num = header.anno_num;
    const int group_num = header.group_num;

    //read group 
    std::map<std::string, std::pair<std::string , std::shared_ptr<AnnotationGroup>>> group_set;
    for (int i = 0; i < group_num ; ++i)
    {
        GroupUnit group_unit;
        in.read((char*)(&group_unit), sizeof(group_unit));
        if (!in.good())
        {
            _list->removeAllAnnotations();
            _list->removeAllGroups();
            in.close();
            return false;
        }

        std::string name;
        char_array_to_string(group_unit.name_str, sizeof(group_unit.name_str), name);

        std::string group_name;
        char_array_to_string(group_unit.group_name_str, sizeof(group_unit.group_name_str), group_name);

        std::string color;
        char_array_to_string(group_unit.color_str, sizeof(group_unit.color_str), color);

        std::shared_ptr<AnnotationGroup> group(new AnnotationGroup());
        group->setName(name);
        group->setColor(color);

        group_set[name] = std::make_pair(group_name , group);

        _list->addGroup(group);
    }

    if (group_set.size() != group_num)
    {
        _list->removeAllAnnotations();
        _list->removeAllGroups();
        in.close();
        return false;
    }

    //loop again to set group to group
    for (auto it = group_set.begin() ; it != group_set.end() ; ++it)
    {
        std::string group_name = it->second.first;
        if (!group_name.empty())
        {
            auto group = _list->getGroup(group_name);
            if (group)
            {
                it->second.second->setGroup(group);
            }
            else
            {
                _list->removeAllAnnotations();
                _list->removeAllGroups();
                in.close();
                return false;
            }
        }
    }

    //read annotation
    std::vector<unsigned int> each_anno_points_num;
    unsigned int annos_points_sum = 0;
    for (int i = 0 ; i < anno_num ; ++i)
    {
        AnnotationUnit anno_unit;

        in.read((char*)(&anno_unit), sizeof(anno_unit));
        if (!in.good())
        {
            _list->removeAllAnnotations();
            _list->removeAllGroups();
            in.close();
            return false;
        }

        std::string name;
        char_array_to_string(anno_unit.name_str, sizeof(anno_unit.name_str), name);

        std::string group_name;
        char_array_to_string(anno_unit.group_name_str, sizeof(anno_unit.group_name_str), group_name);

        std::string color;
        char_array_to_string(anno_unit.color_str, sizeof(anno_unit.color_str), color);

        std::shared_ptr<Annotation> anno(new Annotation());
        anno->setName(name);
        anno->setColor(color);

        if (!group_name.empty())
        {
            auto group = _list->getGroup(group_name);
            if (group)
            {
                anno->setGroup(group);
            }
            else
            {
                _list->removeAllAnnotations();
                _list->removeAllGroups();
                in.close();
                return false;
            }
        }

        anno->setType(static_cast<Annotation::Type>(anno_unit.anno_type_id));
        each_anno_points_num.push_back(anno_unit.point_num);
        annos_points_sum += anno_unit.point_num;


        //decrypt tumor type
        unsigned char detrypt_tumor_type_id[1024];
        memset(detrypt_tumor_type_id, 0, sizeof(detrypt_tumor_type_id));
        status = detrypt(rsa, sizeof(anno_unit.entrypt_tumor_type_id), (unsigned char*)(anno_unit.entrypt_tumor_type_id), detrypt_tumor_type_id);
        if (status != 0)
        {
            _list->removeAllAnnotations();
            _list->removeAllGroups();
            in.close();
            return false;
        }

        for (int i = 0 ; i<TUMOR_TYPE_NUM ; ++i)
        {
            if (0 != detrypt_tumor_type_id[i])
            {
                anno->addTumorType(TUMOR_TYPES[i]);
            }
        }


        _list->addAnnotation(anno);
    }

    //read coordinate
    int pos_cluster_num = annos_points_sum / 12;
    if (pos_cluster_num*12 != annos_points_sum)
    {
        pos_cluster_num += 1;
    }
    std::unique_ptr<PointUnitRSA[]> point_ras_array ( new PointUnitRSA[pos_cluster_num]);
    in.read((char*)point_ras_array.get(), sizeof(PointUnitRSA)*pos_cluster_num);
    if (!in.good())
    {
        _list->removeAllAnnotations();
        _list->removeAllGroups();
        in.close();
    }

    int entrypt_sum = 0;
    std::vector<std::shared_ptr<Annotation>> anno_set = _list->getAnnotations();
    unsigned int current_annos_points_sum = 0;
    int current_anno_id = 0;
    unsigned char detrypt_point[1024];
    for (int i = 0; i< pos_cluster_num ;  ++i)
    {
        const PointUnitRSA& point_ras = point_ras_array[i];

        current_annos_points_sum += point_ras.pos_num;
        float* point_array = nullptr;

        if (0 != point_ras.entrypt_id)
        {
            ++entrypt_sum;
            memset(detrypt_point, 0, sizeof(detrypt_point));

            status = detrypt(rsa, sizeof(point_ras.entrypt_pos), (unsigned char*)(point_ras.entrypt_pos), detrypt_point);
            if (status != 0)
            {
                _list->removeAllAnnotations();
                _list->removeAllGroups();
                in.close();
                return false;
            }
            point_array = (float*)(detrypt_point);
        }
        else
        {
            point_array = (float*)point_ras.entrypt_pos;
        }

        int idx = 0;
        while (idx < point_ras.pos_num)
        {
            if (anno_set[current_anno_id]->getNumberOfPoints() < each_anno_points_num[current_anno_id])
            {
                anno_set[current_anno_id]->addCoordinate(Point(point_array[idx * 2], point_array[idx * 2 + 1]));
                ++idx;
            }
            else
            {
                ++current_anno_id;
                if (current_anno_id >= anno_set.size())
                {
                    break;
                }
            }
        }

        if (current_annos_points_sum >= annos_points_sum || current_anno_id >= anno_set.size())
        {
            break;
        }
    }


    if (current_annos_points_sum == annos_points_sum &&
        current_anno_id == anno_num-1 &&
        each_anno_points_num[anno_num-1] == anno_set[anno_num-1]->getNumberOfPoints())
    {
        //std::cout << "Entrypt point is " << entrypt_sum * 12 << std::endl;
        in.close();
        return true;
    }
    else
    {
        _list->removeAllAnnotations();
        _list->removeAllGroups();
        in.close();
        return false;
    }
}

void BinaryRepository::close_rsa(bool close_flag)
{
    _rsa = close_flag;
}
