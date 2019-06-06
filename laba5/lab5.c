#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>

// вспомогательные переменные для ожидания события
int got1 = 0, got2 = 0, got3 = 0;

// функции, вызываемые при получении процессом соответствующего сигнала
void sigfunc1(int signum)
{
    got1++;
}
void sigfunc3(int signum)
{
    got3++;
}
    
int main()
{
    int pid1,pid2,pid3,pipe1[2],pipe2[2],pipe0[2];
	int count = 0;// переменная-счетчик
	char data[50];// строка для передачи сообщений и данных

    // программные каналы
	pipe(pipe1);// K1
	pipe(pipe2);// K2
	pipe(pipe0);// K0 канал для синхронизации
    
	if(!fork())	// порождение процесса-потомка P1
    	{	
		if(!(pid3=fork()))	// порождение процесса P3 - потомка P1
		{			
			// P3
	    	pid1 = getppid();//пид предка т.е. P1
	    	pid3 = getpid();//пид P3

	    	printf ("P1: create P3\n");
			strcpy(data,"## From P3 data 1/2 ##");	// подготовка данных для P0 (копирует в массив data строку)
			
			
			printf("P3: send signal to P1\n");
	    	sleep(1);//ожидает 1 секунду

	    	kill(pid1, SIGUSR1);// передача сигнала завершения (SIGUSR1) P1

	    	signal(SIGUSR2, sigfunc3);//выполнение функции sigfunc3 с параметром 0, т. к. SIGUSR2 = 0
						
			printf("P3: .. waiting for signal ..\n");	    

			while(!got3) 
				sleep(1);// ожидание сигнала
	    	
			printf("P3: -- data to K1 --\n");
			write(pipe1[1],&data,sizeof(data));// передаёт данные из data по каналу записи pipe1[1]
			
			printf("P3: !! signal !!\n");
	    	printf("P3: -- data from K2 to K1 --\n"); 
	    	while(count!=2)// считывание из K2 данных и запись их в K1
	    	{
				if(read(pipe2[0],&data,sizeof(data))) // чтение по каналу pipe2[0] данных в data
				{
					count++;
					printf ("P3: ++ data from K2: '%s' ++\n", data);							
					write(pipe1[1],&data,sizeof(data));// запись по каналу pipe1[1] данных из data
				}
			}
		    printf("P3: -- data to K1 --\n");
	    	strcpy(data,"## From P3 data 2/2 ##");// подготовка данных для P0(копирует в массив data строку)
			write(pipe1[1],&data,sizeof(data));// запись по каналу pipe1[1] данных из data
	    	printf("P3: XX end P3 XX\n");
			exit(0);// завершение P3
		}			
		// P1
		printf("P0: create K1,K2,K0\n");
		printf("P0: create P1\n");
		pid1 = getpid();//получение пида P1
		signal(SIGUSR1, sigfunc1);// выполненение функции sigfunc1 с параметром 1, т.к. SIGUSR1 = 1
		printf("P1: .. waiting for signal ..\n");		

		while(!got1) sleep(1);	// ожидание сигнала
			printf("P1: !! signal !!\n");	

		printf("P1: -- data to K2 --\n");
		strcpy(data, "## From P1 data 1/1 ##");	// подготовка данных в K2 (копирует строку в data)
		write(pipe2[1],&data,sizeof(data)); // запись по каналу pipe1[1] данных из data
	
		printf("P1: identifikator P3 to K0\n");
		write(pipe0[1],&pid3,sizeof(pid3));	// передача через K0 идентификатора P3
		printf("P1: .. waiting for end P3 ..\n");		

		wait(0);// ожидание завершения P3	

		printf("P1: !! end P3 !!\n");
		printf("P1: XX end P1 XX\n");
		exit(0);// завершение P1
	}
	else
	{		
		if(!fork())// создание P2
		{			
	    	pid2 = getpid();// получение пида P2
			printf ("P0: create P2\n");
	    	printf("P2: .. look at K0 ..\n");

	    	while(!read(pipe0[0],&pid3,sizeof(int)));// чтение с помощью K0 идентификатора P3
    	    	printf("P2: !! identificator P3 !!\n");

			printf ("P2: -- data to K2 --\n");
	    	strcpy (data,"## From P2 data 1/1 ##");	// подготовка данных в K2
			write(pipe2[1],&data,sizeof(data));	
			printf ("P2: send signal to P3\n");
	    	sleep(1);
	    	kill(pid3,SIGUSR2);	// передача сигнала (завершения) P3
	       	printf ("P2: XX end P2 XX\n");
			exit(0);// завершение P2
		}
		else
		{	
			// P0
			printf ("P0: .. look at K1 ..\n");
		    while(count != 4)	// обработка данных из канала K1
		    {
				if(read(pipe1[0],&data,sizeof(data)))// чтение по каналу pipe1[0] данных в data
				{
			    	printf ("P0: ++ new data: '%s' ++\n", data);
			    	count++;
				}
		    }
		   	printf ("P0: .. waiting for end P1,P2,P3 ..\n");
		    wait(0);// ожидание завершения обоих потомков
		    wait(0);
		    printf ("P0: !! end P1,P2,P3 !!\n");
    		printf ("P0: The end.");
			exit(0);// завершение P0
		}
	}
}
