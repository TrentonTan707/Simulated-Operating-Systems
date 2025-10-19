#include <stdio.h>
#include <stdlib.h>
#include "oslabs.h"
#include <stdbool.h>
#include <limits.h>

int nullPTE(struct PTE x){
    if (x.is_valid == 0 && x.frame_number == 0 && x.arrival_timestamp == 0 &&
        x.last_access_timestamp == 0 && x.reference_count == 0){
        return 1;
    }
    else
        return 0;
}

//Helper function, checks if page_number is valid in page_table
int validPage(struct PTE page_table[TABLEMAX], int page_number, int current_timestamp){
    if(page_table[page_number].is_valid == 1){
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count++;
        return page_table[page_number].frame_number;
    }
    return -1;
}
//Helper function, check frame pool, 
int freeFrame(int frame_pool[POOLMAX],int *frame_cnt){
    int freeFrameNum = -1;
    if(*frame_cnt > 0){
        freeFrameNum = frame_pool[0];
    }
    if(freeFrameNum != -1){
        for(int i = 0; i < *frame_cnt-1; i++){
            frame_pool[i] = frame_pool[i+1];
        }
        (*frame_cnt)--;

    }
    return freeFrameNum;
}

int process_page_access_fifo(struct PTE page_table[TABLEMAX],int *table_cnt, int page_number, int frame_pool[POOLMAX],int *frame_cnt, int current_timestamp){

    int pageInMemory = validPage(page_table,page_number,current_timestamp);
    if (pageInMemory != -1){
        return pageInMemory;
    }
    int freeFrameNum = freeFrame(frame_pool,frame_cnt);
    if (freeFrameNum != -1){
        page_table[page_number].arrival_timestamp = current_timestamp;
        page_table[page_number].frame_number = freeFrameNum;
        page_table[page_number].is_valid = 1;
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count = 1;
        return freeFrameNum;
    }

    int smallestArrival = current_timestamp;
    int smallestArrivalIndex = 0;
    for(int i = 0; i < *table_cnt; i++){
        if(page_table[i].is_valid == 1 && page_table[i].arrival_timestamp < smallestArrival){
            smallestArrival = page_table[i].arrival_timestamp;
            smallestArrivalIndex = i;
        }
    }

    page_table[page_number].frame_number = page_table[smallestArrivalIndex].frame_number;
    page_table[page_number].arrival_timestamp = current_timestamp;
    page_table[page_number].last_access_timestamp = current_timestamp;
    page_table[page_number].is_valid = 1;
    page_table[page_number].reference_count = 1;

    page_table[smallestArrivalIndex].is_valid = 0;
    page_table[smallestArrivalIndex].frame_number = -1;
    page_table[smallestArrivalIndex].arrival_timestamp = -1;
    page_table[smallestArrivalIndex].last_access_timestamp = -1;
    page_table[smallestArrivalIndex].reference_count = -1;

    return page_table[page_number].frame_number;

}
int count_page_faults_fifo(struct PTE page_table[TABLEMAX],int table_cnt, int refrence_string[REFERENCEMAX],int reference_cnt,int frame_pool[POOLMAX],int frame_cnt){
    int timestamp = 1;
    int pageNum;
    int pageFaultCount=0;


    //loop through pages in reference string array
    for(int i = 0; i < reference_cnt; i++){
        //increment timestamp
        timestamp++;
        pageNum = refrence_string[i];
//        printf("pageNum %d\n",pageNum);   
        //if the current page is already in memory
        if(page_table[pageNum].is_valid == 1){
            //modify last access time and reference count
            page_table[pageNum].last_access_timestamp = timestamp;
            page_table[pageNum].reference_count++;
        }
        //if current page is not in memory
        else{
            //increment page fault count since page is not in memory
            pageFaultCount++;
//            printf("pageFaultCount %d\n",pageFaultCount);      
            int freeFrameNum = -1;
            //if there is a free frame in the frame pool
            if(frame_cnt > 0){
                //set freeFrameNum to first free frame in frame pool
                freeFrameNum = frame_pool[0];
                for(int j = 0; j < frame_cnt-1; j++){
                    //shift all free frames in frame pool
                    frame_pool[j] = frame_pool[j+1];
                }
                //decrement frame count because one was just removed
                (frame_cnt)--;
                //frame number is inserted into page table and has its fields set
                page_table[pageNum].arrival_timestamp = timestamp;
                page_table[pageNum].frame_number = freeFrameNum;
                page_table[pageNum].is_valid = 1;
                page_table[pageNum].last_access_timestamp = timestamp;
                page_table[pageNum].reference_count = 1;
            }
            //if no free frames in frame pool --> fifo logic
            else{
                //initialize smallestArrival and smallestArrivalIndex
                int smallestArrival = INT_MAX;
//                printf("initial smallestArrival %d\n",smallestArrival);            
                int smallestArrivalIndex = 0;
//                printf("initial smallestArrivalIndex %d\n",smallestArrivalIndex);      
                //loop through all pages in table
                for(int k = 0; k < table_cnt; k++){
                    //if page table is valid and page's arrival timestamp is < smallestArrival then update initialized variables
                    if(page_table[k].is_valid == 1 && page_table[k].arrival_timestamp < smallestArrival){
                        smallestArrival = page_table[k].arrival_timestamp;
                        smallestArrivalIndex = k;
//                        printf("smallestArrival %d\n",smallestArrival);      
//                        printf("smallestArrivalIndex %d\n",smallestArrivalIndex);      
                    }
                }
                //set fields for new page in page table 
                page_table[pageNum].frame_number = page_table[smallestArrivalIndex].frame_number;
                page_table[pageNum].arrival_timestamp = timestamp;
                page_table[pageNum].last_access_timestamp = timestamp;
                page_table[pageNum].is_valid = 1;
                page_table[pageNum].reference_count = 1;
                //set fields for page being removed from page table
                page_table[smallestArrivalIndex].is_valid = 0;
                page_table[smallestArrivalIndex].frame_number = -1;
                page_table[smallestArrivalIndex].arrival_timestamp = -1;
                page_table[smallestArrivalIndex].last_access_timestamp = -1;
                page_table[smallestArrivalIndex].reference_count = -1;
            }
        }

//        printf("timestamp: %d\n",timestamp);
    }
    //return final count of page faults after looping through all entries in refernce sting array
    return pageFaultCount;
}
int process_page_access_lru(struct PTE page_table[TABLEMAX],int *table_cnt, int page_number, int frame_pool[POOLMAX],int *frame_cnt, int current_timestamp){

    int pageInMemory = validPage(page_table,page_number,current_timestamp);
    if (pageInMemory != -1){
        return pageInMemory;
    }
    int freeFrameNum = freeFrame(frame_pool,frame_cnt);
    if (freeFrameNum != -1){
        page_table[page_number].arrival_timestamp = current_timestamp;
        page_table[page_number].frame_number = freeFrameNum;
        page_table[page_number].is_valid = 1;
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count = 1;
        return freeFrameNum;
    }

    int lastUsed = current_timestamp;
    int lastUsedIndex = 0;
    for(int i = 0; i < *table_cnt; i++){
        if(page_table[i].is_valid == 1 && page_table[i].last_access_timestamp < lastUsed){
            lastUsed = page_table[i].last_access_timestamp;
            lastUsedIndex = i;
        }
    }

    page_table[page_number].frame_number = page_table[lastUsedIndex].frame_number;
    page_table[page_number].arrival_timestamp = current_timestamp;
    page_table[page_number].last_access_timestamp = current_timestamp;
    page_table[page_number].is_valid = 1;
    page_table[page_number].reference_count = 1;

    page_table[lastUsedIndex].is_valid = 0;
    page_table[lastUsedIndex].frame_number = -1;
    page_table[lastUsedIndex].arrival_timestamp = -1;
    page_table[lastUsedIndex].last_access_timestamp = -1;
    page_table[lastUsedIndex].reference_count = -1;

    return page_table[page_number].frame_number;

}
int count_page_faults_lru(struct PTE page_table[TABLEMAX],int table_cnt, int refrence_string[REFERENCEMAX],int reference_cnt,int frame_pool[POOLMAX],int frame_cnt){
    int timestamp = 1;
    int pageNum;
    int pageFaultCount=0;


    //loop through pages in reference string array
    for(int i = 0; i < reference_cnt; i++){
        //increment timestamp
        timestamp++;
        pageNum = refrence_string[i];
//        printf("pageNum %d\n",pageNum);   
        //if the current page is already in memory
        if(page_table[pageNum].is_valid == 1){
            //modify last access time and reference count
            page_table[pageNum].last_access_timestamp = timestamp;
            page_table[pageNum].reference_count++;
        }
        //if current page is not in memory
        else{
            //increment page fault count since page is not in memory
            pageFaultCount++;
//            printf("pageFaultCount %d\n",pageFaultCount);      
            int freeFrameNum = -1;
            //if there is a free frame in the frame pool
            if(frame_cnt > 0){
                //set freeFrameNum to first free frame in frame pool
                freeFrameNum = frame_pool[0];
                for(int j = 0; j < frame_cnt-1; j++){
                    //shift all free frames in frame pool
                    frame_pool[j] = frame_pool[j+1];
                }
                //decrement frame count because one was just removed
                (frame_cnt)--;
                //frame number is inserted into page table and has its fields set
                page_table[pageNum].arrival_timestamp = timestamp;
                page_table[pageNum].frame_number = freeFrameNum;
                page_table[pageNum].is_valid = 1;
                page_table[pageNum].last_access_timestamp = timestamp;
                page_table[pageNum].reference_count = 1;
            }
            //if no free frames in frame pool --> fifo logic
            else{
                int lastUsed = INT_MAX;
                int lastUsedIndex;
                for(int i = 0; i < table_cnt; i++){
                    if(page_table[i].is_valid == 1 && page_table[i].last_access_timestamp < lastUsed){
                        lastUsed = page_table[i].last_access_timestamp;
                        lastUsedIndex = i;
                    }
                }
                //set fields for new page in page table 
                page_table[pageNum].frame_number = page_table[lastUsedIndex].frame_number;
                page_table[pageNum].arrival_timestamp = timestamp;
                page_table[pageNum].last_access_timestamp = timestamp;
                page_table[pageNum].is_valid = 1;
                page_table[pageNum].reference_count = 1;
                //set fields for page being removed from page table
                page_table[lastUsedIndex].is_valid = 0;
                page_table[lastUsedIndex].frame_number = -1;
                page_table[lastUsedIndex].arrival_timestamp = -1;
                page_table[lastUsedIndex].last_access_timestamp = -1;
                page_table[lastUsedIndex].reference_count = -1;
            }
        }

//        printf("timestamp: %d\n",timestamp);
    }
    //return final count of page faults after looping through all entries in refernce sting array
    return pageFaultCount;
}
int process_page_access_lfu(struct PTE page_table[TABLEMAX],int *table_cnt, int page_number, int frame_pool[POOLMAX],int *frame_cnt, int current_timestamp){

    int pageInMemory = validPage(page_table,page_number,current_timestamp);
    if (pageInMemory != -1){
        return pageInMemory;
    }
    int freeFrameNum = freeFrame(frame_pool,frame_cnt);
    if (freeFrameNum != -1){
        page_table[page_number].arrival_timestamp = current_timestamp;
        page_table[page_number].frame_number = freeFrameNum;
        page_table[page_number].is_valid = 1;
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count = 1;
        return freeFrameNum;
    }

    int leastUsed = INT_MAX;
    int leastUsedIndex;
    int earliestArrival = INT_MAX;
    for(int i = 0; i < *table_cnt; i++){

        if(page_table[i].is_valid == 1){
            if((page_table[i].reference_count < leastUsed) ||
              (page_table[i].reference_count == leastUsed && page_table[i].arrival_timestamp < earliestArrival)){
                leastUsed = page_table[i].reference_count;
                earliestArrival = page_table[i].arrival_timestamp;
                leastUsedIndex = i;
            }
        }
    }

    page_table[page_number].frame_number = page_table[leastUsedIndex].frame_number;
    page_table[page_number].arrival_timestamp = current_timestamp;
    page_table[page_number].last_access_timestamp = current_timestamp;
    page_table[page_number].is_valid = 1;
    page_table[page_number].reference_count = 1;

    page_table[leastUsedIndex].is_valid = 0;
    page_table[leastUsedIndex].frame_number = -1;
    page_table[leastUsedIndex].arrival_timestamp = -1;
    page_table[leastUsedIndex].last_access_timestamp = -1;
    page_table[leastUsedIndex].reference_count = -1;

    return page_table[page_number].frame_number;

}
int count_page_faults_lfu(struct PTE page_table[TABLEMAX],int table_cnt, int refrence_string[REFERENCEMAX],int reference_cnt,int frame_pool[POOLMAX],int frame_cnt){
    int timestamp = 1;
    int pageNum;
    int pageFaultCount=0;


    //loop through pages in reference string array
    for(int i = 0; i < reference_cnt; i++){
        //increment timestamp
        timestamp++;
        pageNum = refrence_string[i];
//        printf("pageNum %d\n",pageNum);   
        //if the current page is already in memory
        if(page_table[pageNum].is_valid == 1){
            //modify last access time and reference count
            page_table[pageNum].last_access_timestamp = timestamp;
            page_table[pageNum].reference_count++;
        }
        //if current page is not in memory
        else{
            //increment page fault count since page is not in memory
            pageFaultCount++;
//            printf("pageFaultCount %d\n",pageFaultCount);      
            int freeFrameNum = -1;
            //if there is a free frame in the frame pool
            if(frame_cnt > 0){
                //set freeFrameNum to first free frame in frame pool
                freeFrameNum = frame_pool[0];
                for(int j = 0; j < frame_cnt-1; j++){
                    //shift all free frames in frame pool
                    frame_pool[j] = frame_pool[j+1];
                }
                //decrement frame count because one was just removed
                (frame_cnt)--;
                //frame number is inserted into page table and has its fields set
                page_table[pageNum].arrival_timestamp = timestamp;
                page_table[pageNum].frame_number = freeFrameNum;
                page_table[pageNum].is_valid = 1;
                page_table[pageNum].last_access_timestamp = timestamp;
                page_table[pageNum].reference_count = 1;
            }
            //if no free frames in frame pool --> fifo logic
            else{
                int leastUsed = INT_MAX;
                int leastUsedIndex;
                int earliestArrival = INT_MAX;
                for(int i = 0; i < table_cnt; i++){

                    if(page_table[i].is_valid == 1){
                        if((page_table[i].reference_count < leastUsed) ||
                        (page_table[i].reference_count == leastUsed && page_table[i].arrival_timestamp < earliestArrival)){
                            leastUsed = page_table[i].reference_count;
                            earliestArrival = page_table[i].arrival_timestamp;
                            leastUsedIndex = i;
                        }
                    }
                }
                //set fields for new page in page table 
                page_table[pageNum].frame_number = page_table[leastUsedIndex].frame_number;
                page_table[pageNum].arrival_timestamp = timestamp;
                page_table[pageNum].last_access_timestamp = timestamp;
                page_table[pageNum].is_valid = 1;
                page_table[pageNum].reference_count = 1;
                //set fields for page being removed from page table
                page_table[leastUsedIndex].is_valid = 0;
                page_table[leastUsedIndex].frame_number = -1;
                page_table[leastUsedIndex].arrival_timestamp = -1;
                page_table[leastUsedIndex].last_access_timestamp = -1;
                page_table[leastUsedIndex].reference_count = -1;
            }
        }

//        printf("timestamp: %d\n",timestamp);
    }
    //return final count of page faults after looping through all entries in refernce sting array
    return pageFaultCount;
}