
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

// Для запроса сообщений
#include <sys/ipc.h>
#include <sys/msg.h>

//Для семафоров
#include <sys/sem.h> 
#include <errno.h>

using namespace std;

typedef union semun
{
	int val;                 /* value for SETVAL */
	struct semid_ds *buf;	/* buffer for IPC_STAT, IPC_SET */
	short *array;			/* array for GETALL, SETALL */
} semun;


struct msg
{
	long mtype;      /* тип сообщения */
	char mtext[64];  /* текст сообщения (размер задается какой хочешь) */
};


// Функция инициализации очереди
int initmsg(key_t msgkey)
{
	int msgid, status = 0;

	msgid = msgget(msgkey, 0666 | IPC_CREAT | IPC_EXCL);
	if (msgid == -1)
	{
		// Если очередь существовала ранее
		if (errno == EEXIST)               
			msgid = msgget(msgkey, 0);	// то открыть ее 
	}
	if (msgid == -1)
		return (-1);

	return (msgid);
}

// Функция инициализации семафора
int initsem(key_t semkey, int num, union semun arg)
{
	int semid, status = 0;

	semid = semget(semkey, num, 0600 | IPC_CREAT | IPC_EXCL);
	if (semid == -1)
	{
		// Если набор семафоров существовал ранее
		if (errno == EEXIST)  
			semid = semget(semkey, num, 0);    // то открыть его
	}
	if (semid == -1)
		return(-1);

	return (semid);
}


// Функция изменения значения семафора
int setSem(int semid, int num, int value)
{
	struct sembuf p_buf;
	p_buf.sem_num = num;
	p_buf.sem_op = value;
	p_buf.sem_flg = 0;
	//cout << "Она тебя сожрёт" << endl;
	if (semop(semid, &p_buf, 1) == -1)
	{
		cout << "Error in p " << semop(semid, &p_buf, 1) << endl;
		exit(1);
	}

	return (0);
}



int main()
{
	pid_t pid_p1, pid_p2, pid_p3, pid_p4; 	// Идентификаторы процессов
	key_t semkey = 0X200, msgkey = 0X201;	// Ключи для IPC вызова
	union semun arg;

	int number, status, msgid, semid;
	struct msqid_ds msq_stat;
	
	msg qm_p1, qm_p2, qm_p3, qm_p4, rcvd;	// Для отправки / доставки сообщений

	const char* poem[] = {
	  "1",
	  "2",
	  "3",
	  "4",
	  "5",
	  "6",
	  "7",
	  "8",
	  "9",
	  "10",
	  "11",
	  "12",
	};

	// Инициализация семафора
	semid = initsem(semkey, 4, arg);
	if (semid < 0)
	{
		cout << " Error semid = " << semid << " errno = " << strerror(errno) << endl;
		//exit(1);
	}
	cout << "Successfully semid = " << semid << endl;

	// Инициализация очереди
	msgid = initmsg(msgkey);
	if (msgid < 0)
	{
		cout << "Error msgid = " << msgid << " errno = " << strerror(errno) << endl;
		//return (-1);
	}
	cout << "Successfully msgid = " << msgid << endl;

	// Установка базовых семафорных чисел
	for (int i = 0; i < 4; i++)
	{
		arg.val = 0;	// Установить 0 в качестве основы
		status = semctl(semid, i, SETVAL, arg);
		cout << "sem = " << i << "\t status = " << status << "\t errno = " << strerror(errno) << endl;
	}

	// Вычисление количества строк в стихотворении
	int max = sizeof(poem) / sizeof(*poem);

	// Размер сообщения
	const int qm_len = sizeof(msg); 

	// Создание процесса P1
	pid_p1 = fork();
	if (pid_p1 == 0)
	{
		cout << endl << "Fork P1 created" << endl << endl;
		//*
		qm_p1.mtype = 1;
		for (int i = 0; i < max; i += 4)
		{
			
			setSem(semid, 0, -1);	
			cout << "P1" << endl;			// Блокировка до установки семафора
			sprintf(qm_p1.mtext, "%s", poem[i]);	// Установка строки из стихотворения как сообщения
			//cout << endl << i << "\t" << poem[i] << endl;

			// Отправка сообщения
			if (msgsnd(msgid, &qm_p1, qm_len, 0) == -1)
			{
				cout << "Error of send message in P1!" << endl;
				return(-1);
			}
		}
		//*/
		exit(0);
	}
	else
	{
		if (pid_p1 == -1)
			cout << "Unable to create fork P1 " << pid_p1 << endl;
	}
	//wait(0);
	sleep(1);
	// Создание процесса P2
	pid_p2 = fork();
	if (pid_p2 == 0)
	{
		cout << endl << "Fork P2 created" << endl << endl;
		//*
		qm_p1.mtype = 1;
		for (int i = 1; i < max; i += 4)
		{
			setSem(semid, 1, -1);	
			cout << "P2" << endl;			// Блокировка до установки семафора
			sprintf(qm_p1.mtext, "%s", poem[i]);	// Установка строки из стихотворения как сообщения
			//cout << endl << i << "\t" << qm_p1.mtext << endl;

			// Отправка сообщения
			if (msgsnd(msgid, &qm_p1, qm_len, 0) == -1)
			{
				cout << "Error of send message in P2!" << endl;
				return(-1);
			}
		}
		//*/
		exit(0);
	}
	else
	{
		if (pid_p2 == -1)
			cout << "Unable to create fork P2 " << pid_p2 << endl;
	}
	//wait(0);
	sleep(1);
	// Создание процесса P3
	pid_p3 = fork();
	if (pid_p3 == 0)
	{
		cout << endl << "Fork P3 created" << endl << endl;
		//*
		qm_p1.mtype = 1;
		for (int i = 2; i < max; i += 4)
		{
			setSem(semid, 2, -1);					// Блокировка до установки семафора
			cout << "P3" << endl;
			sprintf(qm_p1.mtext, "%s", poem[i]);	// Установка строки из стихотворения как сообщения
			//cout << endl << i << "\t" << qm_p1.mtext << endl;

			// Отправка сообщения
			if (msgsnd(msgid, &qm_p1, qm_len, 0) == -1)
			{
				cout << "Error of send message in P3!" << endl;
				return(-1);
			}
		}
		//*/
		exit(0);
	}
	else
	{
		if (pid_p3 == -1)
			cout << "Unable to create fork P3 " << pid_p3 << endl;
	}
	//wait(0);
	sleep(1);
	// Создание процесса P4
	pid_p4 = fork();
	if (pid_p4 == 0)
	{
		cout << endl << "Fork P4 created" << endl << endl;
		//*
		qm_p1.mtype = 1;
		for (int i = 3; i < max; i += 4)
		{
			setSem(semid, 3, -1);					// Блокировка до установки семафора
			cout << "P4" << endl;
			sprintf(qm_p1.mtext, "%s", poem[i]);	// Установка строки из стихотворения как сообщения
			//cout << endl << i << "\t" << qm_p1.mtext << endl;

			// Отправка сообщения
			if (msgsnd(msgid, &qm_p1, qm_len, 0) == -1)
			{
				cout << "Error of send message in P4!" << endl;
				return(-1);
			}
		}
		//*/
		exit(0);
	}
	else
	{
		if (pid_p4 == -1)
			cout << "Unable to create fork P4 " << pid_p4 << endl;
	}
	//wait(0);
	sleep(1);
	cout << endl << "Starting poem building" << endl << endl;


	
	//for (int j = 0; j < 2; j++) 
	//{
		//for (int i = 0; i < 4; i++) 
		//{
		int i = 0;
		int k = 0;
		//*
		while(k < max)
		{
			if(i < 4 && k < max)
			{
				setSem(semid, i, 1);
				if (msgrcv(msgid, &rcvd, qm_len, 1, 0)) 
				{
					cout << i << "\t> " << rcvd.mtext << endl;
				}
				i++;
				k++;
			}
			else
			{	
				i = 0;			
				setSem(semid, i, 1);
				if (msgrcv(msgid, &rcvd, qm_len, 1, 0)) 
				{
					cout << i << "\t> " << rcvd.mtext << endl;
				}
				i++;
				k++;
			}
		}
		//}*/
	//}
	
	//Waiting for forks to close
	number = waitpid(pid_p1, &status, 0);
	cout << "Process with pid = " << pid_p1 << " exited with status " << status << endl;

	number = waitpid(pid_p2, &status, 0);
	cout << "Process with pid = " << pid_p2 << " exited with status " << status << endl;

	number = waitpid(pid_p3, &status, 0);
	cout << "Process with pid = " << pid_p3 << " exited with status " << status << endl;

	number = waitpid(pid_p4, &status, 0);
	cout << "Process with pid = " << pid_p4 << " exited with status " << status << endl;


	//Closing IPC structtures like message
	status = msgctl(msgid, IPC_RMID, &msq_stat);
	if (status == -1)
		cout << "error of RMD mes" << endl;
	else
		cout << "Message query remove is ok" << endl;

	//and semaphore
	status = semctl(semid, 0, IPC_RMID, arg);
	if (status == -1)
		cout << "error of RMD sem" << endl;
	else
		cout << "Sem object remove is ok" << endl;

	return 0;
}
