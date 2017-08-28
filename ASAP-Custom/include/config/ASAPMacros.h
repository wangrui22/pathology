/***************************************************************-*-c++-*-

  

  $Id: diag_config.h,v 1.27 2007/06/27 15:23:12 yulia Exp $

*************************************************************************/

#ifndef __ASAPMACROS_H__
#define __ASAPMACROS_H__

#define ASAP_MAJOR_VERSION 1
#define ASAP_MINOR_VERSION 7
#define ASAP_PATCH_VERSION 1
static const char* ASAP_VERSION_STRING = "1.7.1";

#ifdef __WIN32__
  // stuff specific for MSVC
  #if (defined _MSC_VER) || (defined __GNUC__)
	  #pragma warning( disable : 4251 )
  #endif
#endif

#endif
