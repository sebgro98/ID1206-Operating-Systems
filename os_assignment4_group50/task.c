#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define CYLINDERS 5000
#define REQUESTS 1000


void fcfs(int sequence[REQUESTS], int initial_position) {
    int total_head_movement = 0;
    int current_position = initial_position;

    for (int i = 0; i < REQUESTS; i++) {
        total_head_movement += abs(current_position - sequence[i]);
        current_position = sequence[i];
    }

    printf("FCFS Total Head Movement: %d\n", total_head_movement);
}

void sstf(int sequence[REQUESTS], int initial_position) {
    int total_head_movement = 0;
    int current_position = initial_position;

    for (int i = 0; i < REQUESTS; i++) {
        int min_distance = abs(current_position - sequence[i]);
        int index = i;

        for (int j = i + 1; j < REQUESTS; j++) {
            int distance = abs(current_position - sequence[j]);
            if (distance < min_distance) {
                min_distance = distance;
                index = j;
            }
        }

        total_head_movement += min_distance;
        current_position = sequence[index];

        // Swap selected request with the current position
        int temp = sequence[i];
        sequence[i] = sequence[index];
        sequence[index] = temp;
    }

    printf("SSTF Total Head Movement: %d\n", total_head_movement);
}

void scan(int sequence[REQUESTS], int initial_position) {
    int total_head_movement = 0;
    int current_position = initial_position;

    // Sort the sequence
    for (int i = 0; i < REQUESTS - 1; i++) {
        for (int j = 0; j < REQUESTS - i - 1; j++) {
            if (sequence[j] > sequence[j + 1]) {
                // Swap
                int temp = sequence[j];
                sequence[j] = sequence[j + 1];
                sequence[j + 1] = temp;
            }
        }
    }

    // Move left
    for (int i = 0; i < REQUESTS; i++) {
        if (sequence[i] >= current_position) {
            break;
        }
        total_head_movement += abs(current_position - sequence[i]);
        current_position = sequence[i];
    }

    // Move right
    for (int i = REQUESTS - 1; i >= 0; i--) {
        total_head_movement += abs(current_position - sequence[i]);
        current_position = sequence[i];
    }

    printf("SCAN Total Head Movement: %d\n", total_head_movement);
}

void c_scan(int sequence[REQUESTS], int initial_position) {
    int total_head_movement = 0;
    int current_position = initial_position;

    // Sort the sequence
    for (int i = 0; i < REQUESTS - 1; i++) {
        for (int j = 0; j < REQUESTS - i - 1; j++) {
            if (sequence[j] > sequence[j + 1]) {
                // Swap
                int temp = sequence[j];
                sequence[j] = sequence[j + 1];
                sequence[j + 1] = temp;
            }
        }
    }

    // Move right
    for (int i = 0; i < REQUESTS; i++) {
        if (sequence[i] > current_position) {
            total_head_movement += abs(current_position - sequence[i]);
            current_position = sequence[i];
        }
    }

    // Move to the end of the disk, then move towards the initial position
    total_head_movement += abs(current_position - CYLINDERS);
    total_head_movement += abs(0 - CYLINDERS); // Include the gap between the end and the beginning
    current_position = 0;

    // Move right again
    for (int i = 0; i < REQUESTS; i++) {
        total_head_movement += abs(current_position - sequence[i]);
        current_position = sequence[i];
    }

    printf("C-SCAN Total Head Movement: %d\n", total_head_movement);
}


void look(int sequence[REQUESTS], int initial_position) {
    int total_head_movement = 0;
    int current_position = initial_position;

    // Sort the sequence
    for (int i = 0; i < REQUESTS - 1; i++) {
        for (int j = 0; j < REQUESTS - i - 1; j++) {
            if (sequence[j] > sequence[j + 1]) {
                // Swap
                int temp = sequence[j];
                sequence[j] = sequence[j + 1];
                sequence[j + 1] = temp;
            }
        }
    }

    // Move left
    for (int i = 0; i < REQUESTS; i++) {
        if (sequence[i] >= current_position) {
            break;
        }
        total_head_movement += abs(current_position - sequence[i]);
        current_position = sequence[i];
    }

    // Move right
    for (int i = REQUESTS - 1; i >= 0; i--) {
        if (sequence[i] > current_position) {
            total_head_movement += abs(current_position - sequence[i]);
            current_position = sequence[i];
        }
    }

    printf("LOOK Total Head Movement: %d\n", total_head_movement);
}

void c_look(int sequence[REQUESTS], int initial_position) {
    int total_head_movement = 0;
    int current_position = initial_position;

    // Sort the sequence
    for (int i = 0; i < REQUESTS - 1; i++) {
        for (int j = 0; j < REQUESTS - i - 1; j++) {
            if (sequence[j] > sequence[j + 1]) {
                // Swap
                int temp = sequence[j];
                sequence[j] = sequence[j + 1];
                sequence[j + 1] = temp;
            }
        }
    }

    // Move right
    for (int i = 0; i < REQUESTS; i++) {
        if (sequence[i] > current_position) {
            total_head_movement += abs(current_position - sequence[i]);
            current_position = sequence[i];
        }
    }

    // Move to the end of the queue, then move towards the initial position
    for (int j = 0; j < REQUESTS; j++) {
        total_head_movement += abs(current_position - sequence[j]);
        current_position = sequence[j];
    }

    printf("C-LOOK Total Head Movement: %d\n", total_head_movement);
}




int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <initial_position>\n", argv[0]);
        return 1;
    }

    // Convert the initial position from the command-line argument to an integer
    int initial_position = atoi(argv[1]);

    // Seed the random number generator once at the beginning of the program
    srand(time(NULL));

    // Generate a random sequence of 1000 cylinder requests
    int cylinder_requests[REQUESTS];
    for (int i = 0; i < REQUESTS; i++) {
        cylinder_requests[i] = rand() % CYLINDERS;
    }

    // Run each disk scheduling algorithm
    fcfs(cylinder_requests, initial_position);
    sstf(cylinder_requests, initial_position);
    scan(cylinder_requests, initial_position);
    c_scan(cylinder_requests, initial_position);
    look(cylinder_requests, initial_position);
    c_look(cylinder_requests, initial_position);

    return 0;
}
