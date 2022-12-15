
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <bits/stdc++.h>
#include <vector>

struct symInfo
{
    double probx;
    double Fx;
    char *binarycode;
    int argv2;
    char *argv1;
    char *argv0;
    int argc;
};

void *Binary(void *arg)
{
    symInfo *Info = (symInfo *)arg;
    int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    if (Info->argc < 3)
    {
        std::cerr << "usage " << Info->argv0 << "hostname port\n";
        exit(0);
    }
    portno = Info->argv2;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        std::cerr << "ERROR opening socket";
    server = gethostbyname(Info->argv1);
    if (server == NULL)
    {
        std::cerr << "ERROR, no such host\n";
        exit(0);
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "ERROR connecting";
        exit(1);
    }

    double px = Info->probx;
    double fx = Info->Fx;

    n = write(sockfd, &px, sizeof(double));
    if (n < 0)
    {
        std::cerr << "ERROR writing to socket";
        exit(1);
    }

    n = write(sockfd, &fx, sizeof(double));
    if (n < 0)
    {
        std::cerr << "ERROR writing to socket";
        exit(1);
    }

    int size;
    n = read(sockfd, &size, sizeof(int));
    if (n < 0)
    {
        std::cerr << "ERROR reading from socket";
        exit(1);
    }
    char *buffer = new char[size + 1];
    bzero(buffer, size + 1);
    n = read(sockfd, buffer, size);
    Info->binarycode = buffer;
    // delete[] buffer;
    close(sockfd);
    return 0;
};

int main(int argc, char *argv[])
{
    symInfo SymbolInfo;
    std::vector<char> Symbols;
    std::vector<double> Probs;
    std::vector<double> cumul;
    std::vector<symInfo> info;
    std::string st;
    getline(std::cin, st);
    double Fx = 0.0; 

    // checking frequency of each character in ASCII Table
    // ASCII has 256 in modern systems
    for (int i = 1; i < 256; i++)
    {

        int ch = (char)i;
        char symbols = (char)i;
        // inbuilt function to calculate count
        double freq = std::count(st.begin(), st.end(), ch);
        // find probabilities
        if (freq > 0)
        {
            float prob = freq / st.length();
            Probs.push_back(prob);
            Symbols.push_back(symbols);
        }
    }
    //Pass info to struct
    for (int i = 0; i < Probs.size(); i++)
    {
        Fx += Probs[i];
        cumul.push_back(Fx);
        // Fbarx = (Fx - Probs[i]) + 0.5 * Probs[i];
        SymbolInfo.probx = Probs[i];
        SymbolInfo.Fx = cumul[i];
        SymbolInfo.argv2 = atoi(argv[2]);
        SymbolInfo.argv1 = argv[1];
        SymbolInfo.argv0 = argv[0];
        SymbolInfo.argc = argc;
        info.push_back(SymbolInfo); // store struct in vector of structs
    }

    std::cout << "SHANNON-FANO-ELIAS Codes:\n\n";
    pthread_t t[Symbols.size()];
    for (int i = 0; i < Symbols.size(); i++)
    {
        if (pthread_create(&t[i], NULL, &Binary, &info[i]) != 0)
        {
            std::cerr << "Cannot create thread";
        }
    }

    for (int i = 0; i < Symbols.size(); i++)
    {
        pthread_join(t[i], NULL);
        // print out the final output
        std::cout << "Symbol " << Symbols[i] << ", Code: " << info[i].binarycode << std::endl;
    }

    return 0;
}
