
// Libraries
#include <iostream>
#include <cmath>
#include <vector>
#include <sstream>
#include <pthread.h>

using namespace std;

struct CodeVals
{
    double px, Fx, Fbarx;
    string binarycode;
};

void *Binary(void *arg)
{
    CodeVals *code = (CodeVals *)arg;                       // Type cast
    code->Fbarx = (code->Fx - code->px) + (0.5 * code->px); // calculate Fbarx
    int length = ceil(log2(1 / (code->px))) + 1;            // Find length of binarycode

    // initialize binarycode with strings of 0
    string str(length, '0');
    code->binarycode = str;

    // loop to find binarycode
    int i;
    double j;
    for (i = 0, j = 0.5; i < length; i++, j /= 2)
    {
        if (j > code->Fbarx)
            continue;
        code->binarycode[i] = '1';
        code->Fbarx -= j;
    }
    return 0;
}

int main()
{

    // declare variables
    vector<string> Symbols;
    vector<double> Numbers;
    vector<CodeVals> algo;
    CodeVals codes;
    double Fx = 0.0;

    // read the input and store
    string symbols, numbers;
    getline(cin, symbols);
    getline(cin, numbers);

    // Using stringstream to split symbols, numbers and store to vectors
    string sym, num;
    stringstream S(symbols), N(numbers);
    while (S >> sym && N >> num)
    {
        Symbols.push_back(sym);
        Numbers.push_back(stod(num));
    }

    // calculate Fx
    for (int i = 0; i < Numbers.size(); i++)
    {
        codes.px = Numbers[i];
        Fx += Numbers[i];
        codes.Fx = Fx;
        algo.push_back(codes);
    }

    cout << "SHANNON-FANO-ELIAS Codes:\n\n";

    pthread_t tid[Symbols.size()];
    // Create threads to implement the S-F-E algorithm for each probabilities
    for (int i = 0; i < Numbers.size(); i++)
    {
        if (pthread_create(&tid[i], NULL, &Binary, &algo[i]) != 0)
        {
            cerr << "Cannot create thread";
        }
    }
    // Create pthread_join to wait for child threads to finish
    for (int i = 0; i < Numbers.size(); i++)
    {
        pthread_join(tid[i], NULL);
        // print out the final output
        cout << "Symbol " << Symbols[i] << ", Code: " << algo[i].binarycode << endl;
    }
    return 0;
}
