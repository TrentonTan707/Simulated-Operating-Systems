#include <stdio.h>
#include <stdlib.h>
#include "oslabs.h"
#include <stdbool.h>

int memoryBlock_is_null(struct MEMORY_BLOCK x){
    if (x.start_address == 0 && x.end_address == 0 && x.segment_size == 0 && x.process_id == 0){
        return 1;
    }
    else
        return 0;
}

struct MEMORY_BLOCK best_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX],int *map_cnt, int process_id){
    struct MEMORY_BLOCK nullBlock = {0,0,0,0};
    if(*map_cnt == 0){
        return nullBlock;
    }
    int closestSize = memory_map[0].segment_size;
    struct MEMORY_BLOCK returnBlock = nullBlock;
    struct MEMORY_BLOCK remainederBlock = nullBlock;
    int returnIndex = -1;

    for(int i = 0; i < *map_cnt; i++){
        if(memory_map[i].process_id == 0 && memory_map[i].segment_size >= request_size){
            if(memory_map[i].segment_size-request_size < closestSize){
                closestSize = memory_map[i].segment_size-request_size;
                returnIndex = i;
                returnBlock = memory_map[i];
                if(closestSize == 0){
                    break;
                }
            }
        }        
    }
    if(returnIndex == -1){
        return nullBlock;
    }

    else{
        if(returnBlock.segment_size == request_size){
            returnBlock.process_id = process_id;
            memory_map[returnIndex].process_id = process_id;
            return returnBlock;
        }
        else{
            returnBlock.process_id = process_id;
            returnBlock.start_address = memory_map[returnIndex].start_address;
            returnBlock.end_address = returnBlock.start_address+request_size-1;
            returnBlock.segment_size = request_size;

            
            remainederBlock.process_id = 0;
            remainederBlock.start_address = returnBlock.end_address+1;
            remainederBlock.end_address = memory_map[returnIndex].end_address;
            remainederBlock.segment_size = memory_map[returnIndex].segment_size-request_size;

            memory_map[returnIndex] = returnBlock;

            for(int i = *map_cnt; i > returnIndex+1; i--){
                memory_map[i] = memory_map[i-1];
            }
            memory_map[returnIndex+1] = remainederBlock;
            (*map_cnt)++;
        }
        return returnBlock;
    }
    return nullBlock;
}
struct MEMORY_BLOCK first_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX],int *map_cnt, int process_id){
    struct MEMORY_BLOCK nullBlock = {0,0,0,0};
    if(*map_cnt == 0){
        return nullBlock;
    }

    struct MEMORY_BLOCK returnBlock = nullBlock;
    struct MEMORY_BLOCK remainederBlock = nullBlock;
    int returnIndex = -1;
    for(int i = 0; i < *map_cnt; i++){
        if(memory_map[i].process_id == 0 && memory_map[i].segment_size >= request_size){
            if(memory_map[i].segment_size == request_size){
                memory_map[i].process_id = process_id;
                return memory_map[i];
            }            
        

            returnBlock.process_id = process_id;
            returnBlock.start_address = memory_map[i].start_address;
            returnBlock.end_address = returnBlock.start_address+request_size-1;
            returnBlock.segment_size = request_size;

            
            remainederBlock.process_id = 0;
            remainederBlock.start_address = returnBlock.end_address+1;
            remainederBlock.end_address = memory_map[i].end_address;
            remainederBlock.segment_size = memory_map[i].segment_size-request_size;

            memory_map[i] = returnBlock;

            for(int j = *map_cnt; j > i+1; j--){
                    memory_map[j] = memory_map[j-1];
                }
            memory_map[i+1] = remainederBlock;
            (*map_cnt)++;
            return returnBlock;
        }
    }
    return nullBlock;
} 
struct MEMORY_BLOCK worst_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX],int *map_cnt, int process_id){
    struct MEMORY_BLOCK nullBlock = {0,0,0,0};
    if(*map_cnt == 0){
        return nullBlock;
    }
    int worstSize = -1;
    struct MEMORY_BLOCK returnBlock = nullBlock;
    struct MEMORY_BLOCK remainederBlock = nullBlock;
    int returnIndex = -1;

    for(int i = 0; i < *map_cnt; i++){
        if(memory_map[i].process_id == 0 && memory_map[i].segment_size >= request_size){
            if(memory_map[i].segment_size-request_size > worstSize){
                worstSize = memory_map[i].segment_size-request_size;
                returnIndex = i;
                returnBlock = memory_map[i];
            }
        }        
    }
    if(returnIndex == -1){
        return nullBlock;
    }

    else{
        if(returnBlock.segment_size == request_size){
            returnBlock.process_id = process_id;
            memory_map[returnIndex].process_id = process_id;
            return returnBlock;
        }
        else{
            returnBlock.process_id = process_id;
            returnBlock.start_address = memory_map[returnIndex].start_address;
            returnBlock.end_address = returnBlock.start_address+request_size-1;
            returnBlock.segment_size = request_size;

            
            remainederBlock.process_id = 0;
            remainederBlock.start_address = returnBlock.end_address+1;
            remainederBlock.end_address = memory_map[returnIndex].end_address;
            remainederBlock.segment_size = memory_map[returnIndex].segment_size-request_size;

            memory_map[returnIndex] = returnBlock;

            for(int i = *map_cnt; i > returnIndex+1; i--){
                memory_map[i] = memory_map[i-1];
            }
            memory_map[returnIndex+1] = remainederBlock;
            (*map_cnt)++;
        }
        return returnBlock;
    }  
    return nullBlock;
} 
struct MEMORY_BLOCK next_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX],int *map_cnt, int process_id, int last_address){
    struct MEMORY_BLOCK nullBlock = {0,0,0,0};
    if(*map_cnt == 0){
        return nullBlock;
    }

    struct MEMORY_BLOCK returnBlock = nullBlock;
    struct MEMORY_BLOCK remainederBlock = nullBlock;


    for(int i = 0; i < *map_cnt; i++){
        if(memory_map[i].process_id == 0 && memory_map[i].segment_size >= request_size && memory_map[i].start_address >= last_address){
            if(memory_map[i].segment_size == request_size){
                memory_map[i].process_id = process_id;
                return memory_map[i];
            }                
        
            returnBlock.process_id = process_id;
            returnBlock.start_address = memory_map[i].start_address;
            returnBlock.end_address = returnBlock.start_address+request_size-1;
            returnBlock.segment_size = request_size;

            
            remainederBlock.process_id = 0;
            remainederBlock.start_address = returnBlock.end_address+1;
            remainederBlock.end_address = memory_map[i].end_address;
            remainederBlock.segment_size = memory_map[i].segment_size-request_size;

            memory_map[i] = returnBlock;

            for(int j = *map_cnt; j > i+1; j--){
                    memory_map[j] = memory_map[j-1];
                }
            memory_map[i+1] = remainederBlock;
            (*map_cnt)++;
            return returnBlock;

        }
    }

    for(int i = 0; i < *map_cnt; i++){
        if(memory_map[i].process_id == 0 && memory_map[i].segment_size >= request_size && memory_map[i].start_address < last_address){
            if(memory_map[i].segment_size == request_size){
                memory_map[i].process_id = process_id;
                return memory_map[i];
            }                
        
            returnBlock.process_id = process_id;
            returnBlock.start_address = memory_map[i].start_address;
            returnBlock.end_address = returnBlock.start_address+request_size-1;
            returnBlock.segment_size = request_size;

            
            remainederBlock.process_id = 0;
            remainederBlock.start_address = returnBlock.end_address+1;
            remainederBlock.end_address = memory_map[i].end_address;
            remainederBlock.segment_size = memory_map[i].segment_size-request_size;

            memory_map[i] = returnBlock;

            for(int j = *map_cnt; j > i+1; j--){
                    memory_map[j] = memory_map[j-1];
                }
            memory_map[i+1] = remainederBlock;
            (*map_cnt)++;
            return returnBlock;

        }
    }

    return nullBlock;
}

void release_memory(struct MEMORY_BLOCK freed_block, struct MEMORY_BLOCK memory_map[MAPMAX],int *map_cnt){

    int freedIndex = -1;
    for (int i = 0; i < *map_cnt; i++){
        if(memory_map[i].start_address == freed_block.start_address && memory_map[i].end_address == freed_block.end_address &&
           memory_map[i].process_id == freed_block.process_id){
            freedIndex = i;
            break;
           }
    }

    if(freedIndex == -1){
        return;
    }

    memory_map[freedIndex].process_id = 0;

    if(freedIndex+1 < *map_cnt && memory_map[freedIndex+1].process_id == 0){
        memory_map[freedIndex].end_address = memory_map[freedIndex+1].end_address;
        memory_map[freedIndex].segment_size = memory_map[freedIndex].segment_size + memory_map[freedIndex+1].segment_size;        
        for(int j = freedIndex+1; j < *map_cnt-1; j++){
            memory_map[j] = memory_map[j+1];
        }
        (*map_cnt)--;
    }
    if(freedIndex > 0 && memory_map[freedIndex-1].process_id == 0){
        memory_map[freedIndex-1].end_address = memory_map[freedIndex].end_address;
        memory_map[freedIndex-1].segment_size = memory_map[freedIndex].segment_size + memory_map[freedIndex-1].segment_size;        
        for(int j = freedIndex; j < *map_cnt-1; j++){
            memory_map[j] = memory_map[j+1];
        }
        (*map_cnt)--;        
    }


}