#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <cstdlib>
#include <netinet/in.h>
#include <unistd.h>

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <cstring>

#define n 10

using namespace std;

struct table
{
	int id;
	string name;
	string surname;
	int number;	
} student[n];


int create_base()
{
	ifstream file;
	file.open("table.txt");
	if (file.is_open())
	{
		string line;
		int i = 1;

		while (!file.eof())
		{
			getline(file, line, '\n');
			istringstream iss(line);
			iss >> student[i].id >> student[i].name >> student[i].surname >> student[i].number;

			cout << student[i].id << " ";
			cout << student[i].name << " ";
			cout << student[i].surname << " ";
			cout << student[i].number << endl;
			i++;
		}
		return i;
	}
	else
		cout << "Server: Error in file" << endl;
}



int main()
{
	
	// �������� ����. ���������� ������ �� �����
	cout << "\t Table" << endl;
	int max = create_base();
	//cout << "max = " << max;

	int sockfd, newsockfd; // id ������
	char buf[100];

	struct sockaddr_in serv_addr;
	struct in_addr addr;
	size_t length = sizeof(struct sockaddr_in);  // ����� ���� sockaddr_in (��� FreeBSD) 
	serv_addr.sin_family = AF_INET;		// ��������� ������� (�����) 	
	serv_addr.sin_port = htons(41123);   // ����� �����
	//inet_aton("192.168.0.14", &serv_addr.sin_addr.s_addr);
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // IP-�����
	//serv_addr.sin_addr.addr = inet_ntoa("192.168.0.14");
	addr.s_addr = INADDR_ANY; // INADDR_ANY, ���������� ��������� ����� ���������� � ������� in_addr_t
	

	//struct sockaddr st_addr;
	//remove("INADDR_ANY");

	//struct sockaddr_in serv_addr = { 0, AF_INET, 7000 , inet_addr("127.0.0.1") };
	struct sockaddr_in client;
	int client_len = sizeof(struct sockaddr_in);
	
	

	// �������� ������
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		perror("Server: Error in creating socket");
		exit(1);
	}
	cout << endl << "Server: Create socket with id = " << sockfd << endl;

	//st_addr.sa_family = AF_UNIX;
	//sprintf(st_addr.sa_data, "%s", "INADDR_ANY");



	// ��������� ������� ����� ���������� � ��������������� ������
	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in)) == -1)
	//if (bind(sockfd, (struct sockaddr *)&st_addr, sizeof(struct sockaddr)) == -1)
	{
		perror("Server: Error binding");
		exit(1);
	}

	// �������� ����� ���������
	if (listen(sockfd, 1) == -1)
	{
		perror("Server: Error listening");
		exit(1);
	}
	cout << "Server: Listening..." << endl;

	
	
	for (;;) // ����������� ����
	{
		// ����� ������� �� ����������
		//if ((newsockfd = accept(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr_in))) == -1)
		if ((newsockfd = accept(sockfd, (struct sockaddr *) &client, (socklen_t*)&client_len)) == -1)
		{
			perror("Error of accept socket");
			continue;
		}

		// ������ ������ �� ���������� 
		// inet_ntoa ��������� �������������� IP������ �� ������� ��������� ������ �x.y.z.t� � ��������� ���� in_addr � �������. 
		cout << "Server: Request from IP: " << inet_ntoa(client.sin_addr) << endl;

		// ������� �������� ������� ��� ������ � ���� �����������
		if (fork() == 0)
		{
			cout << "Server: Create fork" << endl;
			char key[100];
			//strcpy(key, "1541656");
			//string key = "";
			//int key;
			//cout << endl << endl << "Server: Received from client key: " << key << endl;
			while (recv(newsockfd, &key, sizeof(key), 0) != -1)
			{

				string str_;
				char result[100];
				int index;
				cout << endl << endl << "Server: Received from client key: " << key << endl;
				//cout << key.length() << endl;
				//puts(key);
			
				istringstream ist(key);
				ist >> index;
				//index = key;

				if (index >= 1 && index <= max - 1)
				{
					cout << student[index].id << " " << student[index].name << " " << student[index].surname << " " << student[index].number << endl;
					stringstream ss;
					ss << student[index].id << " " << student[index].name << " " << student[index].surname << " " << student[index].number;
					
					str_ = ss.str();
					str_.copy(result, 100);
				

					send(newsockfd, &result, sizeof(result), 0);
				}
				else
				{
					//stringstream ss;
					//ss << key;
					//result = "Server: Not find element";
					//result = string("Server: Not find element with key ") + key;
					send(newsockfd, &("Server: Not find element"), sizeof("Server: Not find element"), 0);
				}			
			}

			// ����� ����, ��� ������ ��������� �������� ������, ����� ����� ���� ������ � �������� �������
			//��������� ������
			//cout << "Server: close socket" << endl;
			close(newsockfd);
			exit(0);
		}

		// � ������������ �������� newsockfd �� �����
		//cout << "Server: close socket2" << endl;
		close(newsockfd);
	}

	return 0;
}
