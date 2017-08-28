#include <iostream>

extern int KFBToTIFF(int argc, char* argv[]);
extern int TIFFLoader(int argc, char* argv[]);
extern int JPEGLoader(int argc, char* argv[]);
extern int KFBLoader(int argc, char* argv[]);
int main(int argc, char* argv[])
{
    return KFBToTIFF(argc, argv);
}