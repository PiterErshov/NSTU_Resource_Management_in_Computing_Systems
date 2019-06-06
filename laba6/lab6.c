#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>


int main()
{
    int K1[2], K2[2];//задаём массивы для каналов
    pipe(K1);//создаём канал K1
    pipe(K2);//создаём канал K2
    printf("Создаём первый процесс\n");
    if(!fork())
    {
        printf("P1 - Закрываем дескрипторы каналов \n");
        close(K2[0]);//закрываем дескриптор чтения канала K2
        close(K2[1]);//закрываем дескриптор записи канала K2
        close(K1[0]);//закрываем дескриптор чтения канала K1
        printf("P1 - Перенаправляем ввод/вывод \n");
        printf("P1 - Открываем канал K1 на запись\n");        
        printf("P1 - Перенаправляем ввод/вывод\n");
        printf("P1 - ls -lisa \n");
        close(1);//закрываем дескриптор вывода (блокируем вывод результата execl на экран)
        dup(K1[1]);//передаём дескриптор записи канала K1 для передачи результат execl
        execl("/bin/ls", "ls", "-lisa", 0);
        exit(0);//завершаем процесс
    }
    sleep(1);// ждём секунду
    printf("Создаём второй процесс.\n");
    if(!fork())
    {
        printf("P2 - Закрываем дескрипторы каналов \n");
        close(K2[0]);
        close(K1[1]);
        printf("P2 - Перенаправляем ввод/вывод\n");
        close(0);
        printf("P2 - Открываем канал K1 на чтение\n");
        dup(K1[0]);// читаем из канала K1 результат ls -lisa (типа команда | )
        printf("P2 - Открываем канал K2 на запись\n");
        printf("P2 - sort \n");
        close(1);       
        dup(K2[1]);// на запись реззультатов sort в K2
        execl("/usr/bin/sort", "sort", 0);
        exit(0);
    }
    sleep(1);
    printf("Создаём третий процесс.\n");
    if(!fork())
    {
        printf("P3 - Закрываем дескрипторы каналов \n");
        close(K1[0]);
        close(K1[1]);
        close(K2[1]);
        close(0);
        printf("P3 - Открываем канал K2 на чтение\n");
        dup(K2[0]);//читаем результаты sort из K2
        printf("P3 - Открываем файл a.txt на запись\n");                   
        int fd;       
        fd = open("a.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
        printf("P3 - Закрываем дескриптор вывода\n"); 
        printf("P3 - Направляем дескриптор файла для запись\n");   
        printf("P3 - wc -l \n");
        printf("P3 - > a.txt \n");
        close(1);//закрываем дескриптор записи файла fd 
        fcntl(fd, F_DUPFD, 1);// F_DUPFD - дублирует дискриптор записи fd
        execl("/usr/bin/wc", "wc", "-l", 0);
        close(0);
        exit(0);
    }
    wait(0);
    return 0;
} 

