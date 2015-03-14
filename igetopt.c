/* GeToPt iNsenSible à la casse modifié d'après le code de Ludvik Jerabek */
/* Toutes les macros, fonctions et variables du getopt et getopt_long standard commencent par i ou I, selon leur nom original */

/* CaSE uNsenSitive GeToPt modified from Ludvik Jerabek's code */
/* All macros, functions and variables from the standard getopt and getopt_long start with i or I, depending and their original name */


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
02/15/2012 - Ludvik Jerabek - Fixed _GETOPT_THROW definition missing in implementation file
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
#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <ctype.h>

#define STATIC_IGETOPT
#include "igetopt.h"
#undef EXPORTS_IGETOPT

#ifdef __cplusplus
	#define _GETOPT_THROW throw()
#else
	#define _GETOPT_THROW
#endif

int ioptind = 1;
int iopterr = 1;
int ioptopt = '?';
enum ENUM_ORDERING { REQUIRE_ORDER, PERMUTE, RETURN_IN_ORDER };

//
//
//		Ansi structures and functions follow
// 
//

static struct _igetopt_data_a
{
	int ioptind;
	int iopterr;
	int ioptopt;
	char *ioptarg;
	int __initialized;
	char *__nextchar;
	enum ENUM_ORDERING __ordering;
	int __posixly_correct;
	int __first_nonopt;
	int __last_nonopt;
} igetopt_data_a;
char *ioptarg_a;


//char *strlower(char *s) { while(*s) *s=tolower(*s++); }

char *strichr(const char *s, int c)
{
  char *ret;
  int uc=toupper(c);
  int lc=tolower(c);
  ret=strchr(s, uc);
  if (!ret) ret=strchr(s, lc);

  return ret;
}

wchar_t *wcsichr(const wchar_t *s, wchar_t c)
{
  wchar_t *ret;
  wchar_t uc=toupper(c);
  wchar_t lc=tolower(c);
  ret=wcschr(s, uc);
  if (!ret) ret=wcschr(s, lc);

  return ret;
}



static void exchange_a(char **argv, struct _igetopt_data_a *d)
{
	int bottom = d->__first_nonopt;
	int middle = d->__last_nonopt;
	int top = d->ioptind;
	char *tem;
	while (top > middle && middle > bottom)
	{
		if (top - middle > middle - bottom)
		{
			int len = middle - bottom;
			register int i;
			for (i = 0; i < len; i++)
			{
				tem = argv[bottom + i];
				argv[bottom + i] = argv[top - (middle - bottom) + i];
				argv[top - (middle - bottom) + i] = tem;
			}
			top -= len;
		}
		else
		{
			int len = top - middle;
			register int i;
			for (i = 0; i < len; i++)
			{
				tem = argv[bottom + i];
				argv[bottom + i] = argv[middle + i];
				argv[middle + i] = tem;
			}
			bottom += len;
		}
	}
	d->__first_nonopt += (d->ioptind - d->__last_nonopt);
	d->__last_nonopt = d->ioptind;
}
static const char *_igetopt_initialize_a (const char *optstring, struct _igetopt_data_a *d, int posixly_correct)
{
	d->__first_nonopt = d->__last_nonopt = d->ioptind;
	d->__nextchar = NULL;
	d->__posixly_correct = posixly_correct | !!getenv("POSIXLY_CORRECT");
	if (optstring[0] == '-')
	{
		d->__ordering = RETURN_IN_ORDER;
		++optstring;
	}
	else if (optstring[0] == '+')
	{
		d->__ordering = REQUIRE_ORDER;
		++optstring;
	}
	else if (d->__posixly_correct)
		d->__ordering = REQUIRE_ORDER;
	else
		d->__ordering = PERMUTE;
	return optstring;
}
int _igetopt_internal_r_a (int argc, char *const *argv, const char *optstring, const struct ioption_a *longopts, int *longind, int long_only, struct _igetopt_data_a *d, int posixly_correct)
{
	int print_errors = d->iopterr;
	if (argc < 1)
		return -1;
	d->ioptarg = NULL;
	if (d->ioptind == 0 || !d->__initialized)
	{
		if (d->ioptind == 0)
			d->ioptind = 1;
		optstring = _igetopt_initialize_a (optstring, d, posixly_correct);
		d->__initialized = 1;
	}
	else if (optstring[0] == '-' || optstring[0] == '+')
		optstring++;
	if (optstring[0] == ':')
		print_errors = 0;
	if (d->__nextchar == NULL || *d->__nextchar == '\0')
	{
		if (d->__last_nonopt > d->ioptind)
			d->__last_nonopt = d->ioptind;
		if (d->__first_nonopt > d->ioptind)
			d->__first_nonopt = d->ioptind;
		if (d->__ordering == PERMUTE)
		{
			if (d->__first_nonopt != d->__last_nonopt && d->__last_nonopt != d->ioptind)
				exchange_a ((char **) argv, d);
			else if (d->__last_nonopt != d->ioptind)
				d->__first_nonopt = d->ioptind;
			while (d->ioptind < argc && (argv[d->ioptind][0] != '-' || argv[d->ioptind][1] == '\0'))
				d->ioptind++;
			d->__last_nonopt = d->ioptind;
		}
		if (d->ioptind != argc && !stricmp(argv[d->ioptind], "--"))
		{
			d->ioptind++;
			if (d->__first_nonopt != d->__last_nonopt && d->__last_nonopt != d->ioptind)
				exchange_a((char **) argv, d);
			else if (d->__first_nonopt == d->__last_nonopt)
				d->__first_nonopt = d->ioptind;
			d->__last_nonopt = argc;
			d->ioptind = argc;
		}
		if (d->ioptind == argc)
		{
			if (d->__first_nonopt != d->__last_nonopt)
				d->ioptind = d->__first_nonopt;
			return -1;
		}
		if ((argv[d->ioptind][0] != '-' || argv[d->ioptind][1] == '\0'))
		{
			if (d->__ordering == REQUIRE_ORDER)
				return -1;
			d->ioptarg = argv[d->ioptind++];
			return 1;
		}
		d->__nextchar = (argv[d->ioptind] + 1 + (longopts != NULL && argv[d->ioptind][1] == '-'));
	}
	if (longopts != NULL && (argv[d->ioptind][1] == '-' || (long_only && (argv[d->ioptind][2] || !strichr(optstring, argv[d->ioptind][1])))))
	{
		char *nameend;
		unsigned int namelen;
		const struct ioption_a *p;
		const struct ioption_a *pfound = NULL;
		struct ioption_list
		{
			const struct ioption_a *p;
			struct ioption_list *next;
		} *ambig_list = NULL;
		int exact = 0;
		int indfound = -1;
		int ioption_index;
		for (nameend = d->__nextchar; *nameend && *nameend != '='; nameend++);
		namelen = (unsigned int)(nameend - d->__nextchar);
		for (p = longopts, ioption_index = 0; p->name; p++, ioption_index++)
			if (!strnicmp(p->name, d->__nextchar, namelen))
			{
				if (namelen == (unsigned int)strlen(p->name))
				{
					pfound = p;
					indfound = ioption_index;
					exact = 1;
					break;
				}
				else if (pfound == NULL)
				{
					pfound = p;
					indfound = ioption_index;
				}
				else if (long_only || pfound->has_arg != p->has_arg || pfound->flag != p->flag || pfound->val != p->val)
				{
					struct ioption_list *newp = (struct ioption_list*)alloca(sizeof(*newp));
					newp->p = p;
					newp->next = ambig_list;
					ambig_list = newp;
				}
			}
			if (ambig_list != NULL && !exact)
			{
				if (print_errors)
				{
					struct ioption_list first;
					first.p = pfound;
					first.next = ambig_list;
					ambig_list = &first;
					fprintf (stderr, "%s: ioption '%s' is ambiguous; possibilities:", argv[0], argv[d->ioptind]);
					do
					{
						fprintf (stderr, " '--%s'", ambig_list->p->name);
						ambig_list = ambig_list->next;
					}
					while (ambig_list != NULL);
					fputc ('\n', stderr);
				}
				d->__nextchar += strlen(d->__nextchar);
				d->ioptind++;
				d->ioptopt = 0;
				return '?';
			}
			if (pfound != NULL)
			{
				ioption_index = indfound;
				d->ioptind++;
				if (*nameend)
				{
					if (pfound->has_arg)
						d->ioptarg = nameend + 1;
					else
					{
						if (print_errors)
						{
							if (argv[d->ioptind - 1][1] == '-')
							{
								fprintf(stderr, "%s: ioption '--%s' doesn't allow an argument\n",argv[0], pfound->name);
							}
							else
							{
								fprintf(stderr, "%s: ioption '%c%s' doesn't allow an argument\n",argv[0], argv[d->ioptind - 1][0],pfound->name);
							}
						}
						d->__nextchar += strlen(d->__nextchar);
						d->ioptopt = pfound->val;
						return '?';
					}
				}
				else if (pfound->has_arg == 1)
				{
					if (d->ioptind < argc)
						d->ioptarg = argv[d->ioptind++];
					else
					{
						if (print_errors)
						{
							fprintf(stderr,"%s: ioption '--%s' requires an argument\n",argv[0], pfound->name);
						}
						d->__nextchar += strlen(d->__nextchar);
						d->ioptopt = pfound->val;
						return optstring[0] == ':' ? ':' : '?';
					}
				}
				d->__nextchar += strlen(d->__nextchar);
				if (longind != NULL)
					*longind = ioption_index;
				if (pfound->flag)
				{
					*(pfound->flag) = pfound->val;
					return 0;
				}
				return pfound->val;
			}
			if (!long_only || argv[d->ioptind][1] == '-' || strichr(optstring, *d->__nextchar) == NULL)
			{
				if (print_errors)
				{
					if (argv[d->ioptind][1] == '-')
					{
						fprintf(stderr, "%s: unrecognized ioption '--%s'\n",argv[0], d->__nextchar);
					}
					else
					{
						fprintf(stderr, "%s: unrecognized ioption '%c%s'\n",argv[0], argv[d->ioptind][0], d->__nextchar);
					}
				}
				d->__nextchar = (char *)"";
				d->ioptind++;
				d->ioptopt = 0;
				return '?';
			}
	}
	{
		char c = *d->__nextchar++;
		char *temp = (char*)strichr(optstring, c);
		if (*d->__nextchar == '\0')
			++d->ioptind;
		if (temp == NULL || c == ':' || c == ';')
		{
			if (print_errors)
			{
				fprintf(stderr, "%s: invalid ioption -- '%c'\n", argv[0], c);
			}
			d->ioptopt = c;
			return '?';
		}
		if (temp[0] == 'W' && temp[1] == ';')
		{
			char *nameend;
			const struct ioption_a *p;
			const struct ioption_a *pfound = NULL;
			int exact = 0;
			int ambig = 0;
			int indfound = 0;
			int ioption_index;
			if (longopts == NULL)
				goto no_longs;
			if (*d->__nextchar != '\0')
			{
				d->ioptarg = d->__nextchar;
				d->ioptind++;
			}
			else if (d->ioptind == argc)
			{
				if (print_errors)
				{
					fprintf(stderr,"%s: ioption requires an argument -- '%c'\n",argv[0], c);
				}
				d->ioptopt = c;
				if (optstring[0] == ':')
					c = ':';
				else
					c = '?';
				return c;
			}
			else
				d->ioptarg = argv[d->ioptind++];
			for (d->__nextchar = nameend = d->ioptarg; *nameend && *nameend != '='; nameend++);
			for (p = longopts, ioption_index = 0; p->name; p++, ioption_index++)
				if (!strnicmp(p->name, d->__nextchar, nameend - d->__nextchar))
				{
					if ((unsigned int) (nameend - d->__nextchar) == strlen(p->name))
					{
						pfound = p;
						indfound = ioption_index;
						exact = 1;
						break;
					}
					else if (pfound == NULL)
					{
						pfound = p;
						indfound = ioption_index;
					}
					else if (long_only || pfound->has_arg != p->has_arg || pfound->flag != p->flag || pfound->val != p->val)
						ambig = 1;
				}
				if (ambig && !exact)
				{
					if (print_errors)
					{
						fprintf(stderr, "%s: ioption '-W %s' is ambiguous\n",argv[0], d->ioptarg);
					}
					d->__nextchar += strlen(d->__nextchar);
					d->ioptind++;
					return '?';
				}
				if (pfound != NULL)
				{
					ioption_index = indfound;
					if (*nameend)
					{
						if (pfound->has_arg)
							d->ioptarg = nameend + 1;
						else
						{
							if (print_errors)
							{
								fprintf(stderr, "%s: ioption '-W %s' doesn't allow an argument\n",argv[0], pfound->name);
							}
							d->__nextchar += strlen(d->__nextchar);
							return '?';
						}
					}
					else if (pfound->has_arg == 1)
					{
						if (d->ioptind < argc)
							d->ioptarg = argv[d->ioptind++];
						else
						{
							if (print_errors)
							{
								fprintf(stderr, "%s: ioption '-W %s' requires an argument\n",argv[0], pfound->name);
							}
							d->__nextchar += strlen(d->__nextchar);
							return optstring[0] == ':' ? ':' : '?';
						}
					}
					else
						d->ioptarg = NULL;
					d->__nextchar += strlen(d->__nextchar);
					if (longind != NULL)
						*longind = ioption_index;
					if (pfound->flag)
					{
						*(pfound->flag) = pfound->val;
						return 0;
					}
					return pfound->val;
				}
no_longs:
				d->__nextchar = NULL;
				return 'W';
		}
		if (temp[1] == ':')
		{
			if (temp[2] == ':')
			{
				if (*d->__nextchar != '\0')
				{
					d->ioptarg = d->__nextchar;
					d->ioptind++;
				}
				else
					d->ioptarg = NULL;
				d->__nextchar = NULL;
			}
			else
			{
				if (*d->__nextchar != '\0')
				{
					d->ioptarg = d->__nextchar;
					d->ioptind++;
				}
				else if (d->ioptind == argc)
				{
					if (print_errors)
					{
						fprintf(stderr,"%s: ioption requires an argument -- '%c'\n",argv[0], c);
					}
					d->ioptopt = c;
					if (optstring[0] == ':')
						c = ':';
					else
						c = '?';
				}
				else
					d->ioptarg = argv[d->ioptind++];
				d->__nextchar = NULL;
			}
		}
		return c;
	}
}
int _igetopt_internal_a (int argc, char *const *argv, const char *optstring, const struct ioption_a *longopts, int *longind, int long_only, int posixly_correct)
{
	int result;
	igetopt_data_a.ioptind = ioptind;
	igetopt_data_a.iopterr = iopterr;
	result = _igetopt_internal_r_a (argc, argv, optstring, longopts,longind, long_only, &igetopt_data_a,posixly_correct);
	ioptind = igetopt_data_a.ioptind;
	ioptarg_a = igetopt_data_a.ioptarg;
	ioptopt = igetopt_data_a.ioptopt;
	return result;
}
int igetopt_a (int argc, char *const *argv, const char *optstring) _GETOPT_THROW
{
	return _igetopt_internal_a (argc, argv, optstring, (const struct ioption_a *) 0, (int *) 0, 0, 0);
}
int igetopt_long_a (int argc, char *const *argv, const char *ioptions, const struct ioption_a *long_ioptions, int *opt_index) _GETOPT_THROW
{
	return _igetopt_internal_a (argc, argv, ioptions, long_ioptions, opt_index, 0, 0);
}
int igetopt_long_only_a (int argc, char *const *argv, const char *ioptions, const struct ioption_a *long_ioptions, int *opt_index) _GETOPT_THROW
{
	return _igetopt_internal_a (argc, argv, ioptions, long_ioptions, opt_index, 1, 0);
}
int _igetopt_long_r_a (int argc, char *const *argv, const char *ioptions, const struct ioption_a *long_ioptions, int *opt_index, struct _igetopt_data_a *d)
{
	return _igetopt_internal_r_a (argc, argv, ioptions, long_ioptions, opt_index,0, d, 0);
}
int _igetopt_long_only_r_a (int argc, char *const *argv, const char *ioptions, const struct ioption_a *long_ioptions, int *opt_index, struct _igetopt_data_a *d)
{
	return _igetopt_internal_r_a (argc, argv, ioptions, long_ioptions, opt_index, 1, d, 0);
}

//
//
//	Unicode Structures and Functions
// 
//

static struct _igetopt_data_w
{
	int ioptind;
	int iopterr;
	int ioptopt;
	wchar_t *ioptarg;
	int __initialized;
	wchar_t *__nextchar;
	enum ENUM_ORDERING __ordering;
	int __posixly_correct;
	int __first_nonopt;
	int __last_nonopt;
} igetopt_data_w;
wchar_t *ioptarg_w;

static void exchange_w(wchar_t **argv, struct _igetopt_data_w *d)
{
	int bottom = d->__first_nonopt;
	int middle = d->__last_nonopt;
	int top = d->ioptind;
	wchar_t *tem;
	while (top > middle && middle > bottom)
	{
		if (top - middle > middle - bottom)
		{
			int len = middle - bottom;
			register int i;
			for (i = 0; i < len; i++)
			{
				tem = argv[bottom + i];
				argv[bottom + i] = argv[top - (middle - bottom) + i];
				argv[top - (middle - bottom) + i] = tem;
			}
			top -= len;
		}
		else
		{
			int len = top - middle;
			register int i;
			for (i = 0; i < len; i++)
			{
				tem = argv[bottom + i];
				argv[bottom + i] = argv[middle + i];
				argv[middle + i] = tem;
			}
			bottom += len;
		}
	}
	d->__first_nonopt += (d->ioptind - d->__last_nonopt);
	d->__last_nonopt = d->ioptind;
}
static const wchar_t *_igetopt_initialize_w (const wchar_t *optstring, struct _igetopt_data_w *d, int posixly_correct)
{
	d->__first_nonopt = d->__last_nonopt = d->ioptind;
	d->__nextchar = NULL;
	d->__posixly_correct = posixly_correct | !!_wgetenv(L"POSIXLY_CORRECT");
	if (optstring[0] == L'-')
	{
		d->__ordering = RETURN_IN_ORDER;
		++optstring;
	}
	else if (optstring[0] == L'+')
	{
		d->__ordering = REQUIRE_ORDER;
		++optstring;
	}
	else if (d->__posixly_correct)
		d->__ordering = REQUIRE_ORDER;
	else
		d->__ordering = PERMUTE;
	return optstring;
}
int _igetopt_internal_r_w (int argc, wchar_t *const *argv, const wchar_t *optstring, const struct ioption_w *longopts, int *longind, int long_only, struct _igetopt_data_w *d, int posixly_correct)
{
	int print_errors = d->iopterr;
	if (argc < 1)
		return -1;
	d->ioptarg = NULL;
	if (d->ioptind == 0 || !d->__initialized)
	{
		if (d->ioptind == 0)
			d->ioptind = 1;
		optstring = _igetopt_initialize_w (optstring, d, posixly_correct);
		d->__initialized = 1;
	}
	else if (optstring[0] == L'-' || optstring[0] == L'+')
		optstring++;
	if (optstring[0] == L':')
		print_errors = 0;
	if (d->__nextchar == NULL || *d->__nextchar == L'\0')
	{
		if (d->__last_nonopt > d->ioptind)
			d->__last_nonopt = d->ioptind;
		if (d->__first_nonopt > d->ioptind)
			d->__first_nonopt = d->ioptind;
		if (d->__ordering == PERMUTE)
		{
			if (d->__first_nonopt != d->__last_nonopt && d->__last_nonopt != d->ioptind)
				exchange_w((wchar_t **) argv, d);
			else if (d->__last_nonopt != d->ioptind)
				d->__first_nonopt = d->ioptind;
			while (d->ioptind < argc && (argv[d->ioptind][0] != L'-' || argv[d->ioptind][1] == L'\0'))
				d->ioptind++;
			d->__last_nonopt = d->ioptind;
		}
		if (d->ioptind != argc && !wcsicmp(argv[d->ioptind], L"--"))
		{
			d->ioptind++;
			if (d->__first_nonopt != d->__last_nonopt && d->__last_nonopt != d->ioptind)
				exchange_w((wchar_t **) argv, d);
			else if (d->__first_nonopt == d->__last_nonopt)
				d->__first_nonopt = d->ioptind;
			d->__last_nonopt = argc;
			d->ioptind = argc;
		}
		if (d->ioptind == argc)
		{
			if (d->__first_nonopt != d->__last_nonopt)
				d->ioptind = d->__first_nonopt;
			return -1;
		}
		if ((argv[d->ioptind][0] != L'-' || argv[d->ioptind][1] == L'\0'))
		{
			if (d->__ordering == REQUIRE_ORDER)
				return -1;
			d->ioptarg = argv[d->ioptind++];
			return 1;
		}
		d->__nextchar = (argv[d->ioptind] + 1 + (longopts != NULL && argv[d->ioptind][1] == L'-'));
	}
	if (longopts != NULL && (argv[d->ioptind][1] == L'-' || (long_only && (argv[d->ioptind][2] || !wcsichr(optstring, argv[d->ioptind][1])))))
	{
		wchar_t *nameend;
		unsigned int namelen;
		const struct ioption_w *p;
		const struct ioption_w *pfound = NULL;
		struct ioption_list
		{
			const struct ioption_w *p;
			struct ioption_list *next;
		} *ambig_list = NULL;
		int exact = 0;
		int indfound = -1;
		int ioption_index;
		for (nameend = d->__nextchar; *nameend && *nameend != L'='; nameend++);
		namelen = (unsigned int)(nameend - d->__nextchar);
		for (p = longopts, ioption_index = 0; p->name; p++, ioption_index++)
			if (!wcsnicmp(p->name, d->__nextchar, namelen))
			{
				if (namelen == (unsigned int)wcslen(p->name))
				{
					pfound = p;
					indfound = ioption_index;
					exact = 1;
					break;
				}
				else if (pfound == NULL)
				{
					pfound = p;
					indfound = ioption_index;
				}
				else if (long_only || pfound->has_arg != p->has_arg || pfound->flag != p->flag || pfound->val != p->val)
				{
					struct ioption_list *newp = (struct ioption_list*)alloca(sizeof(*newp));
					newp->p = p;
					newp->next = ambig_list;
					ambig_list = newp;
				}
			}
			if (ambig_list != NULL && !exact)
			{
				if (print_errors)
				{						
					struct ioption_list first;
					first.p = pfound;
					first.next = ambig_list;
					ambig_list = &first;
					fwprintf(stderr, L"%s: ioption '%s' is ambiguous; possibilities:", argv[0], argv[d->ioptind]);
					do
					{
						fwprintf (stderr, L" '--%s'", ambig_list->p->name);
						ambig_list = ambig_list->next;
					}
					while (ambig_list != NULL);
					fputwc (L'\n', stderr);
				}
				d->__nextchar += wcslen(d->__nextchar);
				d->ioptind++;
				d->ioptopt = 0;
				return L'?';
			}
			if (pfound != NULL)
			{
				ioption_index = indfound;
				d->ioptind++;
				if (*nameend)
				{
					if (pfound->has_arg)
						d->ioptarg = nameend + 1;
					else
					{
						if (print_errors)
						{
							if (argv[d->ioptind - 1][1] == L'-')
							{
								fwprintf(stderr, L"%s: ioption '--%s' doesn't allow an argument\n",argv[0], pfound->name);
							}
							else
							{
								fwprintf(stderr, L"%s: ioption '%c%s' doesn't allow an argument\n",argv[0], argv[d->ioptind - 1][0],pfound->name);
							}
						}
						d->__nextchar += wcslen(d->__nextchar);
						d->ioptopt = pfound->val;
						return L'?';
					}
				}
				else if (pfound->has_arg == 1)
				{
					if (d->ioptind < argc)
						d->ioptarg = argv[d->ioptind++];
					else
					{
						if (print_errors)
						{
							fwprintf(stderr,L"%s: ioption '--%s' requires an argument\n",argv[0], pfound->name);
						}
						d->__nextchar += wcslen(d->__nextchar);
						d->ioptopt = pfound->val;
						return optstring[0] == L':' ? L':' : L'?';
					}
				}
				d->__nextchar += wcslen(d->__nextchar);
				if (longind != NULL)
					*longind = ioption_index;
				if (pfound->flag)
				{
					*(pfound->flag) = pfound->val;
					return 0;
				}
				return pfound->val;
			}
			if (!long_only || argv[d->ioptind][1] == L'-' || wcsichr(optstring, *d->__nextchar) == NULL)
			{
				if (print_errors)
				{
					if (argv[d->ioptind][1] == L'-')
					{
						fwprintf(stderr, L"%s: unrecognized ioption '--%s'\n",argv[0], d->__nextchar);
					}
					else
					{
						fwprintf(stderr, L"%s: unrecognized ioption '%c%s'\n",argv[0], argv[d->ioptind][0], d->__nextchar);
					}
				}
				d->__nextchar = (wchar_t *)L"";
				d->ioptind++;
				d->ioptopt = 0;
				return L'?';
			}
	}
	{
		wchar_t c = *d->__nextchar++;
		wchar_t *temp = (wchar_t*)wcsichr(optstring, c);
		if (*d->__nextchar == L'\0')
			++d->ioptind;
		if (temp == NULL || c == L':' || c == L';')
		{
			if (print_errors)
			{
				fwprintf(stderr, L"%s: invalid ioption -- '%c'\n", argv[0], c);
			}
			d->ioptopt = c;
			return L'?';
		}
		if (temp[0] == L'W' && temp[1] == L';')
		{
			wchar_t *nameend;
			const struct ioption_w *p;
			const struct ioption_w *pfound = NULL;
			int exact = 0;
			int ambig = 0;
			int indfound = 0;
			int ioption_index;
			if (longopts == NULL)
				goto no_longs;
			if (*d->__nextchar != L'\0')
			{
				d->ioptarg = d->__nextchar;
				d->ioptind++;
			}
			else if (d->ioptind == argc)
			{
				if (print_errors)
				{
					fwprintf(stderr,L"%s: ioption requires an argument -- '%c'\n",argv[0], c);
				}
				d->ioptopt = c;
				if (optstring[0] == L':')
					c = L':';
				else
					c = L'?';
				return c;
			}
			else
				d->ioptarg = argv[d->ioptind++];
			for (d->__nextchar = nameend = d->ioptarg; *nameend && *nameend != L'='; nameend++);
			for (p = longopts, ioption_index = 0; p->name; p++, ioption_index++)
				if (!wcsnicmp(p->name, d->__nextchar, nameend - d->__nextchar))
				{
					if ((unsigned int) (nameend - d->__nextchar) == wcslen(p->name))
					{
						pfound = p;
						indfound = ioption_index;
						exact = 1;
						break;
					}
					else if (pfound == NULL)
					{
						pfound = p;
						indfound = ioption_index;
					}
					else if (long_only || pfound->has_arg != p->has_arg || pfound->flag != p->flag || pfound->val != p->val)
						ambig = 1;
				}
				if (ambig && !exact)
				{
					if (print_errors)
					{
						fwprintf(stderr, L"%s: ioption '-W %s' is ambiguous\n",argv[0], d->ioptarg);
					}
					d->__nextchar += wcslen(d->__nextchar);
					d->ioptind++;
					return L'?';
				}
				if (pfound != NULL)
				{
					ioption_index = indfound;
					if (*nameend)
					{
						if (pfound->has_arg)
							d->ioptarg = nameend + 1;
						else
						{
							if (print_errors)
							{
								fwprintf(stderr, L"%s: ioption '-W %s' doesn't allow an argument\n",argv[0], pfound->name);
							}
							d->__nextchar += wcslen(d->__nextchar);
							return L'?';
						}
					}
					else if (pfound->has_arg == 1)
					{
						if (d->ioptind < argc)
							d->ioptarg = argv[d->ioptind++];
						else
						{
							if (print_errors)
							{
								fwprintf(stderr, L"%s: ioption '-W %s' requires an argument\n",argv[0], pfound->name);
							}
							d->__nextchar += wcslen(d->__nextchar);
							return optstring[0] == L':' ? L':' : L'?';
						}
					}
					else
						d->ioptarg = NULL;
					d->__nextchar += wcslen(d->__nextchar);
					if (longind != NULL)
						*longind = ioption_index;
					if (pfound->flag)
					{
						*(pfound->flag) = pfound->val;
						return 0;
					}
					return pfound->val;
				}
no_longs:
				d->__nextchar = NULL;
				return L'W';
		}
		if (temp[1] == L':')
		{
			if (temp[2] == L':')
			{
				if (*d->__nextchar != L'\0')
				{
					d->ioptarg = d->__nextchar;
					d->ioptind++;
				}
				else
					d->ioptarg = NULL;
				d->__nextchar = NULL;
			}
			else
			{
				if (*d->__nextchar != L'\0')
				{
					d->ioptarg = d->__nextchar;
					d->ioptind++;
				}
				else if (d->ioptind == argc)
				{
					if (print_errors)
					{
						fwprintf(stderr,L"%s: ioption requires an argument -- '%c'\n",argv[0], c);
					}
					d->ioptopt = c;
					if (optstring[0] == L':')
						c = L':';
					else
						c = L'?';
				}
				else
					d->ioptarg = argv[d->ioptind++];
				d->__nextchar = NULL;
			}
		}
		return c;
	}
}
int _igetopt_internal_w (int argc, wchar_t *const *argv, const wchar_t *optstring, const struct ioption_w *longopts, int *longind, int long_only, int posixly_correct)
{
	int result;
	igetopt_data_w.ioptind = ioptind;
	igetopt_data_w.iopterr = iopterr;
	result = _igetopt_internal_r_w (argc, argv, optstring, longopts,longind, long_only, &igetopt_data_w,posixly_correct);
	ioptind = igetopt_data_w.ioptind;
	ioptarg_w = igetopt_data_w.ioptarg;
	ioptopt = igetopt_data_w.ioptopt;
	return result;
}
int igetopt_w (int argc, wchar_t *const *argv, const wchar_t *optstring) _GETOPT_THROW
{
	return _igetopt_internal_w (argc, argv, optstring, (const struct ioption_w *) 0, (int *) 0, 0, 0);
}
int igetopt_long_w (int argc, wchar_t *const *argv, const wchar_t *ioptions, const struct ioption_w *long_ioptions, int *opt_index) _GETOPT_THROW
{
	return _igetopt_internal_w (argc, argv, ioptions, long_ioptions, opt_index, 0, 0);
}
int igetopt_long_only_w (int argc, wchar_t *const *argv, const wchar_t *ioptions, const struct ioption_w *long_ioptions, int *opt_index) _GETOPT_THROW
{
	return _igetopt_internal_w (argc, argv, ioptions, long_ioptions, opt_index, 1, 0);
}
int _igetopt_long_r_w (int argc, wchar_t *const *argv, const wchar_t *ioptions, const struct ioption_w *long_ioptions, int *opt_index, struct _igetopt_data_w *d)
{
	return _igetopt_internal_r_w (argc, argv, ioptions, long_ioptions, opt_index,0, d, 0);
}
int _igetopt_long_only_r_w (int argc, wchar_t *const *argv, const wchar_t *ioptions, const struct ioption_w *long_ioptions, int *opt_index, struct _igetopt_data_w *d)
{
	return _igetopt_internal_r_w (argc, argv, ioptions, long_ioptions, opt_index, 1, d, 0);
}
