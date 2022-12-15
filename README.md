# Create a multithreaded Shannon-Fano-Elias encoder (https://en.wikipedia.org/wiki/Shannon–Fano–Elias_coding)
* Given the symbols with their probabilities (sorted in decreasing order based on the probability value), you need to implement the Shannon-Fano-Elias encoding algorithm based on the following steps:
1. Read the input from STDIN.
2. Create n POSIX threads (where n is the number of symbols to encode). Each child thread executes the following tasks:
      - Receives the probabilities of the symbols from the main thread.

      - Implements the Shannon-Fano-Elias encoding algorithm to determine the code for the assigned symbol.

      - Stores the code on a memory location accessible by the main thread.

3. Print the Shannon-Fano-Elias codes for the symbols from the input file.
