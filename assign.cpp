#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstdlib>
#include <ctime>
#include <climits>


#define SIZE 20

using namespace std;

/*
Notes:
cout << " "; = print
Variables are declared with {#define variable value;}
endl = newLine
fork() returns: -1: fail, 1: parent, 0: child


*/

int main() {
    srand(time(0));
    int arr[SIZE];
    int pipefd[2];
    
    //This solved the following problem:
    //output would occasionally produce massively negative
    //numbers(-929374676590927)
    //initializes pipe!
    if(pipe(pipefd)==-1){
        cerr<< "Pipe creation failed"<< endl;
        return 1;
        
    }
    
    //will store process ID
    pid_t pid;
    
    //Randomly assign 20 values to array
    cout << "Array: ";
    for (int i = 0; i < SIZE; i++) {
        arr[i] = rand() % 100 + 1;
        
        //print array
        cout << arr[i] << " ";
    }
    cout << endl;
    
    //create child process
    pid = fork();
    
    if (pid < 0) {
        cerr << "Fork failed" << endl;
        return 1;
    }
    
    //CHILD PROCESS
    if (pid == 0) { 
        //close reading end
        close(pipefd[0]);
        
        //child min will be set to first element of the second half of the array
        int childMin = arr[SIZE/2];
        
        for (int i = SIZE / 2; i < SIZE; i++) {
            if (arr[i] < childMin) {
                childMin = arr[i];
            }
        }
        //send to the writing end of the pipe{address of minValue and size of array}
        write(pipefd[1], &childMin, sizeof(childMin));
        
        // close writing end
        close(pipefd[1]); 
        
        //print child process ID and min value
        cout << "Child Process (PID: " << getpid() << ") found minimum in second half: " << childMin << endl;
        exit(0);
    } else { 
        
        //PARENT PROCESS
        //close reading end again(just in case)
        close(pipefd[1]);
        
        //default min
        int parentMin = arr[0];
        for (int i = 0; i < SIZE / 2; i++) {
            if (arr[i] < parentMin) {
                parentMin = arr[i];
            }
        }
        int childMin;
        //read the child min sent from the pipe
        read(pipefd[0], &childMin, sizeof(childMin));
        
        close(pipefd[0]);
       //wait for child
        wait(NULL); 
        
        int overallMin;
        overallMin = (parentMin<childMin) ? parentMin : childMin;

        cout << "Parent Process (PID: " << getpid() << ") found minimum in first half: " << parentMin << endl;
        cout << "Overall Minimum in Array: " << overallMin << endl;
    }

    return 0;
}