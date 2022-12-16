
// Libraries
#include <iostream>
#include <cmath>
#include <vector>
#include <pthread.h>
#include <bits/stdc++.h>
#include <unistd.h>

using namespace std;

struct Prob
{
    int *turn = 0;
    pthread_mutex_t *bsem;
    pthread_cond_t *mywait;
    char Symbols;
    double *Probs;
    int currentIndex = 0;
};

void *Binary(void *arg)
{
    Prob temp = *(Prob *)arg; // copy struct to local var
    double *Px = (temp.Probs);
    pthread_mutex_unlock(temp.bsem);

    double Fx = 0.0;

    for (int i = 0; i <= temp.currentIndex; i++)
    {
        // calc
        Fx += *(Px + i);
    }

    double Fbarx = 0.0;
    Fbarx = (Fx - *(Px + temp.currentIndex)) + (0.5 * (*(Px + temp.currentIndex))); // calculate Fbarx
    int length = ceil(log2(1 / (*(Px + temp.currentIndex)))) + 1;                   // Find length of binarycode

    // initialize binarycode with strings of 0
    string str(length, '0');
    //  for loop to find binarycode
    int i;
    double j;
    for (i = 0, j = 0.5; i < length; i++, j /= 2)
    {
        if (j > Fbarx)
            continue;
        str[i] = '1';
        Fbarx -= j;
    }
    pthread_mutex_lock(temp.bsem);
    // tracking which symbol's turn
    while (*(temp.turn) != temp.currentIndex)
    {
        pthread_cond_wait(temp.mywait, temp.bsem);
    }
    pthread_mutex_unlock(temp.bsem);
    cout << "Symbol " << temp.Symbols << ", Code: " << str << endl;
    pthread_mutex_lock(temp.bsem);
    (*temp.turn)++;
    pthread_cond_broadcast(temp.mywait);
    pthread_mutex_unlock(temp.bsem);

    return 0;
}

int main()
{
    Prob probStruct;
    pthread_mutex_t sema = PTHREAD_MUTEX_INITIALIZER;
    probStruct.bsem = &sema;
    pthread_cond_t w = PTHREAD_COND_INITIALIZER;
    probStruct.mywait = &w;
    int turn = 0;
    probStruct.turn = &turn;
    double Fx = 0.0;
    vector<char> Symbols;
    vector<double> tempprob;

    // read the input and store
    string st;
    cin >> st;

    // checking frequency of each character in ASCII Table
    // ASCII has 256 in modern systems
    for (int i = 1; i < 256; i++)
    {
        int ch = (char)i;
        char symbols = (char)i;
        // inbuilt function to calculate frequency
        double freq = std::count(st.begin(), st.end(), ch);
        // find probabilities
        if (freq > 0)
        {
            double prob = freq / st.length();
            tempprob.push_back(prob);
            // Probs[tempindex++] = prob;
            Symbols.push_back(symbols);
        }
    }

    probStruct.Probs = tempprob.data();
    cout << "SHANNON-FANO-ELIAS Codes:\n\n";

    // Create threads to implement the S-F-E algorithm in parallel for each probabilities
    pthread_t thread[Symbols.size()];

    for (int i = 0; i < Symbols.size(); i++)
    {
        pthread_mutex_lock(probStruct.bsem);
        probStruct.currentIndex = i;
        probStruct.Symbols = Symbols[i];

        if (pthread_create(&thread[i], NULL, &Binary, &probStruct) != 0)
        {
            cerr << "Cannot create thread";
        }
    }

    // Create pthread_join to wait for child threads to finish
    for (int i = 0; i < Symbols.size(); i++)
    {
        pthread_join(thread[i], NULL);
    }

    return 0;
}
