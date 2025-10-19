#include <stdio.h>
#include <stdlib.h>
#include "oslabs.h"
#include <stdbool.h>



int rcb_is_null(struct RCB x){
    if (x.address == 0 && x.arrival_timestamp == 0 && x.cylinder == 0 &&
        x.process_id == 0 && x.request_id == 0){
        return 1;
    }
    else
        return 0;
}


struct RCB handle_request_arrival_fcfs(struct RCB request_queue[QUEUEMAX], int *queue_cnt, struct RCB current_request, struct RCB new_request, int timestamp){
    if(rcb_is_null(current_request)){
        return new_request;
    }
    else{
        request_queue[*queue_cnt] = new_request;
        (*queue_cnt)++;            
        return current_request;
    }
}
struct RCB handle_request_completion_fcfs(struct RCB request_queue[QUEUEMAX],int *queue_cnt){
    struct RCB nullRCB = {0,0,0,0,0};
    if (*queue_cnt == 0){
        return nullRCB;
    }
    else{
        int earliest_arrival = request_queue[0].arrival_timestamp;
        struct RCB returnRCB = request_queue[0];
        int returnIndex = 0;     
        for(int i = 1; i < *queue_cnt; i++){
            if(request_queue[i].arrival_timestamp < earliest_arrival){
                earliest_arrival = request_queue[i].arrival_timestamp;
                returnRCB = request_queue[i];
                returnIndex = i;
            }
        } 
        for(int j = returnIndex; j < *queue_cnt-1; j++){
            request_queue[j] = request_queue[j+1];
        }

        (*queue_cnt)--;   
        return returnRCB;       
    }
}
struct RCB handle_request_arrival_sstf(struct RCB request_queue[QUEUEMAX],int *queue_cnt, struct RCB current_request, struct RCB new_request, int timestamp){
    if(rcb_is_null(current_request)){
        return new_request;
    }
    else{
        request_queue[*queue_cnt] = new_request;
        (*queue_cnt)++;            
        return current_request;
    }    
}
struct RCB handle_request_completion_sstf(struct RCB request_queue[QUEUEMAX],int *queue_cnt,int current_cylinder){
    struct RCB nullRCB = {0,0,0,0,0};
    if (*queue_cnt == 0){
        return nullRCB;
    }
    else{
        int closest_cylinder = abs(request_queue[0].cylinder-current_cylinder);
        int earliest_arrival = request_queue[0].arrival_timestamp;
        struct RCB returnRCB = request_queue[0];
        int returnIndex = 0;     
        for(int i = 1; i < *queue_cnt; i++){
            if(abs(request_queue[i].cylinder-current_cylinder) < closest_cylinder || 
               abs(request_queue[i].cylinder-current_cylinder) == closest_cylinder && request_queue[i].arrival_timestamp < earliest_arrival){
                earliest_arrival = request_queue[i].arrival_timestamp;
                closest_cylinder = abs(request_queue[i].cylinder-current_cylinder);
                returnRCB = request_queue[i];
                returnIndex = i;
            }
        } 
        for(int j = returnIndex; j < *queue_cnt-1; j++){
            request_queue[j] = request_queue[j+1];
        }

        (*queue_cnt)--;   
        return returnRCB;       
    }    
}
struct RCB handle_request_arrival_look(struct RCB request_queue[QUEUEMAX],int *queue_cnt, struct RCB current_request, struct RCB new_request, int timestamp){
    if(rcb_is_null(current_request)){
        return new_request;
    }
    else{
        request_queue[*queue_cnt] = new_request;
        (*queue_cnt)++;            
        return current_request;
    }   
}
struct RCB handle_request_completion_look(struct RCB request_queue[QUEUEMAX],int *queue_cnt, int current_cylinder, int scan_direction){
    struct RCB nullRCB = {0,0,0,0,0};

    if (*queue_cnt == 0){
        return nullRCB;
    }
    int closest_cylinder = abs(request_queue[0].cylinder-current_cylinder);
    int earliest_arrival = request_queue[0].arrival_timestamp;
    struct RCB returnRCB = request_queue[0];
    int returnIndex = -1;  
    for(int i = 0; i < *queue_cnt; i++){
        if(request_queue[i].cylinder == current_cylinder){
            if(returnIndex == -1 || request_queue[i].arrival_timestamp < earliest_arrival){
                earliest_arrival = request_queue[i].arrival_timestamp;
                returnRCB = request_queue[i];
                returnIndex = i;                
            }
        }
    }
    if(returnIndex != -1){
        for(int j = returnIndex; j < *queue_cnt-1; j++){
            request_queue[j] = request_queue[j+1];
        }

        (*queue_cnt)--;   
        return returnRCB;          
    }
    if(scan_direction == 1){
        for(int i = 0; i < *queue_cnt; i++){
            if(request_queue[i].cylinder > current_cylinder){
                if(returnIndex == -1 || (abs(request_queue[i].cylinder-current_cylinder) < closest_cylinder) || 
                   ((abs(request_queue[i].cylinder-current_cylinder) == closest_cylinder) && (request_queue[i].arrival_timestamp < earliest_arrival))){
                    closest_cylinder = abs(request_queue[i].cylinder-current_cylinder);
                    earliest_arrival = request_queue[i].arrival_timestamp;
                    returnRCB = request_queue[i];
                    returnIndex = i;                
                }
            }
        }
        if(returnIndex == -1){
            for(int i = 0; i < *queue_cnt; i++){
                if(request_queue[i].cylinder < current_cylinder){
                    if(returnIndex == -1 || abs(request_queue[i].cylinder-current_cylinder) < closest_cylinder || 
                    ((abs(request_queue[i].cylinder-current_cylinder) == closest_cylinder) && (request_queue[i].arrival_timestamp < earliest_arrival))){
                        closest_cylinder = abs(request_queue[i].cylinder-current_cylinder);
                        earliest_arrival = request_queue[i].arrival_timestamp;
                        returnRCB = request_queue[i];
                        returnIndex = i;                
                    }
                }
            }            
        }
        if(returnIndex != -1){
            for(int j = returnIndex; j < *queue_cnt-1; j++){
                request_queue[j] = request_queue[j+1];
            }

            (*queue_cnt)--;   
            return returnRCB;          
        }        
    }
    else{
        for(int i = 0; i < *queue_cnt; i++){
            if(request_queue[i].cylinder < current_cylinder){
                if(returnIndex == -1 || abs(request_queue[i].cylinder-current_cylinder) < closest_cylinder || 
                   ((abs(request_queue[i].cylinder-current_cylinder) == closest_cylinder) && (request_queue[i].arrival_timestamp < earliest_arrival))){
                    closest_cylinder = abs(request_queue[i].cylinder-current_cylinder);
                    earliest_arrival = request_queue[i].arrival_timestamp;
                    returnRCB = request_queue[i];
                    returnIndex = i;                
                }
            }
        }
        if(returnIndex == -1){
            for(int i = 0; i < *queue_cnt; i++){
                if(request_queue[i].cylinder > current_cylinder){
                    if(returnIndex == -1 || abs(request_queue[i].cylinder-current_cylinder) < closest_cylinder || 
                    ((abs(request_queue[i].cylinder-current_cylinder) == closest_cylinder) && (request_queue[i].arrival_timestamp < earliest_arrival))){
                        closest_cylinder = abs(request_queue[i].cylinder-current_cylinder);
                        earliest_arrival = request_queue[i].arrival_timestamp;
                        returnRCB = request_queue[i];
                        returnIndex = i;                
                    }
                }
            }            
        }
        if(returnIndex != -1){
            for(int j = returnIndex; j < *queue_cnt-1; j++){
                request_queue[j] = request_queue[j+1];
            }

            (*queue_cnt)--;   
            return returnRCB;          
        }     
    }
    return returnRCB;
}

