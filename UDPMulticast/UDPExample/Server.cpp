#include <winsock2.h>
#include <iostream>
#include <vector>
#include <string>
using namespace std;

#define MAX_CLIENTS 10
#define DEFAULT_BUFLEN 4096

#pragma comment(lib, "ws2_32.lib") // Winsock library
#pragma warning(disable:4996) // отключаем предупреждение _WINSOCK_DEPRECATED_NO_WARNINGS

SOCKET server_socket;

vector<string> history;
vector<int> historyColors;

bool Registration[MAX_CLIENTS] = { false,false,false,false,false,false,false,false,false,false };
int Colors_system[MAX_CLIENTS] = {7,7,7,7,7,7,7,7,7,7};
char UserNames[MAX_CLIENTS][256];


char UserName[256];


int Color = 7;
string cl;
int Index;
int main() {
	system("title Server");

	puts("Start server... DONE.");
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		printf("Failed. Error Code: %d", WSAGetLastError());
		return 1;
	}

	// create a socket
	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		printf("Could not create socket: %d", WSAGetLastError());
		return 2;
	}
	// puts("Create socket... DONE.");

	// prepare the sockaddr_in structure
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(8888);

	// bind socket
	if (bind(server_socket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
		printf("Bind failed with error code: %d", WSAGetLastError());
		return 3;
	}

	// puts("Bind socket... DONE.");

	// слушать входящие соединения
	listen(server_socket, MAX_CLIENTS);

	// принять и входящее соединение
	puts("Server is waiting for incoming connections...\nPlease, start one or more client-side app.");

	// размер нашего приемного буфера, это длина строки
	// набор дескрипторов сокетов
	// fd means "file descriptors"
	fd_set readfds; // https://docs.microsoft.com/en-us/windows/win32/api/winsock/ns-winsock-fd_set
	SOCKET client_socket[MAX_CLIENTS] = {};

	while (true) {
		// очистить сокет fdset
		FD_ZERO(&readfds);

		// добавить главный сокет в fdset
		FD_SET(server_socket, &readfds);

		// добавить дочерние сокеты в fdset
		for (int i = 0; i < MAX_CLIENTS; i++) 
		{
			SOCKET s = client_socket[i];
			if (s > 0) {
				FD_SET(s, &readfds);
			}
		}

		// дождитесь активности на любом из сокетов, тайм-аут равен NULL, поэтому ждите бесконечно
		if (select(0, &readfds, NULL, NULL, NULL) == SOCKET_ERROR) {
			printf("select function call failed with error code : %d", WSAGetLastError());
			return 4;
		}

		// если что-то произошло на мастер-сокете, то это входящее соединение
		SOCKET new_socket; // новый клиентский сокет
		sockaddr_in address;
		int addrlen = sizeof(sockaddr_in);
		if (FD_ISSET(server_socket, &readfds)) {
			if ((new_socket = accept(server_socket, (sockaddr*)&address, &addrlen)) < 0) {
				perror("accept function error");
				return 5;
			}
			send(new_socket, "Type name", 9, 0);
			for (int i = 0; i < history.size(); i++)
			{
				cout << history[i] << "\n";
				string buff3 = "|";
				buff3 += to_string(historyColors[i]);
				//send(new_socket, buff3.c_str(), buff3.length(), 0);
				send(new_socket, history[i].c_str(), history[i].size(), 0);
			}

			// информировать серверную сторону о номере сокета - используется в командах отправки и получения
			printf("New connection, socket fd is %d, ip is: %s, port: %d\n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));
			
			// добавить новый сокет в массив сокетов
			for (int i = 0; i < MAX_CLIENTS; i++) {
				if (client_socket[i] == 0) {
					client_socket[i] = new_socket;
					printf("Adding to list of sockets at index %d\n", i);
					break;
				}
			}
		}

		// если какой-то из клиентских сокетов отправляет что-то
		string FromUser;
		for (int i = 0; i < MAX_CLIENTS; i++)
		{
			SOCKET s = client_socket[i];
			// если клиент присутствует в сокетах чтения
			if (FD_ISSET(s, &readfds))
			{
				// получить реквизиты клиента
				getpeername(s, (sockaddr*)&address, (int*)&addrlen);

				// проверьте, было ли оно на закрытие, а также прочитайте входящее сообщение
				// recv не помещает нулевой терминатор в конец строки (в то время как printf %s предполагает, что он есть)
				
				char client_message[DEFAULT_BUFLEN];
				int client_message_length = recv(s, client_message, DEFAULT_BUFLEN, 0);
				client_message[client_message_length] = '\0';
			
				if (Registration[i] == false && client_socket[i]!= INVALID_SOCKET) {
			
					client_message[client_message_length] = '\0';
					strcpy(UserNames[i], client_message);

					string newUser = "New user came to us  ";
					newUser += UserNames[i];
					for (int z = 0; z < MAX_CLIENTS; z++)
					{
						if (client_socket[z] != INVALID_SOCKET) {
							send(client_socket[z], newUser.c_str(), newUser.length(), 0);
						}
					}
					
					
					Registration[i] = true;

				}

				else if (Registration[i] == true && client_socket[i] != INVALID_SOCKET) {

					client_message[client_message_length] = '\0';
					
					 

					
					cl = client_message;
					Index = cl.find("clr");
					if (Index != -1)
					{
						cl.erase(0, Index + 3);
						Color = stoi(cl);
						Colors_system[i] = Color;
						strcpy(client_message, " ");
						
					}
					else
					{
						if (Registration[i] == true) {

							string temp = UserNames[i];

							temp += ": ";
							temp += client_message;
							temp += '\n';
							// temp += "\n";
							history.push_back(temp);
							historyColors.push_back(Colors_system[i]);
						}
				string check_exit = client_message;
				if (check_exit == "off")
				{
					string newUser = "New user leave us  ";
					newUser += UserNames[i];
					for (int z = 0; z < MAX_CLIENTS; z++)
					{
						if (client_socket[z] != INVALID_SOCKET) {
							send(client_socket[z], newUser.c_str(), newUser.length(), 0);
						}
					}
					cout << "Client #" << i << " is off\n";
					client_socket[i] = 0;
				}
			

				if (client_socket[i] != INVALID_SOCKET) {
					FromUser = UserNames[i];
				}
				string buff3 = "|";
				buff3 += to_string(Colors_system[i]);
				for (int i = 0; i < MAX_CLIENTS; i++) {
					if (client_socket[i] != 0 && Registration[i] == true) {

						string buff2 = FromUser;
						
						buff2[buff2.length()] = ' ';
						buff2 += ": ";
						buff2 += client_message;

						buff2 += '\0';
						send(client_socket[i], buff3.c_str(), buff3.length(), 0);

						send(client_socket[i], buff2.c_str(), buff2.length(), 0);
					
					}
				}
					}
					

				}

			
				
	
				
				


				


			}
		}
	}

	WSACleanup();
}