This C++ program is designed to process sentences from an input file by:

## Reading sentences from the file.
Sorting the words within each sentence alphabetically.
Writing the sorted sentences to an output file.
The program leverages multithreading for efficiency, where one thread reads and extracts sentences, another thread sorts the words in each sentence, and a third thread writes the sorted sentences to the output file.

### Requirements
C++ Compiler: The code requires a C++11 (or higher) compatible compiler.
Standard Libraries: The code uses standard C++ libraries such as `<iostream>`, `<fstream>`, `<thread>`, `<queue>`, `<string>`, `<vector>`, `<algorithm>`, `<mutex>`, `<condition_variable>`, and `<cctype>`.

## To compile and run the program, follow these steps:
1.Compile the Code:\
`g++ test.cpp -o result`\
2.Run the Program:\
`./result input.txt output.txt`

## How It Works
1. Reading (readFile Function):
The program reads lines from the input file.
Each line is split into sentences using the period (.) as a delimiter.
The extracted sentences are pushed into a queue (q1) shared with the sorting thread.
2. Sorting (sortLines Function):
The sorting thread retrieves sentences from the queue (q1).
It splits each sentence into words and sorts them alphabetically in a case-insensitive manner.
The sorted sentence is then pushed into another queue (q2) shared with the writing thread.
3. Writing (writeFile Function):
The writing thread retrieves sorted sentences from the queue (q2).
Each sentence is written to the output file.

### Note:
- Output.txt here is empty, but when we run our program this will have our expected result.

