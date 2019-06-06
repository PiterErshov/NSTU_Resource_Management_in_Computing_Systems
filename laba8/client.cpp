#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <cstdlib>

#include <ctype.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <sstream>

using namespace std;

int main()
{
	int sockfd;
	//string r_msg;
	char msg[100];
	char r_msg[1000];

	struct sockaddr_in server;
	struct in_addr addr;
	size_t length = sizeof(struct sockaddr_in);  // ����� ���� sockaddr_in (��� FreeBSD) 
	server.sin_family = AF_INET;		// ��������� ������� (�����) 	
	server.sin_port = htons(41123);   // ����� �����
	addr.s_addr = INADDR_ANY;
	

	//struct sockaddr_in server = { 0, AF_INET, 8000 };

	// ������������� � ��������� IP address ������� 
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	

	// ���������� ����������� ����� ������
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Error of socket");
		exit(1);
	}

/*
	struct sockaddr addr;
	addr.sa_family = AF_UNIX;
	strcpy(addr.sa_data, "8_9so");	//����������� ����� �� � ������ � � ���������� �����
	*/

	// ���������� ����� � ������ ������� 
	if (connect(sockfd, (struct sockaddr *) &server, sizeof(struct sockaddr_in)) == -1)
	{
		perror("Error of connect");
		exit(1);
	}

	/*printf("Input integer number (id) or 'exit' for break: ");
	while (getline(cin, msg))
	{
		
			cout << "Vveli1: " << msg << endl;

		}
	}*/


//scanf("%s", &msg);
	
	
	//printf("Input integer number (id) or 'exit' for break: ");
	// ����� ������� � �������� 
	for (;;)
	{
		printf("\n\nInput integer number (id) or 'exit' for break: ");
		//if (rc == '\n')
		scanf("%s", &msg);
		//cout << "Vveli: " << msg << endl;
		if (strcmp(msg, "exit"))
		{
			//int key;
			//istringstream ist(msg);
			//ist >> key;

		//cout << "key = " << key << endl;

			//cout << "Vveli = " << c << endl << endl;
			//const char *test = "01234";
	//char test[100] = "01234";
	//string test = "0123";
	//test[100] = '\0';
	//strlen(test)
	//int test;
			if (send(sockfd, &msg, sizeof(msg), 0) != -1)
				cout << "Success send" << endl;
			else
				cout << "Client dont send message" << endl;

			if (recv(sockfd, &r_msg, sizeof(r_msg), 0) != -1)
				cout << "Found element: " << r_msg;
			//printf("%c", rc.data());
			else
			{
				printf("Server don't answer\n");
				close(sockfd);
				exit(1);
			}
			//}
		//}
		//printf("Client left");
		}
		else
			break;
	}
	return 0;
}

