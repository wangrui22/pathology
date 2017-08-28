#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(ZOOMTOOLPLUGIN_LIB)
#  define ZOOMTOOLPLUGIN_EXPORT Q_DECL_EXPORT
# else
#  define ZOOMTOOLPLUGIN_EXPORT Q_DECL_IMPORT
# endif
#else
# define ZOOMTOOLPLUGIN_EXPORT
#endif
