
#ifndef UIMANAGE_H
#define UIMANAGE_H

int DoCmd(char opt, char *oarg, char *long_opt);
void InitDlg();
int CreateDlg(HINSTANCE hInstance);
void AddNewLine(char *hl);
int RunDlg(int nCmdShow);
void ReadDlgFile(char *fn);
void ReloadDlgFile();
void Usage(const char *fmt, ...);

#endif /* UIMANAGE_H */

