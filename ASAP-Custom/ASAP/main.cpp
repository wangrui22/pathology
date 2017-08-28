#include "pathologyworkstation.h"
#include <QApplication>

#ifdef _DEBUG
#pragma comment( linker, "/subsystem:\"console\" /entry:\"mainCRTStartup\"" )
#endif

int main(int argc, char *argv[])
{    
    QApplication::addLibraryPath("./plugins");
    QApplication a(argc, argv);
    PathologyWorkstation w;
    w.show();

    return a.exec();
}
