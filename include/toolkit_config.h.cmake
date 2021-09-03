#ifndef TOOLKIT_CONFIG_H
#define TOOLKIT_CONFIG_H

#define TOOLKIT_API_VERSION (@TOOLKIT_VERSION_MAJOR@ << 16) + (@TOOLKIT_VERSION_MINOR@ << 8) + @TOOLKIT_VERSION_PATCH@

/* compatibility defines */
#ifdef _WIN32
#define __func__ __FUNCTION__
#define M_PI 3.1415926535897932384
#endif

#include <QtCore/QtGlobal>
#define TOOLKIT_DLLIMPORT Q_DECL_IMPORT
#define TOOLKIT_DLLEXPORT Q_DECL_EXPORT
#define TOOLKIT_DLLLOCAL

# ifdef TOOLKIT_BUILD_STATIC
// If one is using the library statically, get rid of
// extra information.
#  define TOOLKIT_DLLAPI
#  define TOOLKIT_PLUGIN_DLLAPI
#  define TOOLKIT_LOCAL
# else
// Depending on whether one is building or using the
// library define DLLAPI to import or export.
# ifdef TOOLKIT_COMPILE_LIB
#   define TOOLKIT_DLLAPI TOOLKIT_DLLEXPORT
#   define TOOLKIT_PLUGIN_DLLAPI TOOLKIT_DLLIMPORT
#  elif TOOLKIT_COMPILE_PLUGIN
#   define TOOLKIT_DLLAPI TOOLKIT_DLLIMPORT
#   define TOOLKIT_PLUGIN_DLLAPI TOOLKIT_DLLEXPORT
#  else
#   define TOOLKIT_DLLAPI TOOLKIT_DLLIMPORT
#   define TOOLKIT_PLUGIN_DLLAPI TOOLKIT_DLLIMPORT
#  endif // TOOLKIT_EXPORTS
#  define TOOLKIT_LOCAL TOOLKIT_DLLLOCAL
# endif // TOOLKIT_BUILD_STATIC


#endif
