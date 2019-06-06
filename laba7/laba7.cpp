
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>

/* �������� ����� �������� IPC (�������� ���������������� ��������������) �������� ����, �������� � �������������
������� � ������������ ������ �������������� ��������������, ���������
���������� ���������. ������������ ������ ������ ����� ���� ��������
���������, ������� ��������� ��� ��������� ����������� ������. ���� �����
��� key_t, ������ �������� ������� �� ���������� � ������������ � �����
<sys/types.h>. ���� ������������ ��� �������� ������� ��������������
�������������� ��� ��������� ������� � ������������� �������. ��� ��������
����������� ����������� �������� get. ����������� �������� get �������� ���
������������� �������������, ������� ����� �������������� � ������
�������� �������������� ��������������. */


#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

// ��� ������� ���������
#include <sys/ipc.h>
#include <sys/msg.h>

//��� ���������
#include <sys/sem.h> 
#include <errno.h>

using namespace std;

/* ������� semctl ��������� ��������� ��������, ������������ � cmd ��� ������� ���������, 
��������� � semid ��� ��� ��������� � ������� semnum �� ����� ������. (�������� ����������, ������� � 0.)
������� ����� ��� ��� ������ ���������. ���� ���������� ������, �� ����� �������� ��� 
semctl(semid,semnum,cmd,arg); ��� ��������� �������� arg ����� ��� union semun, ������������ ���  typedef union semun*/
typedef union semun 
{
	int val;                 /* value for SETVAL */
	struct semid_ds *buf;	/* buffer for IPC_STAT, IPC_SET */
	short *array;			/* array for GETALL, SETALL */ 
} semun;


// ��������� ��� ��������� ��������� �������
/* ��� ��������� ��������� � ������� ������ ��������� ����� msgsnd():
int msgsnd (int msgqid, void *msg, size_t size, int flag),
��� msg � ��������� �� ��������� ������ size, ���������� ������������
������������� ������������� ��� ��������� � ���������� ������-���������.
��������� msg ����� ���: */
struct msg 
{
	long mtype;      /* ��� ��������� */
	char mtext[64];  /* ����� ��������� (������ �������� ����� ������) */
};



// ������� ������������� �������
/*int initmsg(key_t msgkey) 
{    
	int msgid;

	// msgget - ������� ���������� ������������� ������� ���������, ��������� �� ��������� ��������� key
	/*  ���� IPC_CREAT � IPC_EXCL � msgflg ������ �� �� ���� ��� �������� ���������,
	 ��� � O_CREAT � O_EXCL � ��������� mode ��������� ������� open(2)
	 ������� msgget ������ ������, ���� � msgflg �������� ��� �����: IPC_CREAT � IPC_EXCL ,- � ����� ������� ��������� ��� key ��� ����������. */
/* O_EXCL - ���� �� ������������ ��������� � O_CREAT, �� ��� ������� ��� ���������� ����� ����� open ���������� � �������.  */
 /*����� ������� 0600, �� ���� rw------- */
/*	msgid = msgget(msgkey, 0600 | IPC_CREAT | IPC_EXCL);

	/* ��� ������� ���������� ������ ������������ �������� ����� ����. ��� ������ ������������ -1,
	 � ���������� errno ������������� ����� ������. */
/*	if (msgid == -1) 
	{
		if (errno == EEXIST) //  ���� ������� ������������ �����   
			msgid = msgget(msgkey, 0);	//�� ������� �� 
		cout << "Already exist" << endl; // ��� ����������

		return (-1);
	}

	return (msgid);
} */

// ������������� ��������
//*
int initsem(key_t semkey, int num) 
{
	int semid;
	semid = semget(semkey, num, 0666 | IPC_CREAT | IPC_EXCL);
	/* semget - ��������� ����� ���������� ������������� ������ ���������, ���������
	 � ���������� key. ��������� ����� ����� �� ��������� num,
	 ���� �������� key ����� IPC_PRIVATE ��� c ������ key �� ������� �� ������
	  ������������� ������ ��������, � ��������� semflg & IPC_CREAT �������.*/
//*	
if (semid == -1) 
	{
		if (errno == EEXIST)  //���� ����� ��������� ����������� �����   
			semid = semget(semkey, num, 0);      // �� ������� ���            
		return (-1);
	}

	return (semid);
}
//*/

int initmsg (key_t msgkey) 
{    // ������� ������������� �������

  int msgid;
  msgid = msgget(msgkey, 0600 | IPC_CREAT|IPC_EXCL);

  if (msgid == -1) 
  {
	  cout << "Error in initmsg" << endl;

            if (errno == EEXIST) //  ���� ������� ������������ �����              
			msgid = msgget (msgkey, 0);	//�� ������� �� 

  } 



  return (msgid);             

}


int initsem (key_t semkey, int num, union semun arg) 
{

  int semid, status = 0;

semid = semget (semkey, num, 0600 | IPC_CREAT | IPC_EXCL);
  if (semid == -1) 
  {

    if (errno == EEXIST) /*  ���� ����� ��������� ����������� �����   */

      semid = semget (semkey, num, 0);    /*   �� ������� ���            */
else{
	arg.val = 1;
	status = semctl(semid,0,SETVAL,arg);
}
  } 
  if(semid == -1 || status == -1)
	  return(-1);

  return (semid);             

}



// ������� ��������� �������� ��������
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
	pid_t pid_p1, pid_p2, pid_p3, pid_p4; 	// �������������� ���������
	key_t semkey = 0X200, msgkey = 0X201;	// ����� ��� IPC ������
	int number, status, msgid, semid;
	struct msqid_ds msq_stat;
	union semun arg;
	msg qm_p1, qm_p2, qm_p3, qm_p4, rcvd;	// ��� �������� / �������� ���������

	char* poem[] = {
	  "Some say the world will end in fire, ",
	  "Some say in ice.",
	  "From what I�ve tasted of desire",
	  "I hold with those who favor fire.",
	  "But if it had to perish twice,",
	  "I think I know enough of hate",
	  "To say that for destruction ice",
	  "Is also great",
	  "And would suffice.",
	};

	const int qm_len = sizeof(msg); // ������ ���������
//*
	// ������������� ��������
	if ((semid = initsem(semkey, 4, arg)) < 0) 
	{
		cout << "semid = " << semid << " errno = " << errno << endl;
		exit(1);
	}
	cout << "Successfully semid = " << semid << endl;

	// ������������� �������
	if ((msgid = initmsg(msgkey)) < 0) 
	{
		cout << "msgid = " << msgid << " errno =" << errno << endl;
		return (-1);
	}
	cout << "Successfully msgid = " << msgid << endl;

	// ��������� ������� ���������� �����
	for (int i = 0; i < 4; i++) 
	{
		arg.val = 0;	// ���������� 0 � �������� ������

		/* ������� semctl ��������� ��������� ��������, ������������ � cmd ��� ������� ���������,
		 ��������� � semid ��� ��� ��������� � ������� semnum �� ����� ������. (�������� ����������, ������� � 0.)
������� ����� ��� ��� ������ ���������. ���� ���������� ������, �� ����� �������� ��� semctl(semid,semnum,cmd,arg);
 ��� ��������� �������� arg ����� ��� union semun  */

 /* SETVAL
���������� �������� semval �� ��������� � arg.val ��� ���� ��������� semnum-th, ������� �����
 sem_ctime � ��������� semid_ds, ����������� � ������� ���������. ������� ���������� ��������
  ��������� ��� ���� ���������� ��������� �� ���� ���������. ��������, ����������� � �������,
   ��������������, ���� semval ���������� ������ ���� ��� �������� ��� �������������. 
   ����������� �������� ����������� ����� �� ��� ���������. */
		status = semctl(semid, i, SETVAL, arg);
		
	//printf("error: %s\n", strerror( errno ) );
		cout << "sem =" << i << "\tstatus =" << status << "\terrno =" << errno << endl;
	}

	int max = sizeof(poem)/sizeof(*poem);
	// �������� �������� P1
	pid_p1 = fork();
	if (pid_p1 == 0) 
	{	
		cout << endl << "Fork P1 created" << endl << endl;
		qm_p1.mtype = 1;
		/*for (int i = 0; i < max-1; i += 4) 
		{
			setSem(semid, 0, -1);					// ���������� �� ��������� ��������
			sprintf(qm_p1.mtext, "%s", poem[i]);	// ��������� ������ �� ������������� ��� ���������
			cout << endl << i << "\t" << qm_p1.mtext << endl;

			// �������� ���������
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

	// �������� �������� P2
	pid_p2 = fork();
	if (pid_p2 == 0) 
	{
		cout << endl << "Fork P2 created" << endl << endl;
		qm_p2.mtype = 1;
		/*for (int i = 1; i < max-1; i += 4)
		{
			setSem(semid, 1, -1);	// ��������� ���� �������, ���� � ��� �� ����� ���������� �� �������� ���������
			sprintf(qm_p2.mtext, "%s", poem[i]);	// ��������� ������ �� ������������� ��� ���������
			cout << endl << i << "\t" << qm_p2.mtext << endl;

			// �������� ���������
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

	// �������� �������� P3
	pid_p3 = fork();
	if (pid_p3 == 0) 
	{
		cout << endl << "Fork P3 created" << endl << endl;
		qm_p3.mtype = 1;
		/*for (int i = 2; i < max-1; i += 4) 
		{
			setSem(semid, 2, -1);
			sprintf(qm_p3.mtext, "%s", poem[i]);	// ��������� ������ �� ������������� ��� ���������
			cout << endl << i << "\t" << qm_p3.mtext << endl;

			// �������� ���������
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

	// �������� �������� P4
	pid_p4 = fork();
	if (pid_p4 == 0) 
	{
		cout << endl << "Fork P4 created" << endl << endl;
		qm_p4.mtype = 1;
		/*for (int i = 3; i < max; i += 4) 
		{
			setSem(semid, 3, -1);
			sprintf(qm_p4.mtext, "%s", poem[i]);	// ��������� ������ �� ������������� ��� ���������
			cout << endl << i << "\t" << qm_p4.mtext << endl;

			// �������� ���������
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
		// ���� ��� 4-� ���������
		for (int i = 0; i < 4; i++) 
		{
			cout << "2 for" << endl;
			setSem(semid, i, 1);	// ���������� �� �������� ���������

			// ��������� ���������
			if (msgrcv(msgid, &rcvd, qm_len, 1, 0)) 
				cout << i << "\t>" << rcvd.mtext << endl;
		}
	}

	// �������� �������� ���� ���������
	number = waitpid(pid_p1, &status, 0);
	// ������� ����� �� �������...
	cout << "Process with pid = " << pid_p1 << " exited with status " << status << endl;

	number = waitpid(pid_p2, &status, 0);
	cout << "Process with pid = " << pid_p2 << " exited with status " << status << endl;

	number = waitpid(pid_p3, &status, 0);
	cout << "Process with pid = " << pid_p3 << " exited with status " << status << endl;

	number = waitpid(pid_p4, &status, 0);
	cout << "Process with pid = " << pid_p4 << " exited with status " << status << endl;

	// �������� IPC ��������
	status = msgctl(msgid, IPC_RMID, &msq_stat);

	if (status == -1)
		cout << "Error of RMD mes" << endl;
	else
		cout << "Message query remove is ok" << endl;

	// � ���������
	status = semctl(semid, 0, IPC_RMID, arg);

	if (status == -1)
		cout << "Error of RMD sem" << endl;
	else
		cout << "Sem object remove is ok" << endl;
*/
	return 0;
}
