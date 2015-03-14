
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>


// Structure qui permet d'établir la correspondance entre chaine et UINT
struct sUtype {
  UINT uT;
  const char *suT;
} myUt[] = {
  { MB_OK, "OK" },
  { MB_OKCANCEL, "OKCANCEL" },
  { MB_ABORTRETRYIGNORE, "ABORTRETRYIGNORE" },
  { MB_YESNOCANCEL, "YESNOCANCEL" },
  { MB_YESNO, "YESNO" },
  { MB_RETRYCANCEL, "RETRYCANCEL" },

  { MB_ICONHAND, "ICONHAND" },
  { MB_ICONQUESTION, "ICONQUESTION" },
  { MB_ICONEXCLAMATION, "ICONEXCLAMATION" },
  { MB_ICONASTERISK, "ICONASTERISK" },

#if(WINVER >= 0x0400)
  { MB_ICONWARNING, "ICONWARNING" },
  { MB_ICONERROR, "ICONERROR" },
#endif /* WINVER >= 0x0400 */

  { MB_ICONINFORMATION, "ICONINFORMATION" },
  { MB_ICONSTOP, "ICONSTOP" },

  { MB_DEFBUTTON1, "DEFBUTTON1" },
  { MB_DEFBUTTON2, "DEFBUTTON2" },
  { MB_DEFBUTTON3, "DEFBUTTON3" },
#if(WINVER >= 0x0400)
  { MB_DEFBUTTON4, "DEFBUTTON4" },
#endif /* WINVER >= 0x0400 */

  { MB_APPLMODAL, "APPLMODAL" },
  { MB_SYSTEMMODAL, "SYSTEMMODAL" },
  { MB_TASKMODAL, "TASKMODAL" },
#if(WINVER >= 0x0400)
  { MB_HELP, "HELP" },
  { MB_RIGHT, "RIGHT" },
  { MB_RTLREADING, "RTLREADING" },
#endif /* WINVER >= 0x0400 */

//  { MB_NOFOCUS, "NOFOCUS" },
  { MB_SETFOREGROUND, "SETFOREGROUND" },
  { MB_TOPMOST, "TOPMOST" },
  { MB_DEFAULT_DESKTOP_ONLY, "DEFAULT_DESKTOP_ONLY" },
//  { MB_SERVICE_NOTIFICATION, "SERVICE_NOTIFICATION" },

//  { MB_TYPEMASK, "TYPEMASK" },
#if(WINVER >= 0x0400)
//  { MB_USERICON, "USERICON" },
#endif /* WINVER >= 0x0400 */
//  { MB_ICONMASK, "ICONMASK" },
//  { MB_DEFMASK, "DEFMASK" },
//  { MB_MODEMASK, "MODEMASK" },
//  { MB_MISCMASK, "MISCMASK" },
  { (UINT)0, "" }
};

// Etablis la correspondance entre la chaine uType et la valeur uType
UINT str2uT(char *s)
{
  int i=0;
  while (strcmp(myUt[i].suT, "") != 0) {
    // La chaine uTyoe peut-etre en minuscule
    if(_stricmp(s, myUt[i].suT) == 0) return myUt[i].uT;
    i++;
  }

  return 0;
}

// Point d'entrée
// msgbox [text[, caption[, uType[, uType...]]]]
int MsgBox(HWND hwnd, LPSTR lpCmdLine)
{
  // On affiche une aide si necessaire
  if(!lpCmdLine || strcmp(lpCmdLine, "") == 0   || strcmp(lpCmdLine, "?") == 0 ||
     strcmp(lpCmdLine, "/?") == 0 || strcmp(lpCmdLine, "-?") == 0) {
    char msg[1024]="Use :\n msgbox [text[, caption[, type[, type...]]]]\ntype, one of the following :";
    int i=0;
    while(strcmp(myUt[i].suT, "") != 0) {
      strcat(msg, "\n ");
      strcat(msg, myUt[i].suT);
      i++;
    }

    puts(msg);
    MessageBox(hwnd, msg, "Help", MB_OK|MB_ICONINFORMATION);
  } else {
    char text[1024]="", caption[256]="Message";
    UINT uType=0;

    // On balaye les blancs
    LPSTR pLpCmdLine=lpCmdLine;
    while(*pLpCmdLine == ' ') pLpCmdLine++;

    // On recupere le texte
    int i=0;
    for(;;) {
      if (*pLpCmdLine == ',' || *pLpCmdLine == '\0') {
        text[i]='\0';
        if(*pLpCmdLine) pLpCmdLine++;
        break;
      }

      // L'anti-slash sert au retour charriot et à la virgule si on veut l'inclure dans un message
      if(*pLpCmdLine == '\\') {
        if(*(pLpCmdLine+1) == 'n') { text[i]='\n'; pLpCmdLine++; }
        else if(*(pLpCmdLine+1) == ',') { text[i]=','; pLpCmdLine++; }
        else text[i]=*pLpCmdLine;

        i++;
      } else text[i++]=*pLpCmdLine;

      pLpCmdLine++;
    }

    // On balaye les blancs apres la virgule
    while(*pLpCmdLine == ' ') pLpCmdLine++;

    // On recupere le titre
    i=0;
    for(;;) {
      if (*pLpCmdLine == ',' || *pLpCmdLine == '\0') {
        if (i) caption[i]='\0';
        if(*pLpCmdLine) pLpCmdLine++;
        break;
      }

      caption[i++]=*pLpCmdLine++;
    }

    // On balaye les blancs apres la virgule
    while(*pLpCmdLine == ' ') pLpCmdLine++;

    // on recupere les combinaisons de type
    char *suT=strtok(pLpCmdLine, ", ");
    while(suT) {
      uType=uType|str2uT(suT);
      suT=strtok(NULL, ", ");
    }

    // On appel MessageBox avec les parametres recuperes
    int retMB=MessageBox(hwnd, text, caption, uType);

//    sprintf(text, "UType : %d", retMB);
//    MessageBox(NULL, text, "retour", MB_OK|MB_ICONINFORMATION);
    return retMB;
  }

  return 0;
}

