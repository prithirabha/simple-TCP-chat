#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>


#define MSG_LEN     2048
#define MAX_CLIENT  128

//functions to be used
void* serve_clientient(void* arg);
void broadcast_msg(char *msg, int len);
void handle_err(char *err_msg);

//global vars
int client_count = 0;
int client_socks[MAX_CLIENT];
pthread_mutex_t mutex;

int main(int argc, char *argv[])
{
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    int client_addr_size;
    pthread_t t_id;  
    
    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    pthread_mutex_init(&mutex, NULL);

    server_sock = socket(PF_INET, SOCK_STREAM, 0);  

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));


    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
        handle_err("ERROR: bind() fail");
    }

    if (listen(server_sock, 5) == -1){
        handle_err("ERROR: listen() fail");
    }

    system("clear");

    printf("Group chat server running ... (port: %s)\n", argv[1]);

    while (1) {
        client_addr_size = sizeof(client_addr);

        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_size);

        pthread_mutex_lock(&mutex);

        client_socks[client_count++] = client_sock;  

        pthread_mutex_unlock(&mutex);

        pthread_create(&t_id, NULL, serve_client, (void*)&client_sock); 

        pthread_detach(t_id); 

        printf("Connected client IP: %s\n", inet_ntoa(client_addr.sin_addr));
    }

    close(server_sock);   

    return 0;
} 

// receive client's message, and broadcast it to all connected clients
void* serve_client(void *arg)
{
    int client_sock = *((int*)arg);

    int str_len = 0, i;

    char msg[MSG_LEN];

    while ((str_len = read(client_sock, msg, sizeof(msg))) != 0){
        broadcast_msg(msg, str_len);
    }
    
    pthread_mutex_lock(&mutex);

    for (i = 0; i < client_count; i++)    
    {
        if (client_sock == client_socks[i])
        {
            while (i < client_count - 1)
            {
                client_socks[i] = client_socks[i + 1];
                i++;
            }

            break;
        }
    }

    client_count--;
    pthread_mutex_unlock(&mutex);

    close(client_sock);

    return NULL;
}

// broadcast message to all connected clients
void broadcast_msg(char *msg, int len)
{
    int i;
    
    pthread_mutex_lock(&mutex);

    for (i = 0; i < client_count; i++){
        write(client_socks[i], msg, len);
    }

    pthread_mutex_unlock(&mutex);
}

// handle error
void handle_err(char *err_msg)
{
    fputs(err_msg, stderr);
    fputc('\n', stderr);
    exit(1);
} 
