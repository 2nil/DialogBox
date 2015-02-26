
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <windows.h>
#include <winuser.h>
#include <shellapi.h>
#include <shlobj.h>
#include <commctrl.h>

#include "igetopt.h"
#define CLASSNAME "DialogBoxProgRootWindowClass"

/* On peut avoir un maximum de 100 objets d'un même type */
#define NOBJECT 100
#define BUTTON_START NOBJECT
#define LABEL_START	2*NOBJECT
#define EDIT_START 3*NOBJECT
#define COMBO_START	4*NOBJECT
#define CHECK_START	5*NOBJECT

char *namedvar[6*NOBJECT];

#define MAX_LINE 512

#define POS_CENTER 1
#define POS_VALUE 2
#define POS_DEFAULT POS_CENTER

static char *AppTitle=NULL;
static HWND g_hwnd;
static HINSTANCE g_hinst;
static int pos_x=POS_DEFAULT, win_x=-1, pos_y=POS_DEFAULT, win_y=-1, win_w=0, win_w_fixed=0, win_h=25, win_h_fixed=0;
static int obj_x=4, obj_y=4, obj_w=0, obj_h=24, last_bottom=0;
static int wmax=0, hmax=0;

static int bcount=0;
static int lcount=0;
static int ecount=0;
static int ccount=0;
static int rcount=0;
int PollFile=0;
static UINT_PTR PollTimer;
static char *PollFileName=NULL;
char *OutFileName=NULL;
char *InFileName=NULL;

int defpb;
char *buttoncb[NOBJECT];
char *QuitAction=NULL;
/* Par défaut la fenêtre de dialogue n'est pas retaillable */
DWORD DlgStyle=WS_DLGFRAME|WS_SYSMENU;
  

/*  Affiche l'erreur windows aussi sur stderr */
void WinError(const char *s)
{
  CHAR *lpMsgBuf;

  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
      NULL, GetLastError(),
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), /* Default language */
      (LPTSTR) &lpMsgBuf, 0, NULL);

  /* Display the string. */
  MessageBox(NULL, s, lpMsgBuf, MB_OK);
  fprintf(stderr, "%s: %s\n", s, (char *)lpMsgBuf); fflush(stderr);
  /* Free the buffer. */
  LocalFree(lpMsgBuf);
}

void InitDlg()
{
  int i;

  for (i=0; i < 6*NOBJECT; i++) {
    if (namedvar[i]) {
      free(namedvar[i]);
      namedvar[i]=NULL;
    }
  }

  DlgStyle=WS_DLGFRAME|WS_SYSMENU;
  pos_x=POS_DEFAULT;
  win_x=-1;
  pos_y=POS_DEFAULT;
  win_y=-1;
  win_w=0;
  win_h=25;
  win_w_fixed=0;
  win_h_fixed=0;
  obj_x=4;
  obj_y=4;
  obj_w=0;
  obj_h=24;
  wmax=4;
  hmax=28;


  for (i=0; i < bcount; i++) if (buttoncb[i]) free(buttoncb[i]);

  bcount=lcount=ecount=ccount=rcount=0;
/*
  PollFile=0;
  if (PollFileName) free(PollFileName);
  PollFileName=NULL;
 */
  if (QuitAction) free(QuitAction);
  QuitAction=NULL;

  if (OutFileName) free(OutFileName);
  OutFileName=NULL;

  if (InFileName) free(InFileName);
  InFileName=NULL;
}

void SetDlgStyle(DWORD style)
{
  if (style != DlgStyle) {
    DlgStyle=style;
    SetWindowLong(g_hwnd, GWL_STYLE, DlgStyle);
  }
}

void SetAlwaysOnTop ()
{
  SetWindowPos(g_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

void SetQuitAction(char *qas)
{
  QuitAction=strdup(qas);
}

void SetObjList(char **objname, char **objval, int starto, int endo, const char *pref, int item_start)
{
  int i, id;
  char s[MAX_LINE];
  HWND hw;

  for (i=starto, id=0; i < endo; i++, id++) {
    if (namedvar[item_start+id]) {
      objname[i]=(char *)malloc((strlen(namedvar[item_start+id])+strlen(pref)+6)*sizeof(char));
      sprintf(objname[i], "%s%02d(%s)", pref, id, namedvar[item_start+id]);
    } else {
      objname[i]=(char *)malloc((strlen(pref)+3)*sizeof(char));
      sprintf(objname[i], "%s%02d", pref, id);
    }
    hw=GetDlgItem(g_hwnd, item_start+id);

    switch(item_start) {
    case EDIT_START:
    case COMBO_START:
      GetWindowText(hw, s, MAX_LINE);
      objval[i]=strdup(s);
      break;
    case BUTTON_START:
      if (SendMessage(hw, BM_GETSTATE, 0, 0))
        objval[i]=strdup("1");
      else objval[i]=strdup("0");
      break;
    case CHECK_START:
      if (IsDlgButtonChecked(g_hwnd, item_start+id)&BST_CHECKED)
        objval[i]=strdup("1");
      else objval[i]=strdup("0");
      break;
    default:
      objval[i]=NULL;
      break;
    }
  }
}

void DelObjList(char **objname, char **objval, int nobj)
{
  int i;

  for(i=0; i < nobj; i++) {
    if (objname[i]) free(objname[i]);
    if (objval[i]) free(objval[i]);
  }

  free(objname);
  free(objval);
}

void GetDlgState(char ***objname, char ***objval, int *nobj)
{
  int io;

  *nobj=bcount+ecount+ccount+rcount;
  *objname=(char **)malloc(*nobj*sizeof(char *));
  *objval=(char **)malloc(*nobj*sizeof(char *));

  io=0;
  SetObjList(*objname, *objval, io, io+bcount, "BUTTON", BUTTON_START);
  io += bcount;
  SetObjList(*objname, *objval, io, io+ecount, "EDIT", EDIT_START);
  io += ecount;
  SetObjList(*objname, *objval, io, io+ccount, "COMBO", COMBO_START);
  io += ccount;
  SetObjList(*objname, *objval, io, io+rcount, "CHECK", CHECK_START);
}

void SetOutFile(char *fn)
{
  if (OutFileName) free(OutFileName);
  OutFileName=strdup(fn);
}

void SetInFile(char *fn)
{
  if (InFileName) free(InFileName);
  InFileName=strdup(fn);
}


void PrintDlgState()
{
  char **objname, **objval;
  int nobj, i;
  FILE *fp;

  GetDlgState(&objname, &objval, &nobj);

  if (OutFileName) {
    if (stricmp(OutFileName, "stdout") == 0) fp=stdout;
    else fp=fopen(OutFileName, "w");
    for(i=0; i < nobj; i++) {
      fprintf(fp, "%s:%s\n", objname[i], objval[i]);

    }
    if (fp!=stdout) fclose(fp);
  }

  DelObjList(objname, objval, nobj);
}

void SetEnvDlgState()
{
  char **objname, **objval, s[MAX_LINE];
  int nobj, i;

  GetDlgState(&objname, &objval, &nobj);

  for(i=0; i < nobj; i++) {
    if (objname && objval) {
      sprintf(s, "%s=%s", objname[i], objval[i]);
      putenv(s);
    }
  }

  DelObjList(objname, objval, nobj);
}

/* Si setvar alors EnvDlgState des objets sinon PrintDlgState */
/* Et Run action après ExpandEnvironment */
void RunAction(char *action, int SetEnv)
{
  if (strncasecmp(action, "[msgbox]", 8) == 0) {
    extern int MsgBox(HWND hwnd, LPSTR lpCmdLine);
    if (strlen(action) > 8) {
      char dst_action[MAX_LINE];
      if (SetEnv) SetEnvDlgState();
      else PrintDlgState();

      ExpandEnvironmentStrings(&action[9], dst_action, MAX_LINE);
      MsgBox(g_hwnd, dst_action);
    } else MsgBox(g_hwnd, NULL);
  } else {
    char dst_action[MAX_LINE];

    if (SetEnv) SetEnvDlgState();
    else PrintDlgState();

    ExpandEnvironmentStrings(action, dst_action, MAX_LINE);
  //  puts(dst_action); fflush(stdout);
    WinExec(dst_action, SW_SHOW);
  }
}

void ConfirmQuit(HWND hwnd)
{
  if (QuitAction) {
    if (strcmp(QuitAction, "[Confirm]") == 0) {
      if (MessageBox(g_hwnd, "Voulez-vous vraiment sortir", AppTitle, MB_YESNO|MB_ICONQUESTION) == IDNO) return;
    } else RunAction(QuitAction, 0);
  }

  PrintDlgState();
  if (PollFile) KillTimer(hwnd, PollTimer);
  PollFile=0;
  DestroyWindow(hwnd);
}


void DoPollFile(HWND hwnd)
{
	struct stat st;

  if (stat(PollFileName, &st) == 0) {
    static time_t prev_mtime=-1;

    if (prev_mtime == -1) prev_mtime=st.st_mtime;

    if (prev_mtime != st.st_mtime) {
      prev_mtime=st.st_mtime;

      if (hwnd) {
        KillTimer(hwnd, PollTimer);
        DestroyWindow(hwnd);
      }
    }
  }
}

int CheckButtonCB(int id)
{
  if (id == IDOK) id=defpb;

  if (id >= BUTTON_START && id < BUTTON_START+NOBJECT) {
    if (stricmp(buttoncb[id-BUTTON_START], "[Exit]") == 0) return 1;
    RunAction(buttoncb[id-BUTTON_START], 1);
  }

  return 0;
}


int GetScroll(HWND hwnd, int bar, UINT code)
{
  SCROLLINFO si={};
  si.cbSize=sizeof(SCROLLINFO);
  si.fMask=SIF_PAGE | SIF_POS | SIF_RANGE | SIF_TRACKPOS;
  GetScrollInfo(hwnd, bar, &si);

  const int minPos=si.nMin;
  const int maxPos=si.nMax - (si.nPage - 1);

  int result=-1;

  switch(code) {
  case SB_LINEUP /*SB_LINELEFT*/:
      result=max(si.nPos - 1, minPos);
      break;
  case SB_LINEDOWN /*SB_LINERIGHT*/:
      result=min(si.nPos + 1, maxPos);
      break;
  case SB_PAGEUP /*SB_PAGELEFT*/:
      result=max(si.nPos - (int)si.nPage, minPos);
      break;
  case SB_PAGEDOWN /*SB_PAGERIGHT*/:
      result=min(si.nPos + (int)si.nPage, maxPos);
      break;
  case SB_THUMBPOSITION:
      // do nothing
      break;
  case SB_THUMBTRACK:
      result=si.nTrackPos;
      break;
  case SB_TOP /*SB_LEFT*/:
      result=minPos;
      break;
  case SB_BOTTOM /*SB_RIGHT*/:
      result=maxPos;
      break;
  case SB_ENDSCROLL:
      // do nothing
      break;
  }

  return result;
}


void ScrollClient(HWND hwnd, int bar, int pos)
{
  static int s_prevx=1;
  static int s_prevy=1;

  int cx=0;
  int cy=0;

  int *delta=(bar == SB_HORZ ? &cx : &cy);
  int *prev=(bar == SB_HORZ ? &s_prevx : &s_prevy);

  *delta=*prev-pos;
  *prev=pos;

  if(cx || cy) ScrollWindow(hwnd, cx, cy, NULL, NULL);
}

void OnScroll(HWND hwnd, int bar, UINT code)
{
  const int scrollPos=GetScroll(hwnd, bar, code);
  if(scrollPos == -1) return;
  SetScrollPos(hwnd, bar, scrollPos, TRUE);
  ScrollClient(hwnd, bar, scrollPos);
}

LONG APIENTRY MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

  switch(uMsg) {
  case WM_CREATE:
    return 0;
  case WM_SHOWWINDOW : {
    int x, y, w, h;

    if (win_h < last_bottom+4) win_h=last_bottom+4;

    if (!win_w_fixed || !win_h_fixed) {
      RECT r;
      GetWindowRect(hwnd, &r);
      POINT p;
      p.x=win_w;
      p.y=win_h;
      ClientToScreen(hwnd, &p);
      w=p.x-r.left+13;
      h=p.y-r.top+1;
    }

    // Taille de fenêtre prédéfini ?
    if (win_w_fixed) w=win_w_fixed;
    if (win_h_fixed) h=win_h_fixed;

    // taille de l'écran
    RECT rc;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
    int ws=rc.right-rc.left;
    int hs=rc.bottom-rc.top;

    // Pour ne pas dépasser la taille de l'écran
    if (w > ws) w=ws;
    if (h > hs) h=hs;

    // Positionnement en X
    if (pos_x == POS_VALUE) x=win_x;
    else if (pos_x == POS_CENTER) x=(ws-w)/2;
    else x=10;

    // Positionnement en Y
    if (pos_y == POS_VALUE) y=win_y;
    else if (pos_y == POS_CENTER) y=(hs-h)/2;
    else y=10;

    MoveWindow(hwnd, x, y, w, h, TRUE);

    // Met en place les ascenseurs
    const SIZE sz={ wmax, hmax };
    SCROLLINFO si={ sizeof(SCROLLINFO), SIF_PAGE|SIF_POS|SIF_RANGE, 1, sz.cx, sz.cx, 1, 0 };
    SetScrollInfo(hwnd, SB_HORZ, &si, FALSE);
    si.nMax=sz.cy; si.nPage=sz.cy;
    SetScrollInfo(hwnd, SB_VERT, &si, FALSE);

    // Démarrage de la scrutation du fichier de dialogue
    if (PollFile) PollTimer=SetTimer(hwnd, 1, 400, NULL);
  } return 0;
  case WM_HSCROLL:
    OnScroll(hwnd, SB_HORZ, (int) LOWORD(wParam));                   
    return 0;
  case WM_VSCROLL:
    OnScroll(hwnd, SB_VERT, (int) LOWORD(wParam));                   
    return 0;
  case WM_SIZE:
    if(wParam != SIZE_RESTORED && wParam != SIZE_MAXIMIZED) return 0;

    SCROLLINFO si;
    si.cbSize=sizeof(SCROLLINFO);
    int bar[2];
    bar[0]=SB_HORZ;
    bar[1]=SB_VERT;
    int page[2];
    page[0]=LOWORD(lParam);
    page[1]=HIWORD(lParam);
    size_t i;

    for(i=0; i < 2; ++i) {
      si.fMask=SIF_PAGE;
      si.nPage=page[i];
      SetScrollInfo(hwnd, bar[i], &si, TRUE);
      si.fMask=SIF_RANGE | SIF_POS;
      GetScrollInfo(hwnd, bar[i], &si);
      const int maxScrollPos=si.nMax - (page[i] - 1);
      const BOOL needToScroll=(si.nPos != si.nMin && si.nPos == maxScrollPos) || (wParam == SIZE_MAXIMIZED);
      if (needToScroll) ScrollClient(hwnd, bar[i], si.nPos);
    }
    return 0;
  case WM_TIMER:
    if (PollFile) DoPollFile(hwnd);
    return 0;
  case WM_QUIT:
  case WM_CLOSE:
    ConfirmQuit(hwnd);
    return 0;
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
  case WM_COMMAND:
    switch (wParam) {
    case IDOK:
      if (CheckButtonCB(IDOK)) ConfirmQuit(hwnd);
      return 0;
    case IDCANCEL:
      ConfirmQuit(hwnd);
      return 0;
    default :
      if (CheckButtonCB((int)wParam)) ConfirmQuit(hwnd);
      return 0;
    }
    break;
  case WM_SYSCOMMAND:
    switch (wParam) {
    case SC_CLOSE:
      ConfirmQuit(hwnd);
      return 0;
    }
    break;
  }

  return (LONG)DefWindowProc(hwnd, uMsg, wParam, lParam);
}


int CreateDlg(HINSTANCE hInstance)
{
  static int une=1;

  if (une) {
    WNDCLASSEX wc;
    INITCOMMONCONTROLSEX icc;
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&icc);

    g_hinst=hInstance;
    wc.cbSize=sizeof(WNDCLASSEX);
    wc.style=CS_HREDRAW|CS_VREDRAW;
    wc.lpfnWndProc=(WNDPROC)MainWndProc;
    wc.cbClsExtra=0;
    wc.cbWndExtra=DLGWINDOWEXTRA;
    wc.hInstance=hInstance;
    wc.hIcon=LoadIcon(hInstance, MAKEINTRESOURCE(101));
    wc.hCursor=LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground=CreateSolidBrush(0xF0F0F0);
    wc.lpszMenuName=NULL;
    wc.lpszClassName=CLASSNAME;

    wc.hIconSm=LoadIcon(hInstance, MAKEINTRESOURCE(101));

    /*  Register Main Window Class */
    if (!RegisterClassEx(&wc)) {
      WinError("RegisterClassEx");
      return 0;
    }

    une=0;
  }

    
  /*  Create Main Window */
  g_hwnd=CreateWindowEx(0, CLASSNAME, AppTitle, WS_VSCROLL | WS_HSCROLL | DlgStyle,
    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
    (HWND)NULL, (HMENU)NULL, hInstance, (LPVOID)NULL);

  if (!g_hwnd) {
    WinError("CreateWindowEx");
    return 0;
  }

  return 1;
}


int RunDlg(int nCmdShow)
{
  MSG msg;

  /*  Show and update the application */
  ShowWindow(g_hwnd, nCmdShow);
  UpdateWindow(g_hwnd);

  /*  Traitement des messages */
  while (GetMessage(&msg, NULL, 0, 0)) {
  if ((IsWindow(g_hwnd) && !IsDialogMessage(g_hwnd, &msg))) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  return (int)msg.wParam;
}

void SetWinX(char *x)
{
  if (*x == 'c') {
    pos_x=POS_CENTER;
  } else {
    pos_x=POS_VALUE;
    win_x=atoi(x);
  }
}

void SetWinY(char *y)
{
  if (*y == 'c') {
    pos_y=POS_CENTER;
  } else {
    pos_y=POS_VALUE;
    win_y=atoi(y);
  }
}

void SetWidth(char *w)
{
  win_w_fixed=atoi(w);
}

void SetHeight(char *h)
{
  win_h_fixed=atoi(h);
}

void SetTitle(const char *tstr)
{
  if (AppTitle) free(AppTitle);
  AppTitle=strdup(tstr);
  SetWindowText(g_hwnd, AppTitle);
}  

#define DEFAULT 0
#define EDIT 0
#define COMBO 1
#define CHECK 2
#define BUTTON 3
#define STATIC 4

void Resize(HWND hw, const char *s, int t)
{
  RECT r={0,0,0,0};
  int w, h;

  switch(t) {
  case 0: /* DEFAULT */
    DrawText(GetDC(hw), s, -1, &r, DT_CALCRECT|DT_EDITCONTROL);
    w=r.right-r.left;
    h=r.bottom-r.top;
    MoveWindow(hw, obj_x, obj_y+4, 4*w/5, h+2, TRUE);
    break;
  case 1: /* COMBO */
    DrawText(GetDC(hw), s, -1, &r, DT_CALCRECT|DT_EDITCONTROL);
    w=r.right-r.left;
    h=r.bottom-r.top;
    MoveWindow(hw, obj_x, obj_y+2, w, h, TRUE);
    break;
  case 2: /* CHECK */
    DrawText(GetDC(hw), s, -1, &r, DT_CALCRECT|DT_EDITCONTROL);
    w=r.right-r.left;
    h=r.bottom-r.top;
    MoveWindow(hw, obj_x, obj_y+3, w, h+2, TRUE);
    break;
  case 3: /* BUTTON */
    DrawText(GetDC(hw), s, -1, &r, DT_CALCRECT|DT_EDITCONTROL);
    w=r.right-r.left;
    h=r.bottom-r.top;
    MoveWindow(hw, obj_x, obj_y, w, h+12, TRUE);
    break;
  case 4: /* STATIC */
    DrawText(GetDC(hw), s, -1, &r, DT_CALCRECT);
    w=r.right-r.left;
    h=r.bottom-r.top;
    MoveWindow(hw, obj_x, obj_y+6, w, h+2, TRUE);
    break;
   }

//  printf("av s %s, obj_x %d, w %d, win_w %d\n", s, obj_x, w, win_w);
//  printf("av s %s\n obj_y %d, h %d, win_h %d\n", s, obj_y, h, win_h);
  if (win_w < obj_x+w) win_w=obj_x+w;
  if (win_h < obj_y+h) win_h=obj_y+h;

  if (wmax < obj_x+w) wmax=obj_x+w;
  if (hmax < obj_y+h) hmax=obj_y+h;

//  printf("ap win_w %d\n", win_w);
//  printf("ap win_h %d\n", win_h);
  obj_x += w+4;
  if (last_bottom < obj_y+h) last_bottom=obj_y+h;
//  obj_y += h+4;
}

char *replace_underscore(char *s)
{
  static char *ls=NULL, *ps, *pe;

  if (ls) free(ls);
  ls=strdup(s);

  ps=ls;
  while (*ps) {
    if (*ps == '_') ps++;
    else break;
  }

  pe=&ps[strlen(ps)-1];
  while (pe > ps) {
    if (*pe == '_') { *pe='\0'; pe--; }
    else break;
  }

  return ps;
}

char *strip_antislash(char *s)
{
  return s;
  static char *ls=NULL, *ps;

  if (ls) free(ls);
  ps=ls=(char *)malloc(strlen(s)+1);

  while (*s) {
    if (*s == '\\') s++;
    else *ps++=*s++;
  }

  *ps=*s; /* Le '\0' */

  return ls;
}

HWND AddObject(DWORD ExStyle, const char *ClassName, char *WindowName, DWORD Style, int id)
{
  HWND hw;
  char *wn;

  wn=strip_antislash(replace_underscore(WindowName));

  /* If an object's has no value but its name can be found in the environment, it is assigned its value */
  if (!wn || strcmp(wn, "") == 0) {
    wn=getenv(namedvar[id]);
  }

  if (strcmp(ClassName, "BUTTON") == 0 && strcmp(wn, "OK") == 0) { defpb=id; Style |= BS_DEFPUSHBUTTON; }

  if (strcmp(ClassName, "EDIT") == 0) { /*ExStyle=WS_EX_CLIENTEDGE; } */Style |= ES_MULTILINE; }

  hw=CreateWindowEx(ExStyle, ClassName, wn, Style | WS_VISIBLE | WS_CHILD,
   obj_x, obj_y, obj_w, obj_h,
   g_hwnd,
   (HMENU)id,
   g_hinst,
   (LPVOID)0);

  SendMessage(hw, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), 0);
  //printf("wmax %d / obj_xw = %d, hmax = %d / obj_yh = %d\n", wmax, obj_x+obj_w, hmax, obj_y+obj_h);
  if (wmax < obj_x+obj_w) wmax=obj_x+obj_w;
  if (hmax < obj_y+obj_h) hmax=obj_y+obj_h;

  return hw;
}


/* A variable name can be any string of 2 or more character starting with $ */
char *SetVarName(char *s, int idx)
{
  static char *exp=NULL;
  int i=0, ne;

  /* Isole et stocke le nom d'objet, si besoin */
  if (s[0] == '$') {
    i++;
    while (s[i] && !isspace(s[i])) i++;
    namedvar[idx]=(char *)malloc(i);
    strncpy(namedvar[idx], &s[1], i-1);
    namedvar[idx][i-1]='\0';
//    printf("nv %s\n", namedvar[idx]); fflush(stdout);
  }

  /* Enléve les espaces ainsi que les guillements ou apostrophes en trop */
  while (isspace(s[i])) i++;
  if (s[i] == '"' || s[i] == '\'') i++;
  s=&s[i];

  i=strlen(s);
  if (s[i-1] == '"' || s[i-1] == '\'') s[i-1]='\0';

  /* Développe les variable d'env qui pourrait servir */
  ne=ExpandEnvironmentStrings(s, exp, 0);
  if (exp) free(exp);
  exp=(char *)malloc(ne);

  ExpandEnvironmentStrings(s, exp, ne);

  return exp;
}

void AddButton(char *bs)
{
  char *bstr, *blabel, *baction;
  HWND hw;

  bstr=SetVarName(bs, BUTTON_START+bcount);

  blabel=strdup(bstr);
  baction=strchr(blabel, ',');

  if (baction) {
    *baction='\0';
    baction++;
  } else baction=strdup("[Exit]");

  while (*baction && isspace(*baction)) baction++;

  buttoncb[bcount]=strdup(baction);
  hw=AddObject(0, "BUTTON", blabel, WS_TABSTOP, BUTTON_START+bcount);
  Resize(hw, blabel, BUTTON);
  bcount++;
}

void AddLabel(char *ls)
{
  HWND hw;
  char *lstr;

  lstr=SetVarName(ls, LABEL_START+lcount);
  hw=AddObject(0, "STATIC", lstr, 0, LABEL_START+lcount);
  Resize(hw, lstr, STATIC);
  lcount++;
}


void AddEdit(char *es)
{
  HWND hw;
  char *estr;

  estr=SetVarName(es, EDIT_START+ecount);
  hw=AddObject(WS_EX_CLIENTEDGE, "EDIT", estr, WS_TABSTOP|ES_AUTOHSCROLL|ES_AUTOVSCROLL|ES_MULTILINE|ES_WANTRETURN, EDIT_START+ecount);
  Resize(hw, estr, EDIT);
  ecount++;
}

void MaskedEdit(char *es)
{
  HWND hw;
  char *estr;

  estr=SetVarName(es, EDIT_START+ecount);
  hw=AddObject(WS_EX_CLIENTEDGE, "EDIT", estr, ES_PASSWORD|WS_TABSTOP|ES_AUTOHSCROLL, EDIT_START+ecount);
  Resize(hw, estr, EDIT);
  ecount++;
}

void AddDropDown(char *cs)
{
  HWND hw;
  char cbn[]="CB0", *tok, bigt[256]="";
  char *cstr;

  cstr=SetVarName(cs, COMBO_START+ccount);
  cbn[2] += (char)ccount;
  hw=AddObject(0, "COMBOBOX", cbn, CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, COMBO_START+ccount);
  tok=strtok(cstr, ",\n");
  while (tok) {
    if (strlen(tok) > strlen(bigt)) strcpy(bigt, tok);
    SendMessage(hw, CB_ADDSTRING, 0, (LPARAM)strip_antislash(replace_underscore(tok)));
    tok=strtok(NULL, ",\n");
  }

  Resize(hw, bigt, COMBO);
  SendMessage(hw, CB_SETCURSEL, 0, 0);

  ccount++;
}

void AddCheck(char *rs)
{
  HWND hw;
  char *rstr;
  int checked=0;
  char en[]="CHECK00", *ev;

//  if ((*rs == 'x' || *rs == 'X') && rs[1] == ' ') { rs += 2; checked=1; }
  en[6]='0'+rcount;
  ev=getenv(en);
  if (ev && *ev == '1') checked=1;

  rstr=SetVarName(rs, CHECK_START+rcount);

  hw=AddObject(0, "BUTTON", rstr, BS_AUTOCHECKBOX | WS_TABSTOP, CHECK_START+rcount);
  if (checked) CheckDlgButton(g_hwnd, CHECK_START+rcount, BST_CHECKED);
  Resize(hw, rstr, CHECK);
  rcount++;
}


void AddSpace(char *nsp)
{
  RECT r;
  GetWindowRect(g_hwnd, &r);
  POINT p;
  p.x=atoi(nsp);
  ScreenToClient(g_hwnd, &p);
  p.x += r.left;
  obj_x += p.x;
  if (wmax < obj_x+obj_w) wmax=obj_x+obj_w;
}

void AddNewLine(char *hnl)
{
  int hl=32;

  if (hnl && strcmp(hnl, "") != 0) hl=atoi(hnl);

  obj_x=4;
  obj_y += hl;

  if (hl > 0) win_h=obj_y+hl+4;
  if (hmax < obj_y+obj_h) hmax=obj_y+obj_h;
}


void SetPolling()
{
  PollFile=1;
}


void ReadDlgFile(char *fn);

void AddMultiLineLabel(const char *s)
{
  char *l=strdup(s), *p=l;

  while (*p) {
    if (*p == '\n') {
      *p++='\0';
      AddLabel(l);
      AddNewLine(NULL);
      AddNewLine(NULL);
      l=p;
    }
    p++;
  }

  if (*l) AddLabel(l);
}

void Usage(const char *fmt, ...)
{
  char s[]="\
a : Fenêtre toujours au-dessus.\n\
r : Fenêtre du dialogue, retaillable ou pas.\n\
f nom_fichier : Lit le fichier de description de dialogue, indiqué.\n\
p : Scrute les changement sur le fichier de description de dialogue et met à jour en fonction.\n\
o : Envoie la valeur des objets dans un fichier en sortie du dialogue.\n\
x valnum : Fixe les coordonnée en X du dialogue.\n\
y valnum : Fixe les coordonnée en Y du dialogue.\n\
w valnum : Fixe la largeur du dialogue.\n\
h valnum : Fixe la hauteur du dialogue.\n\
t titre : Modifie le titre du dialogue.\n\
b label_bouton, action_bouton : Ajoute un bouton et l'action qui lui correspond (séparés par une virgule).\n\
c label_check : Ajoute un check box.\n\
l label : Ajoute un label.\n\
e valtext : Ajoute une zone de saisie texte ('_' à un rôle particulier).\n\
m valtext : Ajoute une zone de saisie masqué (mot de passe)\n\
d valtext[,valtext, ...] : Ajoute une drop list (combo).\n\
s valnum : Décale de n pixels vers la droite.\n\
n : Passe à la ligne suivante.\n\
q action : Modifie l'action de sortie du dialogue, le mot clef [Confirm] demande confirmation avant de sortir.";


  SetDlgStyle(WS_OVERLAPPEDWINDOW);
  SetTitle("Aide DialogBox");

  if (fmt) {
    char ls[1024];
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(ls, 1024, fmt, ap);
    va_end(ap);

    AddLabel(ls); AddNewLine(NULL);
    puts(ls);
  }

  AddLabel(s);
  puts(s); fflush(stdout);
}

int DoCmd(char iopt, char *oarg, char *ilong_opt)
{
  switch (iopt) {
  case 'a': case 'A': SetAlwaysOnTop(); break;
  case 'r': case 'R': SetDlgStyle(WS_OVERLAPPEDWINDOW);	break;
  case 'f': case 'F': ReadDlgFile(oarg); break;
  case 'p': case 'P': SetPolling(); break;
  case 'o': case 'O': SetOutFile(oarg); break;
  case 'j': case 'J': putenv(oarg); break;
  case 'i': case 'I': SetInFile(oarg); break;
  case 'x': case 'X': SetWinX(oarg); break;
  case 'y': case 'Y': SetWinY(oarg); break;
  case 'w': case 'W': SetWidth(oarg); break;
  case 'h': case 'H': SetHeight(oarg); break;
  case 't': case 'T': SetTitle(oarg); break;
  case 'l': case 'L': AddLabel(oarg); break;
  case 'b': case 'B': AddButton(oarg); break;
  case 'c': case 'C': AddCheck(oarg); break;
  case 'e': case 'E': AddEdit(oarg); break;
  case 'm': case 'M': MaskedEdit(oarg); break;
  case 'd': case 'D': AddDropDown(oarg); break;
  case 's': case 'S': AddSpace(oarg); break;
  case 'n': case 'N': AddNewLine(oarg); break;
  case 'q': case 'Q': SetQuitAction(oarg); break;
  case '#': break;                              // Commentaire
  case '?':                                     // Aide
  default :
    if (ilong_opt) {
      if (ilong_opt[0] == '-') {
        if (ilong_opt[1] == '-') Usage("Unknown long option \"%s\" ", ilong_opt);
        else Usage("Unknown short option \"%s\" ", ilong_opt);
      }
      else Usage("Unknown command \"%s\" ", ilong_opt);
    } else if (ioptopt) Usage("Unknown short option (2) \"-%c\" ", ioptopt);
    else Usage(NULL);

    return 0;
  }

  return 1;
}

void ReadDlgFile(char *fn)
{
  char *respfn;
  FILE *fp=fopen(fn, "r");

  if (fp) {
    char line[MAX_LINE], *p;

    for (;;) {
      if (fgets(line, MAX_LINE, fp) == NULL) break;
      if (line[strlen(line)-1] == '\n') line[strlen(line)-1]='\0';

      if (line[0]) {
        /* Enléve les caracteres, non nul et non espace */
        p=&line[1];
        while (*p && !isspace(*p)) p++;
        /* Enleve les caracteres espace */
        while (*p && isspace(*p)) p++;
        if (!DoCmd(line[0], p, line)) break;
      }
    }

    fclose(fp);

    /* Pour éviter l'étreinte mortelle entre le free de PollFileName et le strdup de fn */
    respfn=strdup(fn);
    free(PollFileName);
    PollFileName=respfn;
  }
}

void ReloadDlgFile()
{
  ReadDlgFile(PollFileName);
}


