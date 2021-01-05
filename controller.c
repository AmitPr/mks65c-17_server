#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
int in_fd;
int out_fd;
void disconnect()
{
    close(in_fd);
    close(out_fd);
}

void sighandler(int signo)
{
    if (signo == SIGINT)
    {
        write(out_fd, "\0", 1);
        disconnect();
        exit(0);
    }
}

int main()
{
    signal(SIGINT, sighandler);
    char pid[10];
    sprintf(pid, "%d", getpid());
    printf("Running SwitchCase Client (PID: %s)\n", pid);

    printf("Connecting to server...\n");
    int fd = open(".wkp", O_WRONLY);
    if (fd == -1)
    {
        printf("Error connecting to server\nerrno: %d: %s\n", errno, strerror(errno));
        disconnect();
        exit(-1);
    }
    write(fd, pid, 10);
    close(fd);
    //Create the pipes
    char c_out[20];
    char c_in[20];
    strcpy(c_out, pid);
    strcat(c_out, "_0");
    strcpy(c_in, pid);
    strcat(c_in, "_1");
    mkfifo(c_out, 0664);
    mkfifo(c_in, 0664);
    in_fd = open(c_in, O_RDONLY);
    if (in_fd == -1)
    {
        printf("errno: %d: %s\n", errno, strerror(errno));
        exit(-1);
    }
    out_fd = open(c_out, O_WRONLY);
    if (out_fd == -1)
    {
        printf("errno: %d: %s\n", errno, strerror(errno));
        exit(-1);
    }
    printf("Initial connection established...\n");

    char ack[10];
    read(in_fd, ack, sizeof(ack));
    if(strcmp(ack, "200")!=0){
        printf("Wrong ACK from server: %s\n", ack);
        disconnect();
        exit(-1);
    }
    write(out_fd, "ACK", sizeof("ACK"));
    printf("Connected.\n");
    
    char user_input[100], output_str[100];
    while (1)
    {
        printf("Enter a line: ");
        fgets(user_input, 100, stdin);
        write(out_fd, user_input, 100);

        read(in_fd, output_str, 100);
        printf("Swapped Case: %s\n", output_str);
    }
    return 0;
}