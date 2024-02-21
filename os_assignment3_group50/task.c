#include <stdio.h>
#include <stdlib.h>

#define TLB_SIZE 16
#define PAGE_TABLE_SIZE 256
#define PAGE_SIZE 256
#define FRAME_SIZE 256
#define MEMORY_SIZE 65536

typedef struct {
    int pageNumber;
    int frameNumber;
} TLBEntry;

int pageTable[PAGE_TABLE_SIZE][2];
TLBEntry TLB[TLB_SIZE];

char physicalMemory[MEMORY_SIZE];

FILE *backingStore;
FILE *virtualAddresses;

int pageFaults = 0;
int TLBHits = 0;
int addressesRead = 0;
int currentFrame = 0;


void updatePageTable(int pageNumber);
void updateTLB(int pageNumber, int frameNumber);
int checkTLB(int pageNumber);
void handleTLBHit(int virtualAddress, int TLBFrameNumber, int pageOffset);
void handleTLBMissPageTableHit(int virtualAddress, int pageNumber, int pageOffset);
void handlePageFault(int virtualAddress, int pageNumber, int pageOffset);

//main method this is where the program starts. 
int main(int argc, char *argv[]) {
    // Open the backing store and virtual addresses files
    backingStore = fopen("BACKING_STORE.bin", "rb");
    virtualAddresses = fopen(argv[1], "r");

    // Check if files were opened successfully
    if (!backingStore || !virtualAddresses) {
        perror("Error opening files");
        return 1;
    }

    // Initialize page table and TLB
    for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
        // Initialize the 'valid' bit of each entry in the page table
        pageTable[i][1] = 0;
    }

    for (int i = 0; i < TLB_SIZE; i++) {
        // Initialize TLB entries with invalid values
        TLB[i].pageNumber = -1;
        TLB[i].frameNumber = -1;
    }

    int virtualAddress;
    int pageNumber, pageOffset;

    // Main loop for processing virtual addresses
    while (fscanf(virtualAddresses, "%d", &virtualAddress) != EOF) {
        addressesRead++;

        // Extract page number and page offset from the virtual address
        pageNumber = (virtualAddress >> 8) & 0xFF;
        pageOffset = virtualAddress & 0xFF;

        // Check if page is in TLB
        int TLBFrameNumber = checkTLB(pageNumber);
         if (TLBFrameNumber != -1) {
            handleTLBHit(virtualAddress, TLBFrameNumber, pageOffset);
        } else if (pageTable[pageNumber][1] != 0) {
            handleTLBMissPageTableHit(virtualAddress, pageNumber, pageOffset);
        } else {
            handlePageFault(virtualAddress, pageNumber, pageOffset);
        }
    }

    // Print statistics
    float pageFaultRate = (float)pageFaults / addressesRead;
    float TLBHitRate = (float)TLBHits / addressesRead;
    printf("Page fault rate: %f\n", pageFaultRate);
    printf("TLB hit rate: %f\n", TLBHitRate);

    // Close the files
    fclose(backingStore);
    fclose(virtualAddresses);

    return 0;
}


// Function to handle TLB hit
void handleTLBHit(int virtualAddress, int TLBFrameNumber, int pageOffset) {
    printf("TLB Hit: Virtual Address: %d, Physical Address: %d, Value: %d\n", 
           virtualAddress, TLBFrameNumber << 8 | pageOffset, 
           (int8_t)physicalMemory[TLBFrameNumber << 8 | pageOffset]);
    TLBHits++;
}

// Function to handle TLB miss and page table hit
void handleTLBMissPageTableHit(int virtualAddress, int pageNumber, int pageOffset) {
    int pageTableFrameNumber = pageTable[pageNumber][0];
    printf("Page Table Hit (TLB Miss): Virtual Address: %d, Physical Address: %d, Value: %d\n", 
           virtualAddress, pageTableFrameNumber << 8 | pageOffset, 
           (int8_t)physicalMemory[pageTableFrameNumber << 8 | pageOffset]);
    updateTLB(pageNumber, pageTableFrameNumber);
}

// Function to handle page fault
void handlePageFault(int virtualAddress, int pageNumber, int pageOffset) {
    if (fseek(backingStore, pageNumber * PAGE_SIZE, SEEK_SET) != 0) {
        perror("fseek");
        exit(EXIT_FAILURE);
    }

    if (fread(&physicalMemory[currentFrame * FRAME_SIZE], sizeof(char), PAGE_SIZE, backingStore) != PAGE_SIZE) {
        perror("fread");
        exit(EXIT_FAILURE);
    }

    // Update page table and TLB
    updatePageTable(pageNumber);
    updateTLB(pageNumber, currentFrame);

    // Print the result of the memory access
    printf("Page Fault: Virtual Address: %d, Physical Address: %d, Value: %d\n", 
           virtualAddress, currentFrame << 8 | pageOffset, 
           (int8_t)physicalMemory[currentFrame << 8 | pageOffset]);

    // Increment the current frame and page fault count
    currentFrame++;
    pageFaults++;
}


void updateTLB(int pageNumber, int frameNumber) {
    for (int i = TLB_SIZE - 1; i > 0; i--) {
        TLB[i].pageNumber = TLB[i - 1].pageNumber;
        TLB[i].frameNumber = TLB[i - 1].frameNumber;
    }
    TLB[0].pageNumber = pageNumber;
    TLB[0].frameNumber = frameNumber;
}

// Function to update page table
void updatePageTable(int pageNumber) {
    pageTable[pageNumber][0] = currentFrame;
    pageTable[pageNumber][1] = 1; // Set the 'valid' bit for the page table entry
}

// Function to check TLB for a page number
int checkTLB(int pageNumber) {
    for (int i = 0; i < TLB_SIZE; i++) {
        if (TLB[i].pageNumber == pageNumber) {
            return TLB[i].frameNumber;
        }
    }
    return -1;
}

