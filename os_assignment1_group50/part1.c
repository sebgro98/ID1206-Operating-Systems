#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h> 

/*These lines include necessary header files for standard input/output, process handling, inter-process communication (IPC), and message queues.*/
// Part 1: ls / | wc -l
//This defines a function part1 that represents the first part of the task. It declares an array pipe_fd to store file descriptors for a pipe.
void part1() {
    int pipe_fd[2];
    pipe(pipe_fd);
    
    /*This creates a child process using the fork system call and stores the process ID (pid). The code within the if (pid == 0) 
    block will be executed by the child process, and the code within the else if (pid > 0) block will be executed by the parent process.*/
    pid_t pid = fork();

 //In the child process, it closes the read end of the pipe, redirects the standard output to the write end of the pipe using dup2, and then executes the ls / command using execlp.
    if (pid == 0) {
        // Child process
        close(pipe_fd[0]); // Close read end of the pipe
        dup2(pipe_fd[1], STDOUT_FILENO); // Redirect stdout to the pipe write end
        close(pipe_fd[1]); // Close pipe write end

        // Execute ls /
        execlp("ls", "ls", "/", NULL);
        perror("exec");
        exit(EXIT_FAILURE);
    
    //In the parent process, it closes the write end of the pipe, redirects the standard input to the read end of the pipe using dup2, and then executes the wc -l command using execlp.
    } else if (pid > 0) {
        // Parent process
        close(pipe_fd[1]); // Close write end of the pipe
        dup2(pipe_fd[0], STDIN_FILENO); // Redirect stdin to the pipe read end
        close(pipe_fd[0]); // Close pipe read end

        // Execute wc -l
        execlp("wc", "wc", "-l", NULL);
        perror("exec");
        exit(EXIT_FAILURE);
    } else {
        perror("fork");
        exit(EXIT_FAILURE);
    }
}


/*The main function calls both part1 and part2 functions to execute the tasks. The output includes the result of the ls / | wc -l
 command and the word count from reading a file and passing the content through a message queue.*/
int main() {
    
  printf("Part 1 - ls / | wc -l:\n");
    part1();

  
  

    return 0;
}

