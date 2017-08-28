#include <stdlib.h>
#include <stdio.h>

//Socket header
#ifdef WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

//mysql begin
#include "mysql_connection.h"
#include "cppconn/driver.h"
#include "cppconn/exception.h"
#include "cppconn/resultset.h"
#include "cppconn/statement.h"
#include "cppconn/prepared_statement.h"
#include "cppconn/sqlstring.h"  
//mysql end

#include "annotation/AnnotationList.h"
#include "annotation/Annotation.h"
#include "annotation/AnnotationGroup.h"
#include "annotation/BinaryRepository.h"
#include "annotation/Annotation_define.h"

static std::ofstream out_log;

class LogSheild
{
public:
    LogSheild()
    {
        out_log.open("anno_server.log", std::ios::out);
        if (out_log.is_open())
        {
            out_log << "Annotation server log:\n";
        }
    }
    ~LogSheild()
    {
        out_log.close();
    }
protected:
private:
};

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

static void char_str_to_hex_str(char *pucCharStr, int iSize, char *pszHexStr)
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

static bool query_path(std::string& anno_path , char (&md5_hex)[32])
{
    try
    {
        //create connect
        sql::Driver *driver = get_driver_instance();
        sql::Connection *con = driver->connect("tcp://127.0.0.1:3306", "root", "6ckj1sWR");
        con->setSchema("pathology");

        sql::Statement *stmt = con->createStatement();
        delete stmt;

        std::stringstream ss;
        ss << "SELECT * FROM images WHERE md5=\'";
        for (int i = 0; i < 32; ++i)
        {
            ss << md5_hex[i];
        }
        ss << "\';";
        //std::cout << "query cmd : " << ss.str().c_str() << std::endl;

        sql::PreparedStatement *pstmt = con->prepareStatement(ss.str().c_str());
        sql::ResultSet *res = pstmt->executeQuery();

        if (res->next())
        {
            anno_path = res->getString("anno_path");

            delete res;
            res = nullptr;
            delete pstmt;
            pstmt = nullptr;
            delete con;
            con = nullptr;

            return !anno_path.empty();
        }
        else
        {
            delete res;
            res = nullptr;
            delete pstmt;
            pstmt = nullptr;
            delete con;
            con = nullptr;

            return false;
        }
    }
    catch (const sql::SQLException& e)
    {
        std::cout << "# ERR: SQLException in " << __FILE__;
        std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
        std::cout << "# ERR: " << e.what();
        std::cout << " (MySQL error code: " << e.getErrorCode();
        std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return false;
    }
}

#ifdef WIN32

#else
// void handle_connect(int serversockfd)
// {
//     sockaddr_in cli_addr;
//     socklen_t cli_add_len = sizeof(cli_addr);
//     int clientsockfd = accept(serversockfd, (struct sockaddr*)&cli_addr, &cli_add_len);
//     if(clientsockfd < 0)
//     {
//         std::cout << "error on accept.\n";
//         continue;
//     }

//     char md5[16];
//     read(clientsockfd ,(char*)(md5), 16);
// }
#endif

//////////////////////////////////////////////////////////////////////////
//
int main(int argc, char* argv[])
{
    std::cout << "enter main\n";
    
    std::string ip = "127.0.0.1";
    int port = 1234;

    if (argc == 1 )
    {
        std::cout << "Has not ip & port input , use local ip : 127.0.0.1 and port 1234\n";
    }
    else if(argc == 3)
    {
        ip = argv[1];
        port = atoi(argv[2]);
        std::cout << "IP : " << ip << std::endl;
        std::cout << "Port : " << port << std::endl;
    }

    LogSheild log_sheild;

#ifdef WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif


    //create socket
#ifdef WIN32
    SOCKET servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
#else
    int serversockfd = socket(AF_INET , SOCK_STREAM , 0);
    if(serversockfd<0)
    {
        std::cout << "Create socket failed!\n";
    }
#endif

    
#ifdef WIN32
    //bind
    sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));  //每个字节都用0填充
    sockAddr.sin_family = PF_INET;  //使用IPv4地址
    sockAddr.sin_addr.s_addr = inet_addr(ip.c_str());  //具体的IP地址
    sockAddr.sin_port = htons(port);  //端口
    bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));

    //listen
    listen(servSock, 20);
    std::cout << "server running ...\n";
    out_log << "server running ...\n";
#else
    //bind
    sockaddr_in serv_addr;
    bzero(((char*)&serv_addr) , sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ip.c_str());  //具体的IP地址
    serv_addr.sin_port = htons(port);  //端口
    bind(serversockfd,(struct sockaddr *)&serv_addr ,sizeof(serv_addr)); 

    //listen
    listen(serversockfd, 20);
    std::cout << "server running ...\n";
    out_log << "server running ...\n";
#endif

    std::cout << "Jump outof listen\n";
    
//accept loop
    while (1)
    {
#ifdef WIN32
        SOCKADDR clntAddr;
        int nSize = sizeof(SOCKADDR);
        SOCKET clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize);
#else
        sockaddr_in cli_addr;
        socklen_t cli_add_len = sizeof(cli_addr);
        int clientsockfd = accept(serversockfd, (struct sockaddr*)&cli_addr, &cli_add_len);
        if(clientsockfd < 0)
        {
            std::cout << "error on accept.\n";
            continue;
        }
#endif

        char md5[16];
#ifdef WIN32
        recv(clntSock, (char*)(md5), 16, NULL);
#else
        read(clientsockfd ,(char*)(md5), 16);
#endif
        
        char md5_hex[16 * 2];
        char_str_to_hex_str(md5, 16, md5_hex);
        std::cout << "calculate md5 is :" << md5_hex << "\n";

        //check in DB
        std::string anno_path;
        if (!query_path(anno_path , md5_hex))
        {
            std::cout << "query failed!\n";
            AnnotationFileHeader empty_file_header;
            empty_file_header.anno_num = 0;
            empty_file_header.group_num = 0;
            empty_file_header.valid = true;

#ifdef WIN32
            send(clntSock, (char*)(&empty_file_header), sizeof(empty_file_header) , NULL);
#else
            write(clientsockfd , (char*)(&empty_file_header) , sizeof(empty_file_header));
#endif
        }
        else
        {
            std::cout << "query success!\t";
            std::cout << "annotation path : " << anno_path << std::endl;

            std::shared_ptr<AnnotationList> anno_list(new AnnotationList());
            std::shared_ptr<Repository> rep(new BinaryRepository(anno_list));
            rep->setSource(anno_path);
            if (rep->load())
            {
                char* buffers = nullptr;
                unsigned int size = 0;
                if (anno_list->write(buffers, size))
                {
                    std::cout << "send learning result to client. buffer size : " << size << std::endl;

#ifdef WIN32
                    send(clntSock, buffers, size, NULL);
#else
                    write(clientsockfd , buffers, size);
#endif
                    delete[] buffers;
                }
                else
                {
                    std::cout << "send empty learning result to client.\n";
                    AnnotationFileHeader empty_file_header;
                    empty_file_header.anno_num = 0;
                    empty_file_header.group_num = 0;
                    empty_file_header.valid = true;
#ifdef WIN32
                    send(clntSock, (char*)(&empty_file_header), sizeof(empty_file_header), NULL);
#else
                    write(clientsockfd, (char*)(&empty_file_header), sizeof(empty_file_header));
#endif
                }
            }
        }

        
#ifdef WIN32
        closesocket(clntSock);
#else
        close(clientsockfd);
#endif

    }

#ifdef WIN32
    //关闭 server 套接字 
    closesocket(servSock);
    //终止 DLL 的使用
    WSACleanup();
#else
    close(serversockfd);
#endif

    return 0;
}