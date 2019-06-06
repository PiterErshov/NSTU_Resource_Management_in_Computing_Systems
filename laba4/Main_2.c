#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
int main()
{
int i, n, pd[2], pid_pi, pid_cos, pd2[2]; //переменные для цикла, параметр и для каналов
	double f, x, cos, Pi; //параметры и результат
	char rpd[10], wpd[10], rpd2[10], wpd2[10]; //чуть-чуть о каналах
	
	//Ввод параметров
	printf("Введите x = ");
	scanf("%lf", &x);
	printf("Введите n = ");
	scanf("%d", &n);
	printf("\n");
		
	//cos: создаем канал для узнаём информацию о канале
	pipe(pd);					
	sprintf(rpd,"%d",pd[0]);	
	sprintf(wpd,"%d",pd[1]);	
	
	//cos: передача параметров
	write(pd[1], &n, sizeof(n));
	write(pd[1], &x, sizeof(x));
	//cos: запуск потока
	pid_cos = fork();
	if (pid_cos == 0)			//cos: потомок процесса
	{
		execl("./cos",rpd,wpd,0);	//вызываем cos.c
		exit(0);
	}
	
	//pi: создаем канал для узнаём информацию о канале
	pipe(pd2);				
	sprintf(rpd2,"%d",pd2[0]);	
	sprintf(wpd2,"%d",pd2[1]);
	
	//pi: передача параметров
	write(pd2[1], &n, sizeof(n));	
	//pi: запуск потока
	pid_pi = fork();
	if (pid_pi == 0) 			//pi: потомок процесса
	{
		execl("./pi",rpd2,wpd2,0);//вызываем pi.c
		exit(0);
	}

	waitpid(pid_pi,NULL,0); 		//ждем поток пи
	
	read(pd2[0], &Pi, sizeof(Pi));	//получаем pi
	printf("Pi after = %lf\n", Pi);	//печатаем pi
	
	waitpid(pid_cos,NULL,0);		//ждем поток косинуса
	read(pd[0], &cos, sizeof(cos));	//получаем cos
	printf("cos(%lf) after = %lf\n", x,cos);	//печатаем cos		

	f = (1.0 - cos)/(Pi*x*x);		//вычисляем функцию		
	
	printf("f(%lf) after = %lf\n", x,f); //печатаем ответ
	return 0;
}
