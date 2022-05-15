#include <stdio.h>
#include <string.h>
#include <winsock2.h>



#pragma comment(lib, "ws2_32")
#define _CRT_SECURE_NO_WARNINGS
DWORD WINAPI ClientThread(LPVOID);
void RemoveClient(SOCKET);



SOCKET clients[64];
int numClients;



int main()
{
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);



    SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);



    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(8888);



    bind(listener, (SOCKADDR*)&addr, sizeof(addr));
    listen(listener, 5);



    numClients = 0;



    while (1)
    {
        SOCKET client = accept(listener, NULL, NULL);
        printf("New client accepted: %d\n", client);



        // Them client vao mang
        clients[numClients] = client;
        numClients++;



        CreateThread(0, 0, ClientThread, &client, 0, 0);
    }



    closesocket(listener);
    WSACleanup();
}



DWORD WINAPI ClientThread(LPVOID lpParam)
{
    SOCKET client = *(SOCKET*)lpParam;
    char client_send[128],buf[256], cmd[32];
    const char* find = "client_id:";
    int ret,rec;
    boolean wrong = true;
    while (1)
    {
        // Nhan du lieu tu client
        while (wrong) {
            printf("Client_name:\n Example: client_id:xxxxxxx\n");
            rec = recv(client, client_send, sizeof(client_send), 0);
            
            if (strstr(client_send, find) != NULL) {
                wrong = false;
                printf("Accept client\n");
            }
            else {
                printf("Wrong syntax: ""client_id:xxxxx""\n");
            }
            client_send[rec] = '\0';
            
        }
        char* client_id = client_send + strlen(find);
        client_id[rec] = '\0';
        ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)// neu ma co loi thi xoa client do luon
        {
            RemoveClient(client);
            break;
        }
        // In ra man hinh
        buf[ret] = '\0';
       
        // Xu ly du lieu
        // Doc tu dau tien va luu vao cmd
        sscanf(buf, "%s", cmd); //doc cac ky tu lien tiep nhau den khi xuat hien space 


        if (strcmp(cmd, "all") == 0)    //chat nhom, neu du lieu nhan duoc la all... thi chuyen sang chat nhom 
        {
            // Gui cho cac client khac
            for (int i = 0; i < numClients; i++)
                if (clients[i] != client) // tranh viec gui lai cho chinh client dang dui len
                {
                    char* msg = buf + strlen(cmd) +1; // message gui di bat dau tu ki tu tiep theo sau cmd
                    char sendd[128];
                    strcpy(sendd, client_id);
                    strcat(sendd, msg);
                    send(clients[i], sendd, strlen(sendd), 0);
                }
        }
        else // Chat ca nhan
        {
            int id = atoi(cmd); // chuyen chuoi cmd sang so
            // Gui cho client id
            for (int i = 0; i < numClients; i++)
                if (clients[i] == id)
                {
                    char* msg = buf + strlen(cmd);
                    msg = strcat(msg, client_id);
                    puts(msg);
                    send(clients[i], (const char*)msg, strlen(msg), 0);
                }
        }
    }
    closesocket(client);
}



void RemoveClient(SOCKET client)
{
    // Tim vi tri can xoa
    int i = 0;
    while (clients[i] != client) i++;



    // Xoa socket client khoi mang
    if (i < numClients - 1)
        clients[i] = clients[numClients - 1];



    // Giam so phan tu di 1
    numClients--;
}