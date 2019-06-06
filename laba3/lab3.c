#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

FILE *fd; // указатель на файл

//----------------TABLE#1--------------------------------------------
struct Tab_f // Структура таблицы
{        
  char name[14];   // имя файла
  int inode;       // числовой индекс
  int point;       // точка
  char access[14]; // режим доступа
};

//----------------------TABLE#2--------------------------------------
struct Tab_opening_file // Структура открытых файлов таблицы
{        
  char name[14];   // имя файла
  int fd;          // числовой дескриптор
  int tf_link;    // ссылка с таблицей файлов
};

//----------------------Main string of TF----------------------------------------------
void str_T_f()
{
  fprintf(fd," TABLE*1\n");
  fprintf(fd,"---------\n");
  fprintf(fd," | file name     number inode     locate point     Access mode |\n");
  return;
}

//-------------------------Main string of TOF-------------------------------------------
void str_T_o_f()
{
  fprintf(fd," TABLE*2\n");
  fprintf(fd,"---------\n");
  fprintf(fd," | file name     Descriptor     link  |\n");
  fprintf(fd," | ---------     ----------     ------- |\n");
  fprintf(fd," | ---------     ----------     ------- |\n");
  fprintf(fd," | ---------     ----------     ------- |\n");

  return;
 }

//--------------------Context of TF------------------------------------------------
void Inform1(struct Tab_f p)
{
  fprintf(fd," | %7s        ", p.name);
  fprintf(fd,"%7d          ", p.inode);
  fprintf(fd,"%7d          ", p.point);
  fprintf(fd," %8s  |", p.access);
  fprintf(fd,"\n");
  return;
}

//----------------------- Context of TOF ---------------------------------------------
void Inform2(struct Tab_opening_file p)
{
  fprintf(fd," | %s          ", p.name);
  fprintf(fd,"     %d           ", p.fd);
  fprintf(fd," %d  |", p.tf_link);
  fprintf(fd,"\n");
  return;
}

//--------------------Main procedure------------------------------------------------
 void main()
{
  struct Tab_f A1,B1;
  struct Tab_opening_file A2,B2;

  fd = fopen("rezultat","a");    //open the file

//---------------------------STEP 1---------------------------------
  fprintf(fd,"\n            Process A open file (test.c)  for reading\n");
  fprintf(fd,"            ======================================\n");
  fprintf(fd,"\n");
  strcpy(A1.name,"test.c");
  strcpy(A1.access,"O_RDONLY");
  A1.inode = 3;
  A1.point = 0;
  str_T_f();
  Inform1(A1);

  fprintf(fd,"\n");
  strcpy(A2.name,"test.c");
  A2.fd = 0;
  A2.tf_link = 0;
  str_T_o_f();
  Inform2(A2);

  fprintf(fd,"\n");
//------------------------------------------------------------------------

  fprintf(fd,"\n             Process B open the file (test.c) for writing\n");
  fprintf(fd,"             =========================================\n");
  fprintf(fd,"\n");
  strcpy(B1.name,"test.c");
  strcpy(B1.access,"O_WRONLY");
  B1.inode = 3;
  B1.point = 0;
  str_T_f();
  Inform1(A1);
  Inform1(B1);

  fprintf(fd,"\n");
  strcpy(B2.name,"test.c");
  B2.fd = 0;
  B2.tf_link = 1;
  str_T_o_f();
  Inform2(B2);

  fprintf(fd,"\n");

//----------------------------STEP 2---------------------------------------
  fprintf(fd,"\n            Process A \n");
  fprintf(fd,"            ===========\n");
  fprintf(fd,"\n");
  strcpy(A1.name,"test.c");
  strcpy(A1.access,"O_RDONLY");
  A1.inode=3;
  A1.point=0;
  str_T_f();
  Inform1(A1);
  Inform1(B1);

  fprintf(fd,"\n");
  strcpy(A2.name,"test.c");
  A2.fd=0;
  A2.tf_link=0;
  str_T_o_f();
  Inform2(A2);

  fprintf(fd,"\n");
//------------------------------------------------------------------------

  fprintf(fd,"\n             Process B \n");
  fprintf(fd,"             ==========\n");
  fprintf(fd,"\n");
  strcpy(B1.name,"test.c");
  strcpy(B1.access,"O_WRONLY");
  B1.inode = 3;
  B1.point = 1;
  str_T_f();
  Inform1(A1);
  Inform1(B1);

  fprintf(fd,"\n");
  strcpy(B2.name,"test.c");
  B2.fd = 0;
  B2.tf_link = 1;
  str_T_o_f();
  Inform2(B2);

  fprintf(fd,"\n");

//---------------------------STEP 3-----------------------------------------
  fprintf(fd,"\n          Close  Process B \n");
  fprintf(fd,"\n            Process A \n");
  fprintf(fd,"            ===========\n");
  fprintf(fd,"\n");
  strcpy(A1.name,"test.c");
  strcpy(A1.access,"O_RDONLY");
  A1.inode = 3;
  A1.point = 1;
  str_T_f();
  Inform1(A1);

  fprintf(fd,"\n");
  strcpy(A2.name,"test.c");
  A2.fd = 0;
  A2.tf_link = 0;
  str_T_o_f();
  Inform2(A2);

  fprintf(fd,"\n");
  fprintf(fd,"\n          Close  Process A \n");     //Close A
//---------------------------STEP 3-----------------------------------------
  str_T_o_f();
  close(fd);
  printf("\n        Tables written to file <rezultat>\n");
  return;
}
