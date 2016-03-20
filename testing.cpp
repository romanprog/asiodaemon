#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#ifndef S_SPLINT_S    // Workaround for splint.
#include <unistd.h>
#endif

unsigned char * install_dname(unsigned char *p, char *dname)
{
    *p = '.';
    p++;
    strcpy((char *)p, dname);
    p--;

    while(*p != '\0')
    {
        if(*p == '.')
        {
            unsigned char *end = p + 1;
            while(!(*end == '.' || *end == '\0'))
            {
                end++;
            }
            *p = end - p - 1;
        }
        p++;
    }
    return p;
}

void sig_alarm(int signo)
{
    alarm(5);
}

int main(int argc, char **argv)
{
    int            socket_handle;
    unsigned short port = 53;
    struct sockaddr_in servaddr, cliaddr;
    servaddr.sin_port = port;
    int max = 512;
    unsigned char buffer[512];

    // Do we have a command line arguments?
    if (argc != 3) {
        fprintf(stderr, "usage: %s server-ipdomain-name\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Create a socket.
    if ((socket_handle = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Unable to create socket");
        return EXIT_FAILURE;
    }

    // TODO: The bulk of the program goes here...

    //function to construct a request packet
    buffer[0] = 0x00;
    buffer[1] = 0x01;
    buffer[2] = 0x01;
    buffer[3] = 0x00;
    buffer[4] = 0x00;
    buffer[5] = 0x01;
    buffer[6] = 0x00;
    buffer[7] = 0x00;
    buffer[8] = 0x00;
    buffer[9] = 0x00;
    buffer[10] = 0x00;
    buffer[11] = 0x00;

    unsigned char *p = install_dname(&buffer[12], argv[2]);
    *p++ = 0x00;
    *p++ = 0x01;
    *p++ = 0x00;
    *p++ = 0x01;


    //function to send request
    int length = sizeof(buffer);
    if(sendto(socket_handle, buffer, length, 0, (struct sockaddr *) &servaddr,
              sizeof(servaddr)) == -1)
              {
                  perror("unable to send");
                  return -1;
              }
    //function that receives the response
    //TODO put in alarm()
    signal (SIGALRM, sig_alarm);
    alarm(5);
    int address_length = sizeof(struct sockaddr_in);
    int count;
    socklen_t clilen;
    clilen = sizeof(cliaddr);
    if((count = recvfrom(socket_handle, buffer, max, 0, (struct sockaddr *) &cliaddr,
                        &clilen)) < 0)
                        {
                            if(errno == EINTR)
                            {
                                fprintf(stderr, "socket timeout\n");
                            }
                            else
                            {
                                perror ("error during packet receive");
                                return -2;
                            }
                        }

    //function that disects the response looking for the answer
    p = buffer;
    p += 12;
    p += 2 + strlen(argv[2]) + 4;
    if((*p & 0xC0) == 0xC0)
    {
        p += 12;
    }
    else
    {
        p += 2 + strlen(argv[2]) + 10;
    }
    for(int i = 0; i < count; i++)
    {
        printf("%d", buffer[i]);
    }
    printf("\n");

    // Close the socket to clean up.
    close(socket_handle);
    return EXIT_SUCCESS;
}
