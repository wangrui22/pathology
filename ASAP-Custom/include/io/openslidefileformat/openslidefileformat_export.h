
#ifndef OPENSLIDEFILEFORMAT_EXPORT_H
#define OPENSLIDEFILEFORMAT_EXPORT_H

#ifdef WIN32
    #ifdef OPENSLIDEFILEFORMAT_STATIC_DEFINE
    #  define OPENSLIDEFILEFORMAT_EXPORT
    #  define OPENSLIDEFILEFORMAT_NO_EXPORT
    #else
    #  ifndef OPENSLIDEFILEFORMAT_EXPORT
    #    ifdef openslidefileformat_EXPORTS
            /* We are building this library */
    #      define OPENSLIDEFILEFORMAT_EXPORT __declspec(dllexport)
    #    else
            /* We are using this library */
    #      define OPENSLIDEFILEFORMAT_EXPORT __declspec(dllimport)
    #    endif
    #  endif
    
    #  ifndef OPENSLIDEFILEFORMAT_NO_EXPORT
    #    define OPENSLIDEFILEFORMAT_NO_EXPORT 
    #  endif
    #endif
    
    #ifndef OPENSLIDEFILEFORMAT_DEPRECATED
    #  define OPENSLIDEFILEFORMAT_DEPRECATED __declspec(deprecated)
    #endif
    
    #ifndef OPENSLIDEFILEFORMAT_DEPRECATED_EXPORT
    #  define OPENSLIDEFILEFORMAT_DEPRECATED_EXPORT OPENSLIDEFILEFORMAT_EXPORT OPENSLIDEFILEFORMAT_DEPRECATED
    #endif
    
    #ifndef OPENSLIDEFILEFORMAT_DEPRECATED_NO_EXPORT
    #  define OPENSLIDEFILEFORMAT_DEPRECATED_NO_EXPORT OPENSLIDEFILEFORMAT_NO_EXPORT OPENSLIDEFILEFORMAT_DEPRECATED
    #endif
    
    #if 0 /* DEFINE_NO_DEPRECATED */
    #  ifndef OPENSLIDEFILEFORMAT_NO_DEPRECATED
    #    define OPENSLIDEFILEFORMAT_NO_DEPRECATED
    #  endif
    #endif
#else
    #  define OPENSLIDEFILEFORMAT_EXPORT
    #  define OPENSLIDEFILEFORMAT_NO_EXPORT
#endif
    
#endif
