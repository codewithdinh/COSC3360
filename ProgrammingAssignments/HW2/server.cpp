

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <cmath>
#include <cstring>

void fireman(int)
{
    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    signal(SIGCHLD, fireman);
    if (argc < 2)
    {
        std::cerr << "ERROR, no port provided\n";
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "ERROR opening socket";
        exit(1);
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *)&serv_addr,
             sizeof(serv_addr)) < 0)
    {
        std::cerr << "ERROR on binding";
        exit(1);
    }
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    while (true)
    {
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
        if (fork() == 0)
        {
            if (newsockfd < 0)
            {
                std::cerr << "ERROR on accept";
                exit(1);
            }
            double px;
            n = read(newsockfd, &px, sizeof(double));
            if (n < 0)
            {
                std::cerr << "ERROR reading from socket";
                exit(1);
            }
            double fx;
            n = read(newsockfd, &fx, sizeof(double));
            if (n < 0)
            {
                std::cerr << "ERROR reading from socket";
                exit(1);
            }

            int length = ceil(log2(1 / px)) + 1; // Find length of binarycode
            // initialize binarycode with strings of 0
            std::string str(length, '0');
            std::string binarycode = str;
            // for loop to find binarycode

            double fbarx = 0.0;
            int i;
            double j;
            fbarx = (fx - px) + 0.5 * px;
            double k = fbarx;
            for (i = 0, j = 0.5; i < length; i++, j /= 2)
            {
                if (j > k)
                    continue;
                binarycode[i] = '1';
                k -= j;
            }
            const char *message = binarycode.c_str();
            int sMessage = strlen(message);
            n = write(newsockfd, &sMessage, sizeof(int));
            if (n < 0)
            {
                std::cerr << "ERROR writing to socket";
                exit(1);
            }

            n = write(newsockfd, message, sMessage);
            if (n < 0)
            {
                std::cerr << "ERROR writing to socket";
                exit(1);
            }
            close(newsockfd);
            // delete[] buffer;
            _exit(0);
        }
    }
    close(sockfd);
    return 0;
}
