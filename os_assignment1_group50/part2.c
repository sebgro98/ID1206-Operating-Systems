#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h> 
// Part 2: Message queues
//This part defines a structure msg_buffer to hold messages in the message queue. It includes a message type and a text field.
struct msg_buffer {
    long msg_type;
    char msg_text[100];
};

//This defines a function part2 for the second part of the task. It generates a key for the message queue using ftok and creates a message queue using msgget.
void part2() {
    key_t key = ftok("message_queue_key", 65);
    int msgid = msgget(key, 0666 | IPC_CREAT);
    //Similar to part 1, this creates a child process using fork.
    pid_t pid = fork();

    //In the child process, it reads a message from the message queue using msgrcv, counts the number of words in the received text using strtok, and prints the word count.
    if (pid == 0) {
        // Child process (reader)
        struct msg_buffer message;
        msgrcv(msgid, &message, sizeof(message), 1, 0);

        int word_count = 0;
        char *token = strtok(message.msg_text, " ");
        while (token != NULL) {
            word_count++;
            token = strtok(NULL, " ");
        }

        printf("Word count in the file: %d\n", word_count);
        exit(EXIT_SUCCESS);
    
    //In the parent process, it opens a file (sample.txt), reads its content, sends the content through the message queue using msgsnd, and then waits for the child process to finish.
    } else if (pid > 0) {
        // Parent process (writer)
        FILE *file = fopen("sample.txt", "r");
        if (file == NULL) {
            perror("fopen");
            exit(EXIT_FAILURE);
        }

        struct msg_buffer message;
        message.msg_type = 1;
        fread(message.msg_text, sizeof(char), sizeof(message.msg_text), file);

        msgsnd(msgid, &message, sizeof(message), 0);
        fclose(file);

        wait(NULL); // Wait for the child process to finish
        exit(EXIT_SUCCESS);
    } else {
        perror("fork");
        exit(EXIT_FAILURE);
    }
}


/*The main function calls both part1 and part2 functions to execute the tasks. The output includes the result of the ls / | wc -l
 command and the word count from reading a file and passing the content through a message queue.*/
int main() {
      printf("\nPart 2 - Message queues:\n");
    part2(); 

    return 0;
}

