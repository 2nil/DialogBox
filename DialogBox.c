
#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "UIManage.h"
#include "igetopt.h"

char **GetArgs(int *pa_argc)
{
  int i;
  char **pa_argv;
  size_t l;
 
  LPWSTR *WArgV=CommandLineToArgvW(GetCommandLineW(), pa_argc);

  pa_argv=(char **)malloc(*pa_argc*sizeof(char *));

  for (i=0; i < *pa_argc; i++) {
    l=lstrlenW(WArgV[i]);
    pa_argv[i]=(char *)malloc((l+10)*sizeof(char));
    wsprintf(pa_argv[i], "%ws", WArgV[i]);
  }

  return pa_argv;
}

char ishort_options[] = "akjr:f:po:i:x:y:w:h:t:l:b:c:e:m:u:v:d:s:n::q:";
static struct ioption ilong_options[] = {
 {"alwaysontop",   ino_argument,       0,  'a' },
 {"scroll",        ino_argument,       0,  'k' },
 {"injectvar",     irequired_argument, 0,  'j' },
 {"resizable",     ino_argument,       0,  'r' },
 {"file",          irequired_argument, 0,  'f' },
 {"pollfile",      ino_argument,       0,  'p' },
 {"outfile",       irequired_argument, 0,  'o' },
 {"infile",        irequired_argument, 0,  'i' },
 {"x",             irequired_argument, 0,  'x' },
 {"y",             irequired_argument, 0,  'y' },
 {"w",             irequired_argument, 0,  'w' },
 {"h",             irequired_argument, 0,  'h' },
 {"title",         irequired_argument, 0,  't' },
 {"label",         irequired_argument, 0,  'l' },
 {"button",        irequired_argument, 0,  'b' },
 {"checkbox",      irequired_argument, 0,  'c' },
 {"edit",          irequired_argument, 0,  'e' },
 {"maskededit",    irequired_argument, 0,  'm' },
 {"numericedit",   irequired_argument, 0,  'u' },
 {"verifiededit",  irequired_argument, 0,  'v' },
 {"dropdown",      irequired_argument, 0,  'd' },
 {"space",         ioptional_argument, 0,  's' },
 {"newline",       ioptional_argument, 0,  'n' },
 {"quitaction",    irequired_argument, 0,  'q' },
 /* Reste :
 g : GraphicObject, z : Zbuffered3DObject
 */  
 {0,               0,                 0,   0  }
};

void ParseArgs()
{
  int iopt, i;
  int pa_argc;
  char **pa_argv;

  pa_argv=GetArgs(&pa_argc);

  if (pa_argc == 1) Usage(NULL);
  else {
    int ioption_index = 0;
    int dcmd=1;

    for(;;) {
      int icurind=ioptind;
      iopt=igetopt_long(pa_argc, pa_argv, ishort_options, ilong_options, &ioption_index);
      if (iopt == EOF) break;
      if (!(dcmd=DoCmd(iopt, ioptarg, pa_argv[icurind]))) break;
    }

    if (dcmd) for (i=ioptind ; i < pa_argc; i++) ReadDlgFile(pa_argv[i]);
  }
}

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
  extern int PollFile;
  int ret;
  static int un=1;

  for(;;) {
    InitDlg();

    if (CreateDlg(hInstance)) {
      if (un) {
        ParseArgs();
        un=0;
      } else ReloadDlgFile();

      AddNewLine(NULL);
      ret=RunDlg(nCmdShow);
      if(!PollFile) return ret;
    } else break;
  }

  return 0;
}

