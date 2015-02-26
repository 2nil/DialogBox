#!/bin/bash

bak () {
  if [ "x$1" = "x1" ]
  then
    echo -e "Sauvegarde $2\t==> $3"
    #/usr/bin/rsync --progress -a --chmod=ugo=rwX --size-only --delete-before $4 $5
    /usr/bin/rsync --progress -a --chmod=ugo=rwX --exclude='*~' --exclude='*.o' $4 $5
  fi
}

# Si un param�tre est pass� alors on sauvegarde tout
if [ $# -eq 1 ]
then
  echo "Sauvegarde compl�te"
else
  # Sinon on ne sauvegarde que ce qui a �t� s�lectionn�
  echo "Sauvegarde des �l�ments coch�s"
  bak "$CHECK00" "Famille"   "D:\Backup" "/cygdrive/e/Famille"   "/cygdrive/d/Backup"
  bak "$CHECK01" "Documents" "D:\Backup" "/cygdrive/e/Documents" "/cygdrive/d/Backup"

  bak "$CHECK02" "Photos" "\\PC-Esther\Famille" "/cygdrive/e/Famille/Photos" "//pc-esther/Famille"
  bak "$CHECK03" "Vid�os" "\\PC-Esther\Famille" "/cygdrive/e/Famille/Vid�os" "//pc-esther/Famille"

  bak "$CHECK04" "Famille"   "G:\Backup" "/cygdrive/e/Famille"   "/cygdrive/g/Backup"
  bak "$CHECK05" "Documents" "G:\Backup" "/cygdrive/e/Documents" "/cygdrive/g/Backup"

  bak "$CHECK06" "Films classiques"   "D:\Backup" "/cygdrive/e/Grabed/Films class�s/Classiques"   "/cygdrive/d/Backup/Films"
  bak "$CHECK07" "Lecture" "D:\Backup" "/cygdrive/e/Grabed/Lecture" "/cygdrive/d/Backup"
  bak "$CHECK07" "Musique" "D:\Backup" "/cygdrive/e/Grabed/Musique" "/cygdrive/d/Backup"
fi

echo -n Sauvegarde termin�e, appuyer sur \<Entr�e\>
read a
