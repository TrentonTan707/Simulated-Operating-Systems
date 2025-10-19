#include <stdio.h>
#include <stdlib.h>
#include "oslabs.h"
#include <stdbool.h>

 
int pcb_is_null(struct PCB x){
    if (x.arrival_timestamp == 0 && x.execution_endtime == 0 && x.execution_starttime == 0 &&
        x.process_id == 0 && x.process_priority == 0 && x.remaining_bursttime == 0 && x.total_bursttime == 0){
        return 1;
    }
    else
        return 0;
}

struct PCB handle_process_arrival_pp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process, struct PCB new_process, int timestamp){
    if(pcb_is_null(current_process)){
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;
        return new_process;
    }
    else if (new_process.process_priority >= current_process.process_priority){

        new_process.execution_starttime = 0;
        new_process.execution_endtime = 0;
        new_process.remaining_bursttime = new_process.total_bursttime;        
        ready_queue[*queue_cnt] = new_process;
        (*queue_cnt)++;        
        return current_process;
    }
    else{
        current_process.execution_endtime = 0;
        current_process.remaining_bursttime -= timestamp - current_process.execution_starttime;   
        ready_queue[*queue_cnt] = current_process;
        (*queue_cnt)++;        
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;   
        return new_process;
    }
}
struct PCB handle_process_completion_pp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int timestamp){
    struct PCB nullPCB = {0,0,0,0,0,0,0};
    if (*queue_cnt == 0){
        return nullPCB;
    }
    else{
        int highest_priority = ready_queue[0].process_priority;
        struct PCB returnPCB = ready_queue[0];
        int returnIndex = 0;
        for(int i = 1; i < *queue_cnt; i++){
            if(ready_queue[i].process_priority < highest_priority){
                highest_priority = ready_queue[i].process_priority;
                returnPCB = ready_queue[i];
                returnIndex = i;
            }
        }

        for(int j = returnIndex; j < *queue_cnt-1; j++){
            ready_queue[j] = ready_queue[j+1];
        }

        (*queue_cnt)--;

        returnPCB.execution_starttime = timestamp;
        returnPCB.execution_endtime = timestamp + returnPCB.remaining_bursttime;

        return returnPCB;
    }
    
}
struct PCB handle_process_arrival_srtp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process, struct PCB new_process, int time_stamp){
    if(pcb_is_null(current_process)){
        new_process.execution_starttime = time_stamp;
        new_process.execution_endtime = time_stamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;
        return new_process;
    }
    else if (current_process.remaining_bursttime <= new_process.total_bursttime){
        new_process.execution_starttime = 0;
        new_process.execution_endtime = 0;
        new_process.remaining_bursttime = new_process.total_bursttime;        
        ready_queue[*queue_cnt] = new_process;
        (*queue_cnt)++;        
        return current_process;        
    }
    else{
        new_process.execution_starttime = time_stamp;
        new_process.execution_endtime = time_stamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;

        current_process.remaining_bursttime = current_process.total_bursttime - (time_stamp-current_process.execution_starttime);
        current_process.execution_starttime = 0;
        current_process.execution_endtime = 0;
        ready_queue[*queue_cnt] = current_process;
        (*queue_cnt)++;      
        return new_process;
    }
}

struct PCB handle_process_completion_srtp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int timestamp){
    struct PCB nullPCB = {0,0,0,0,0,0,0};
    if (*queue_cnt == 0){
        return nullPCB;
    }
    else{
        int smallest_burst = ready_queue[0].remaining_bursttime;
        struct PCB returnPCB = ready_queue[0];
        int returnIndex = 0;
        for(int i = 1; i < *queue_cnt; i++){
            if(ready_queue[i].remaining_bursttime < smallest_burst){
                smallest_burst = ready_queue[i].remaining_bursttime;
                returnPCB = ready_queue[i];
                returnIndex = i;
            }
        }

        for(int j = returnIndex; j < *queue_cnt-1; j++){
            ready_queue[j] = ready_queue[j+1];
        }

        (*queue_cnt)--;

        returnPCB.execution_starttime = timestamp;
        returnPCB.execution_endtime = timestamp + returnPCB.remaining_bursttime;

        return returnPCB;
    }
}

struct PCB handle_process_arrival_rr(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process, struct PCB new_process, int timestamp, int time_quantum){
    if(pcb_is_null(current_process)){
        new_process.execution_starttime = timestamp;
        if (new_process.total_bursttime <= time_quantum){       
            new_process.execution_endtime = timestamp + new_process.total_bursttime;
        }
        else{
            new_process.execution_endtime = timestamp + time_quantum;
        }
        new_process.remaining_bursttime = new_process.total_bursttime;
        return new_process;
    }
    else{
        new_process.execution_starttime = 0;
        new_process.execution_endtime = 0;
        new_process.remaining_bursttime = new_process.total_bursttime;
        ready_queue[*queue_cnt] = new_process;
        (*queue_cnt)++;              
        return current_process;
    }
}

struct PCB handle_process_completion_rr(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int timestamp, int time_quantum){
    struct PCB nullPCB = {0,0,0,0,0,0,0};
    if (*queue_cnt == 0){
        return nullPCB;
    }
    else{
        int earliest_arrival = ready_queue[0].arrival_timestamp;
        struct PCB returnPCB = ready_queue[0];
        int returnIndex = 0;
        for(int i = 1; i < *queue_cnt; i++){
            if(ready_queue[i].arrival_timestamp < earliest_arrival){
                earliest_arrival = ready_queue[i].arrival_timestamp;
                returnPCB = ready_queue[i];
                returnIndex = i;
            }
        }
        
        for(int j = returnIndex; j < *queue_cnt-1; j++){
            ready_queue[j] = ready_queue[j+1];
        }

        (*queue_cnt)--;

        returnPCB.execution_starttime = timestamp;
        if(time_quantum < returnPCB.remaining_bursttime){
            returnPCB.execution_endtime = timestamp + time_quantum;
        }
        else{
            returnPCB.execution_endtime = timestamp + returnPCB.remaining_bursttime;
        }
        return returnPCB;
    }
}

