/* Getopt for Microsoft C
This code is a modification of the Free Software Foundation, Inc.
Getopt library for parsing command line argument the purpose was
to provide a Microsoft Visual C friendly derivative. This code
provides functionality for both Unicode and Multibyte builds.

Date: 02/03/2011 - Ludvik Jerabek - Initial Release
Version: 1.0
Comment: Supports igetopt, igetopt_long, and igetopt_long_only
and POSIXLY_CORRECT environment flag
License: LGPL

Revisions:

02/03/2011 - Ludvik Jerabek - Initial Release
02/20/2011 - Ludvik Jerabek - Fixed compiler warnings at Level 4
07/05/2011 - Ludvik Jerabek - Added no_argument, required_argument, ioptional_argument defs
08/03/2011 - Ludvik Jerabek - Fixed non-argument runtime bug which caused runtime exception
08/09/2011 - Ludvik Jerabek - Added code to export functions for DLL and LIB
02/15/2012 - Ludvik Jerabek - Fixed _IGETOPT_THROW definition missing in implementation file
08/01/2012 - Ludvik Jerabek - Created separate functions for char and wchar_t characters so single dll can do both unicode and ansi
10/15/2012 - Ludvik Jerabek - Modified to match latest GNU features

**DISCLAIMER**
THIS MATERIAL IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING, BUT Not LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE, OR NON-INFRINGEMENT. SOME JURISDICTIONS DO NOT ALLOW THE
EXCLUSION OF IMPLIED WARRANTIES, SO THE ABOVE EXCLUSION MAY NOT
APPLY TO YOU. IN NO EVENT WILL I BE LIABLE TO ANY PARTY FOR ANY
DIRECT, INDIRECT, SPECIAL OR OTHER CONSEQUENTIAL DAMAGES FOR ANY
USE OF THIS MATERIAL INCLUDING, WITHOUT LIMITATION, ANY LOST
PROFITS, BUSINESS INTERRUPTION, LOSS OF PROGRAMS OR OTHER DATA ON
YOUR INFORMATION HANDLING SYSTEM OR OTHERWISE, EVEN If WE ARE
EXPRESSLY ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
*/
#ifndef __IGETOPT_H_
	#define __IGETOPT_H_

	#ifdef _IGETOPT_API
		#undef _IGETOPT_API
	#endif

	#if defined(EXPORTS_IGETOPT) && defined(STATIC_IGETOPT)
		#error "The preprocessor definitions of EXPORTS_IGETOPT and STATIC_IGETOPT can only be used individually"
	#elif defined(STATIC_IGETOPT)
//		#pragma message("Warning static builds of igetopt violate the Lesser GNU Public License")
		#define _IGETOPT_API
	#elif defined(EXPORTS_IGETOPT)
//		#pragma message("Exporting igetopt library")
//		#define _IGETOPT_API __declspec(dllexport)
		#define _IGETOPT_API extern
	#else
//		#pragma message("Importing igetopt library")
		//#define _IGETOPT_API __declspec(dllimport)
		#define _IGETOPT_API extern
	#endif

	// Change behavior for C\C++
	#ifdef __cplusplus
		#define _BEGIN_EXTERN_C extern "C" {
		#define _END_EXTERN_C }
		#define _IGETOPT_THROW throw()
	#else
		#define _BEGIN_EXTERN_C
		#define _END_EXTERN_C
		#define _IGETOPT_THROW
	#endif

	// Standard GNU ioptions
	#define	inull_argument		0	/*Argument Null*/
	#define	ino_argument			0	/*Argument Switch Only*/
	#define irequired_argument	1	/*Argument Required*/
	#define ioptional_argument	2	/*Argument Optional*/	

	// Shorter Options
	#define IARG_NULL	0	/*Argument Null*/
	#define IARG_NONE	0	/*Argument Switch Only*/
	#define IARG_REQ		1	/*Argument Required*/
	#define IARG_OPT		2	/*Argument Optional*/

	#include <string.h>
	#include <wchar.h>

_BEGIN_EXTERN_C

#ifndef STATIC_IGETOPT
	 _IGETOPT_API int ioptind;
	 _IGETOPT_API int iopterr;
	 _IGETOPT_API int ioptopt;
#endif

	// Ansi
	struct ioption_a
	{
		const char* name;
		int has_arg;
		int *flag;
		char val;
	};
#ifndef STATIC_IGETOPT
	 _IGETOPT_API char *ioptarg_a;
#endif
	 _IGETOPT_API int igetopt_a(int argc, char *const *argv, const char *optstring) _IGETOPT_THROW;
	 _IGETOPT_API int igetopt_long_a(int argc, char *const *argv, const char *ioptions, const struct ioption_a *long_ioptions, int *opt_index) _IGETOPT_THROW;
	 _IGETOPT_API int igetopt_long_only_a(int argc, char *const *argv, const char *ioptions, const struct ioption_a *long_ioptions, int *opt_index) _IGETOPT_THROW;

	// Unicode
	struct ioption_w
	{
		const wchar_t* name;
		int has_arg;
		int *flag;
		wchar_t val;
	};
#ifndef STATIC_IGETOPT
	 _IGETOPT_API wchar_t *ioptarg_w;
#endif
	 _IGETOPT_API int igetopt_w(int argc, wchar_t *const *argv, const wchar_t *optstring) _IGETOPT_THROW;
	 _IGETOPT_API int igetopt_long_w(int argc, wchar_t *const *argv, const wchar_t *ioptions, const struct ioption_w *long_ioptions, int *opt_index) _IGETOPT_THROW;
	 _IGETOPT_API int igetopt_long_only_w(int argc, wchar_t *const *argv, const wchar_t *ioptions, const struct ioption_w *long_ioptions, int *iopt_index) _IGETOPT_THROW;	
	
_END_EXTERN_C

	#undef _BEGIN_EXTERN_C
	#undef _END_EXTERN_C
	#undef _IGETOPT_THROW
	#undef _IGETOPT_API

	#ifdef _UNICODE
		#define igetopt igetopt_w
		#define igetopt_long igetopt_long_w
		#define igetopt_long_only igetopt_long_only_w
		#define ioption ioption_w
		#define ioptarg ioptarg_w
	#else
		#define igetopt igetopt_a
		#define igetopt_long igetopt_long_a
		#define igetopt_long_only igetopt_long_only_a
		#define ioption ioption_a
		#define ioptarg ioptarg_a
	#endif
#endif  // __IGETOPT_H_
