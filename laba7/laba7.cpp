
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>

/* Наиболее общим понятием IPC (средства межпроцессорного взаимодействия) является ключ, хранимый в общесистемной
таблице и обозначающий объект межпроцессного взаимодействия, доступный
нескольким процессам. Обозначаемый ключом объект может быть очередью
сообщений, набором семафоров или сегментом разделяемой памяти. Ключ имеет
тип key_t, состав которого зависит от реализации и определяется в файле
<sys/types.h>. Ключ используется для создания объекта межпроцессного
взаимодействия или получения доступа к существующему объекту. Обе операции
выполняются посредством операции get. Результатом операции get является его
целочисленный идентификатор, который может использоваться в других
функциях межпроцессного взаимодействия. */


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

/* Функция semctl позволяет выполнять операции, определенные в cmd над набором семафоров, 
указанным в semid или над семафором с номером semnum из этого набора. (Семафоры нумеруются, начиная с 0.)
Функция имеет три или четыре аргумента. Если аргументов четыре, то вызов выглядит как 
semctl(semid,semnum,cmd,arg); где четвертый аргумент arg имеет тип union semun, определенный как  typedef union semun*/
typedef union semun 
{
	int val;                 /* value for SETVAL */
	struct semid_ds *buf;	/* buffer for IPC_STAT, IPC_SET */
	short *array;			/* array for GETALL, SETALL */ 
} semun;


// Структура для обработки сообщений запроса
/* Для помещения сообщения в очередь служит системный вызов msgsnd():
int msgsnd (int msgqid, void *msg, size_t size, int flag),
где msg – указатель на структуру длиной size, содержащую определяемый
пользователем целочисленный тип сообщения и символьный массив-сообщение.
Структура msg имеет вид: */
struct msg 
{
	long mtype;      /* тип сообщения */
	char mtext[64];  /* текст сообщения (размер задается какой хочешь) */
};



// Функция инициализации очереди
/*int initmsg(key_t msgkey) 
{    
	int msgid;

	// msgget - функция возвращает идентификатор очереди сообщений, связанный со значением параметра key
	/*  Поля IPC_CREAT и IPC_EXCL в msgflg играют ту же роль для очередей сообщений,
	 что и O_CREAT и O_EXCL в параметре mode системной функции open(2)
	 функция msgget вернет ошибку, если в msgflg включены оба флага: IPC_CREAT и IPC_EXCL ,- а такая очередь сообщений для key уже существует. */
/* O_EXCL - Если он используется совместно с O_CREAT, то при наличии уже созданного файла вызов open завершится с ошибкой.  */
 /*Права доступа 0600, то есть rw------- */
/*	msgid = msgget(msgkey, 0600 | IPC_CREAT | IPC_EXCL);

	/* При удачном завершении вызова возвращаемое значение равно нулю. При ошибке возвращается -1,
	 а переменной errno присваивается номер ошибки. */
/*	if (msgid == -1) 
	{
		if (errno == EEXIST) //  если очередь существовала ранее   
			msgid = msgget(msgkey, 0);	//то открыть ее 
		cout << "Already exist" << endl; // Уже существует

		return (-1);
	}

	return (msgid);
} */

// Инициализация семафора
//*
int initsem(key_t semkey, int num) 
{
	int semid;
	semid = semget(semkey, num, 0666 | IPC_CREAT | IPC_EXCL);
	/* semget - Системный вызов возвращает идентификатор набора семафоров, связанный
	 с аргументом key. Создается новый набор из семафоров num,
	 если значение key равно IPC_PRIVATE или c ключом key не связано ни одного
	  существующего набора семафора, а выражение semflg & IPC_CREAT истинно.*/
//*	
if (semid == -1) 
	{
		if (errno == EEXIST)  //если набор семафоров существовал ранее   
			semid = semget(semkey, num, 0);      // то открыть его            
		return (-1);
	}

	return (semid);
}
//*/

int initmsg (key_t msgkey) 
{    // функция инициализации очереди

  int msgid;
  msgid = msgget(msgkey, 0600 | IPC_CREAT|IPC_EXCL);

  if (msgid == -1) 
  {
	  cout << "Error in initmsg" << endl;

            if (errno == EEXIST) //  если очередь существовала ранее              
			msgid = msgget (msgkey, 0);	//то открыть ее 

  } 



  return (msgid);             

}


int initsem (key_t semkey, int num, union semun arg) 
{

  int semid, status = 0;

semid = semget (semkey, num, 0600 | IPC_CREAT | IPC_EXCL);
  if (semid == -1) 
  {

    if (errno == EEXIST) /*  если набор семафоров существовал ранее   */

      semid = semget (semkey, num, 0);    /*   то открыть его            */
else{
	arg.val = 1;
	status = semctl(semid,0,SETVAL,arg);
}
  } 
  if(semid == -1 || status == -1)
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
	int number, status, msgid, semid;
	struct msqid_ds msq_stat;
	union semun arg;
	msg qm_p1, qm_p2, qm_p3, qm_p4, rcvd;	// Для отправки / доставки сообщений

	char* poem[] = {
	  "Some say the world will end in fire, ",
	  "Some say in ice.",
	  "From what I’ve tasted of desire",
	  "I hold with those who favor fire.",
	  "But if it had to perish twice,",
	  "I think I know enough of hate",
	  "To say that for destruction ice",
	  "Is also great",
	  "And would suffice.",
	};

	const int qm_len = sizeof(msg); // Размер сообщения
//*
	// Инициализация семафора
	if ((semid = initsem(semkey, 4, arg)) < 0) 
	{
		cout << "semid = " << semid << " errno = " << errno << endl;
		exit(1);
	}
	cout << "Successfully semid = " << semid << endl;

	// Инициализация очереди
	if ((msgid = initmsg(msgkey)) < 0) 
	{
		cout << "msgid = " << msgid << " errno =" << errno << endl;
		return (-1);
	}
	cout << "Successfully msgid = " << msgid << endl;

	// Установка базовых семафорных чисел
	for (int i = 0; i < 4; i++) 
	{
		arg.val = 0;	// Установить 0 в качестве основы

		/* Функция semctl позволяет выполнять операции, определенные в cmd над набором семафоров,
		 указанным в semid или над семафором с номером semnum из этого набора. (Семафоры нумеруются, начиная с 0.)
Функция имеет три или четыре аргумента. Если аргументов четыре, то вызов выглядит как semctl(semid,semnum,cmd,arg);
 где четвертый аргумент arg имеет тип union semun  */

 /* SETVAL
Установите значение semval на указанное в arg.val для всех семафоров semnum-th, изменяя также
 sem_ctime в структуре semid_ds, соотносимой с набором семафоров. История отмененных операций
  удаляется для всех измененных семафоров во всех процессах. Процессы, находящиеся в очереди,
   активизируются, если semval становится равным нулю или значение его увеличивается. 
   Вызывающему процессу потребуется право на его изменение. */
		status = semctl(semid, i, SETVAL, arg);
		
	//printf("error: %s\n", strerror( errno ) );
		cout << "sem =" << i << "\tstatus =" << status << "\terrno =" << errno << endl;
	}

	int max = sizeof(poem)/sizeof(*poem);
	// Создание процесса P1
	pid_p1 = fork();
	if (pid_p1 == 0) 
	{	
		cout << endl << "Fork P1 created" << endl << endl;
		qm_p1.mtype = 1;
		/*for (int i = 0; i < max-1; i += 4) 
		{
			setSem(semid, 0, -1);					// Блокировка до установки семафора
			sprintf(qm_p1.mtext, "%s", poem[i]);	// Установка строки из стихотворения как сообщения
			cout << endl << i << "\t" << qm_p1.mtext << endl;

			// Отправка сообщения
			if (msgsnd(msgid, &qm_p1, qm_len, 0) == -1) 
			{	
				cout << "Error of send message!" << endl;
				return(-1);
			}
		}*/

		exit(0);
	}
	else
		if (pid_p1 == -1) 
	  cout << "Unable to create fork P1 " << pid_p1 << endl;

	// Создание процесса P2
	pid_p2 = fork();
	if (pid_p2 == 0) 
	{
		cout << endl << "Fork P2 created" << endl << endl;
		qm_p2.mtype = 1;
		/*for (int i = 1; i < max-1; i += 4)
		{
			setSem(semid, 1, -1);	// Блокируем этот процесс, пока у нас не будет разрешения на отправку сообщения
			sprintf(qm_p2.mtext, "%s", poem[i]);	// Установка строки из стихотворения как сообщения
			cout << endl << i << "\t" << qm_p2.mtext << endl;

			// Отправка сообщения
			if (msgsnd(msgid, &qm_p2, qm_len, 0) == -1) 
			{	
				cout << "Error of send message!" << endl;
				return(-1);
			}
		}*/

		exit(0);
	}
	else
		if (pid_p1 == -1) 
	  cout << "Unable to create fork P2 " << pid_p2 << endl;

	// Создание процесса P3
	pid_p3 = fork();
	if (pid_p3 == 0) 
	{
		cout << endl << "Fork P3 created" << endl << endl;
		qm_p3.mtype = 1;
		/*for (int i = 2; i < max-1; i += 4) 
		{
			setSem(semid, 2, -1);
			sprintf(qm_p3.mtext, "%s", poem[i]);	// Установка строки из стихотворения как сообщения
			cout << endl << i << "\t" << qm_p3.mtext << endl;

			// Отправка сообщения
			if (msgsnd(msgid, &qm_p3, qm_len, 0) == -1) 
			{	
				cout << "Error of send message!" << endl;
				return(-1);
			}
		}*/

		exit(0);
	}
	else
		if (pid_p1 == -1) 
	  cout << "Unable to create fork P3 " << pid_p3 << endl;

	// Создание процесса P4
	pid_p4 = fork();
	if (pid_p4 == 0) 
	{
		cout << endl << "Fork P4 created" << endl << endl;
		qm_p4.mtype = 1;
		/*for (int i = 3; i < max; i += 4) 
		{
			setSem(semid, 3, -1);
			sprintf(qm_p4.mtext, "%s", poem[i]);	// Установка строки из стихотворения как сообщения
			cout << endl << i << "\t" << qm_p4.mtext << endl;

			// Отправка сообщения
			if (msgsnd(msgid, &qm_p4, qm_len, 0) == -1) 
			{	
				cout << "Error of send message!" << endl;
				return(-1);
			}
		}*/

		exit(0);
	}
	else
		if (pid_p1 == -1) 
	  cout << "Unable to create fork P4 " << pid_p4 << endl;

	cout << endl << "All process created" << endl << endl;
/*
	for (int j = 0; j < 3 ; j++) 
	{
		cout << "1 for" << endl;
		// Цикл для 4-х семафоров
		for (int i = 0; i < 4; i++) 
		{
			cout << "2 for" << endl;
			setSem(semid, i, 1);	// разрешение на отправку сообщения

			// Получение сообщения
			if (msgrcv(msgid, &rcvd, qm_len, 1, 0)) 
				cout << i << "\t>" << rcvd.mtext << endl;
		}
	}

	// Ожидание закрытия всех процессов
	number = waitpid(pid_p1, &status, 0);
	// Процесс вышел со стаусом...
	cout << "Process with pid = " << pid_p1 << " exited with status " << status << endl;

	number = waitpid(pid_p2, &status, 0);
	cout << "Process with pid = " << pid_p2 << " exited with status " << status << endl;

	number = waitpid(pid_p3, &status, 0);
	cout << "Process with pid = " << pid_p3 << " exited with status " << status << endl;

	number = waitpid(pid_p4, &status, 0);
	cout << "Process with pid = " << pid_p4 << " exited with status " << status << endl;

	// Закрытие IPC структур
	status = msgctl(msgid, IPC_RMID, &msq_stat);

	if (status == -1)
		cout << "Error of RMD mes" << endl;
	else
		cout << "Message query remove is ok" << endl;

	// и семафоров
	status = semctl(semid, 0, IPC_RMID, arg);

	if (status == -1)
		cout << "Error of RMD sem" << endl;
	else
		cout << "Sem object remove is ok" << endl;
*/
	return 0;
}
