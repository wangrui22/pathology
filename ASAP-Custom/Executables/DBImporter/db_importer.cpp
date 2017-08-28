#include <iostream>
#include <sstream>
#include <fstream>

#include "boost/filesystem.hpp"

//mysql begin
#include "mysql_connection.h"
#include "cppconn/driver.h"
#include "cppconn/exception.h"
#include "cppconn/resultset.h"
#include "cppconn/statement.h"
#include "cppconn/prepared_statement.h"
#include "cppconn/sqlstring.h"
//mysql end

#ifndef WIN32
#include <dlfcn.h>
#endif

#include "io/multiresolutionimageinterface/MultiResolutionImageReader.h"
#include "io/multiresolutionimageinterface/MultiResolutionImage.h"

static std::ofstream out_log;

class LogSheild
{
public:
    LogSheild()
    {
        out_log.open("db_importer.log", std::ios::out);
        if (out_log.is_open())
        {
            out_log << "DB importer log:\n";
        }
    }
    ~LogSheild()
    {
        out_log.close();
    }
protected:
private:
};

static void get_all_files(const std::string& root, const std::vector<std::string>& ext_fliter  , unsigned int& num , std::vector<std::string>& file_names , std::vector<std::string>& file_paths)
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
                bool got_it = false;
                for (int k= 0; k< ext_fliter.size() ; ++k)
                {
                    if (ext == ext_fliter[k])
                    {
                        got_it = true;
                        break;
                    }
                }
                if (got_it)
                {
                    file_names.push_back(it->path().filename().string());
                    file_paths.push_back(root + "/" + it->path().filename().string());
                    ++num;
                }
            }
        }

        for (unsigned int i = 0; i < dirs.size(); ++i)
        {
            const std::string next_dir(root + "/" + dirs[i]);

            get_all_files(next_dir, ext_fliter , num , file_names , file_paths);
        }
    }
}

static void char_to_hex(unsigned char ch, char *szHex)
{
    int i;
    unsigned char byte[2];
    byte[0] = ch / 16;
    byte[1] = ch % 16;
    for (i = 0; i < 2; i++)
    {
        if (byte[i] >= 0 && byte[i] <= 9)
            szHex[i] = '0' + byte[i];
        else
            szHex[i] = 'a' + byte[i] - 10;
    }
    szHex[2] = 0;
}

void char_str_to_hex_str(char *pucCharStr, int iSize, char *pszHexStr)
{
    int i;
    char szHex[3];
    pszHexStr[0] = 0;
    for (i = 0; i < iSize; i++)
    {
        char_to_hex(pucCharStr[i], szHex);
        strcat(pszHexStr, szHex);
    }
}

static bool get_md5(const std::string& file, unsigned char(&md5)[16], MultiResolutionImageReader& img_reader)
{
    MultiResolutionImage* img = img_reader.open(file);
    if (!img)
    {
        out_log << "ERROR : file : " << file << " open failed!\n";
        return false;
    }

    if (!img->valid())
    {
        out_log << "ERROR : file : " << file << " open valid!\n";
        return false;
    }

    if (!img->getImgHash(md5))
    {
        out_log << "ERROR : calculate file : " << file << " md5 failed!\n";
        return false;
    }

    return true;
}

static bool import_img(sql::Connection *con , unsigned int& file_num, std::vector<std::string>& file_names, std::vector<std::string>& file_paths)
{
    try
    {
        sql::Statement *stmt = con->createStatement();
        delete stmt;
        sql::PreparedStatement *pstmt = nullptr;
        sql::ResultSet *res = nullptr;


        MultiResolutionImageReader imgReader;
        unsigned char md5[16];
        for (unsigned int i = 0; i < file_num; ++i)
        {
            //1 calculate md5
            if (!get_md5(file_paths[i], md5, imgReader))
            {
                continue;
            }
            //convert md5 to 32 hex char
            char md5_hex[16 * 2];
            char_str_to_hex_str((char*)md5, 16, md5_hex);

            std::string insert_sql;

            //2 check primary key(md5)
            {
                std::stringstream ss;
                ss << "SELECT * FROM images where md5=\'";
                for (int i = 0; i < 32; ++i)
                {
                    ss << md5_hex[i];
                }
                ss << "\';";
                insert_sql = ss.str();
            }
            sql::PreparedStatement *pstmt = con->prepareStatement(insert_sql.c_str());
            sql::ResultSet *res = pstmt->executeQuery();
            delete pstmt;
            pstmt = nullptr;

            if (res->next())
            {
                out_log << "WARNING : has the same md5 file: " << file_names[i] <<" , use the new one replace it.\n";
                delete res;
                res = nullptr;

                //delete old one 
                {
                    std::stringstream ss;
                    ss << "DELETE FROM images where md5=\'";
                    for (int i = 0; i < 32; ++i)
                    {
                        ss << md5_hex[i];
                    }
                    ss << "\';";
                    insert_sql = ss.str();
                }
                sql::PreparedStatement *pstmt = con->prepareStatement(insert_sql.c_str());
                sql::ResultSet *res = pstmt->executeQuery();
                delete pstmt;
                pstmt = nullptr;
                delete res;
                res = nullptr;
            }


            //2 insert into database
            {
                std::stringstream ss;
                ss << "INSERT INTO images (name , md5 , file_path) values (\'";
                ss << file_names[i] << "\' , \'";
                for (int i = 0; i < 32; ++i)
                {
                    ss << md5_hex[i];
                }
                ss << "\' , \'";
                ss << file_paths[i] << "\');";

                 insert_sql = ss.str();
            }
            pstmt = con->prepareStatement(insert_sql.c_str());
            res = pstmt->executeQuery();
            delete pstmt;
            pstmt = nullptr;
            delete res;
            res = nullptr;
        }

        delete con;
        con = nullptr;
    }
    catch (const sql::SQLException& e)
    {
        out_log << "ERROR : ";
        out_log << "# ERR: SQLException in " << __FILE__;
        out_log << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
        out_log << "# ERR: " << e.what();
        out_log << " (MySQL error code: " << e.getErrorCode();
        out_log << ", SQLState: " << e.getSQLState() << " )" << std::endl;

        delete con;
        con = nullptr;

        return false;
    }

    return true;
}

static bool import_anno(sql::Connection *con, unsigned int& file_num, std::vector<std::string>& file_names, std::vector<std::string>& file_paths)
{
    try
    {
        sql::Statement *stmt = con->createStatement();
        delete stmt;

        for (unsigned int i = 0; i < file_num; ++i)
        {
            //1 get file name without postfix
            std::string no_postfix;
            if (file_names[i].size() < 6)
            {
                out_log << "WARNING : file name error : " << file_names[i] << ".\n";
                continue;
            }
            no_postfix = file_names[i].substr(0, file_names[i].size() - 5);


            //2 select file name
            std::string insert_sql;
            {
                std::stringstream ss;
                ss << "SELECT * FROM images where name like \'";
                ss << no_postfix;
                ss << "%\';";
                insert_sql = ss.str();
            }
            sql::PreparedStatement *pstmt = con->prepareStatement(insert_sql.c_str());
            sql::ResultSet *res = pstmt->executeQuery();
            delete pstmt;
            pstmt = nullptr;

            if (res->next())
            {
                
                delete res;
                res = nullptr;

                //add new anno_file
                {
                    std::stringstream ss;
                    ss << "Update images set anno_path=\'" << file_paths[i] << "\' where name like \'";
                    ss << no_postfix;
                    ss << "%\';";
                    insert_sql = ss.str();
                }
                sql::PreparedStatement *pstmt = con->prepareStatement(insert_sql.c_str());
                sql::ResultSet *res = pstmt->executeQuery();
                delete pstmt;
                pstmt = nullptr;
                delete res;
                res = nullptr;
            }
            else
            {
                out_log << "WARNING : insert file: " << file_names[i] << " failed! You should import image first.\n";
            }
        }

        delete con;
        con = nullptr;
    }
    catch (const sql::SQLException& e)
    {
        out_log << "ERROR : ";
        out_log << "# ERR: SQLException in " << __FILE__;
        out_log << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
        out_log << "# ERR: " << e.what();
        out_log << " (MySQL error code: " << e.getErrorCode();
        out_log << ", SQLState: " << e.getSQLState() << " )" << std::endl;

        delete con;
        con = nullptr;

        return false;
    }

    return true;
}

void test()
{

}

int main(int argc , char* argv[])
{

    LogSheild log_sheild;
    //exe "type" "ip" "username" "password "database" "file_root"
    if(argc == 2 && std::string(argv[1]) =="-h" || std::string(argv[1])=="--help")
    {
        std::cout << "\tFormat :[import_type(image or annotation)] [ip] [username] [password] [database] [file_root]\n";
        return 0;
    }
    if (argc != 7)  
    {
        out_log << "ERROR : invalid input.\n";
        out_log << "\tFormat :import_type ip username password database file_root\n";
        out_log << "\tImport type : image or annotation\n";
        return -1;
    }

    //Get all files
    const std::string import_type = argv[1];
    const std::string ip = std::string("tcp://") + std::string(argv[2]);
    const std::string user_name = argv[3];
    const std::string password = argv[4];
    const std::string database = argv[5];
    std::string file_root = argv[6];
    for (int i = 0; i < file_root.size() ; ++i)
    {
        if (file_root[i] == '\\')
        {
            file_root[i] = '/';
        }
    }

    out_log << "IP : " << ip << std::endl;
    out_log << "User name : " << user_name << std::endl;
    out_log << "Password : " << password << std::endl;
    out_log << "Database : " << database << std::endl;
    out_log << "Import type : " << import_type<< std::endl;
    out_log << "File root : " << file_root << std::endl;


    //Import to database
    std::vector<std::string> file_names;
    std::vector<std::string> file_paths;
    std::vector<std::string> filter_ext;
    unsigned int file_num = 0;
    if (import_type == "image")
    {
        //////////////////////////////////////////////////////////////////////////
        //Formats from http://openslide.org/
        //Aperio(.svs, .tif)
        //Hamamatsu(.vms, .vmu, .ndpi)
        //Leica(.scn)
        //MIRAX(.mrxs)
        //Philips(.tiff)
        //Sakura(.svslide)
        //Trestle(.tif)
        //Ventana(.bif, .tif)
        //Generic tiled TIFF(.tif)
        //////////////////////////////////////////////////////////////////////////
        filter_ext.push_back(".tif");
        filter_ext.push_back(".svs");
        filter_ext.push_back(".vms");
        filter_ext.push_back(".vmu");
        filter_ext.push_back(".ndpi");
        filter_ext.push_back(".scn");
        filter_ext.push_back(".mrxs");
        filter_ext.push_back(".tiff");
        filter_ext.push_back(".svslide");
        filter_ext.push_back(".bif");
    }
    else if (import_type == "annotation")
    {
        filter_ext.push_back(".araw");
    }
    else
    {
        out_log << "ERROR : invalid input , invalid import type!\n";
        return -1;
    }

    get_all_files(file_root, filter_ext, file_num, file_names, file_paths);

    if (file_num != file_names.size() || file_num != file_paths.size())
    {
        out_log << "ERROR : Get files error!\n";
        return -1;
    }
    if (file_paths.empty())
    {
        out_log << "ERROR : Empty files!\n";
        return -1;
    }

    out_log << "File to be import list : \n";
    for (unsigned int i = 0 ; i< file_num ; ++i)
    {
        out_log << "\t" << file_paths[i] << std::endl;
    }
    out_log << std::endl;


    //////////////////////////////////////////////////////////////////////////
    //Connect database
    sql::Connection *con = nullptr;
    try
    {
        //create connect
        sql::Driver *driver = get_driver_instance();
        con = driver->connect(ip.c_str(), user_name.c_str(), password.c_str());
        //con = driver->connect("tcp://127.0.0.1:3306", "root", "0123456");
        con->setSchema(database.c_str());
    }
    catch (const sql::SQLException& e)
    {
        out_log << "ERROR : ";
        out_log << "# ERR: SQLException in " << __FILE__;
        out_log << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
        out_log << "# ERR: " << e.what();
        out_log << " (MySQL error code: " << e.getErrorCode();
        out_log << ", SQLState: " << e.getSQLState() << " )" << std::endl;

        delete con;
        con = nullptr;

        return -1;
    }
    //////////////////////////////////////////////////////////////////////////

    bool import_status = false;;
    if (import_type == "image")
    {
        import_status = import_img(con, file_num, file_names, file_paths);
    }
    else if (import_type == "annotation")
    {
        import_status = import_anno(con, file_num, file_names, file_paths);
    }

    if (import_status)
    {
        out_log << "Import to database success.\n";
        return 0;
    }
    else
    {
        out_log << "Import to database failed.\n";
        return -1;
    }
}