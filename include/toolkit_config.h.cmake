#ifndef TOOLKIT_CONFIG_H
#define TOOLKIT_CONFIG_H

#define TOOLKIT_API_VERSION (@TOOLKIT_VERSION_MAJOR@ << 16) + (@TOOLKIT_VERSION_MINOR@ << 8) + @TOOLKIT_VERSION_PATCH@

/* compatibility defines */
#ifdef _WIN32
#define __func__ __FUNCTION__
#define M_PI 3.1415926535897932384
#endif

// Handle portable symbol export.
// Defining manually which symbol should be exported is required
// under Windows whether MinGW or MSVC is used.
//
// The headers then have to be able to work in two different modes:
// - dllexport when one is building the library,
// - dllimport for clients using the library.
//
// On Linux, set the visibility accordingly. If C++ symbol visibility
// is handled by the compiler, see: http://gcc.gnu.org/wiki/Visibility
# if defined WINDOWS_BUILD || defined __CYGWIN__
// On Microsoft Windows, use dllimport and dllexport to tag symbols.
#  include <QCore/QtGlobal>
#  define TOOLKIT_DLLIMPORT Q_DECL_IMPORT
#  define TOOLKIT_DLLEXPORT Q_DECL_EXPORT
#  define TOOLKIT_DLLLOCAL
# else
// On Linux, for GCC >= 4, tag symbols using GCC extension.
#  if __GNUC__ >= 4
#   define TOOLKIT_DLLIMPORT __attribute__ ((visibility("default")))
#   define TOOLKIT_DLLEXPORT __attribute__ ((visibility("default")))
#   define TOOLKIT_DLLLOCAL  __attribute__ ((visibility("hidden")))
#  else
// Otherwise (GCC < 4 or another compiler is used), export everything.
#   define TOOLKIT_DLLIMPORT
#   define TOOLKIT_DLLEXPORT
#   define TOOLKIT_DLLLOCAL
#  endif // __GNUC__ >= 4
# endif // defined _WIN32 || defined __CYGWIN__

# ifdef TOOLKIT_BUILD_STATIC
// If one is using the library statically, get rid of
// extra information.
#  define TOOLKIT_DLLAPI
#  define TOOLKIT_PLUGIN_DLLAPI
#  define TOOLKIT_LOCAL
# else
// Depending on whether one is building or using the
// library define DLLAPI to import or export.
#  ifdef TOOLKIT_COMPILE_LIB
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
