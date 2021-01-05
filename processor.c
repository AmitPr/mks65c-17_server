#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

int in_fd;
int out_fd;
int is_open = 0;

void gracefully_exit()
{
    printf("errno: %d: %s\n", errno, strerror(errno));
    exit(-1);
}
void disconnect()
{
    close(in_fd);
    close(out_fd);
    is_open = 0;
}

void switch_case(char *str)
{
    char *C = str;
    while (*C)
    {
        if (isupper(*C))
        {
            *C = tolower(*C);
        }
        else if (islower(*C))
        {
            *C = toupper(*C);
        }
        *C++;
    }
}

void handle_connection()
{
    char input_str[100];
    while (1)
    {
        read(in_fd, input_str, 100);
        if (!*input_str)
        {
            printf("Client disconnect.\n");
            disconnect();
            return;
        }
        switch_case(input_str);
        write(out_fd, input_str, 100);
    }
}

int handshake()
{
    mkfifo(".wkp", 0664);
    char buf[10];
    int fd = open(".wkp", O_RDONLY);
    read(fd, buf, 10);
    printf("handshake initiated: %s\n", buf);
    close(fd);
    remove(".wkp");
    //Server knows it can recieve
    char c_in[20];
    char c_out[20];
    strcpy(c_in, buf);
    strcat(c_in, "_0");
    strcpy(c_out, buf);
    strcat(c_out, "_1");

    out_fd = open(c_out, O_WRONLY);
    if (out_fd == -1)
    {
        gracefully_exit();
    }
    in_fd = open(c_in, O_RDONLY);
    if (in_fd == -1)
    {
        gracefully_exit();
    }
    is_open = 1;
    int x;
    x = remove(c_in);
    if (x == -1)
    {
        gracefully_exit();
    }
    x = remove(c_out);
    if (x == -1)
    {
        gracefully_exit();
    }

    write(out_fd, "200", sizeof("200"));
    char ack[10];
    read(in_fd, ack, sizeof(ack));
    if (strcmp(ack, "ACK") != 0)
    {
        disconnect();
        printf("Error during handshake, recieved acknowledgement: %s from client.\n", ack);
        return 0;
    }
    return 1;
}

void sighandler(int signo)
{
    if (signo == SIGINT)
    {
        if (is_open)
        {
            disconnect();
        }
        else
        {
            remove(".wkp");
        }
        exit(0);
    }
}

int main()
{
    signal(SIGINT, sighandler);
    while (1)
    {
        if (handshake())
        {
            handle_connection();
        }
    }
}