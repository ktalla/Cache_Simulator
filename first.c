#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <limits.h>

struct block
{
    bool valid;
    unsigned long tag;
    unsigned long time; //for LRU
};

unsigned long address;
char operation;
unsigned long tag;
unsigned long t1;
unsigned long index;
unsigned long block_offset_bits;
unsigned long set_index_bits;
unsigned long sets;
int policy;
int memread;
int memwrite;
int cachehit;
int cachemiss;
struct block** cache;
int num_of_blocks;
int time_id;
void hit_or_miss_write();
void hit_or_miss_read();
void block_write();



int main(int argc, char* argv[argc+1]) //check this
{
    int cache_size = atoi(argv[1]);
    char* associativity = argv[2];
    num_of_blocks = atoi(&associativity[6]);
    //print("%d\n", num_of_blocks);


    
    if(strcmp(argv[3], "fifo")==0)
    {
        policy=1;
    }
    else if(strcmp(argv[3], "lru")==0)
    {
        policy=2;
    }

    int block_size = atoi(argv[4]);
    FILE* fp = fopen(argv[5], "r");

    memread=0;
    memwrite=0;
    cachehit=0;
    cachemiss=0;

    block_offset_bits=log2(block_size);
    //printf("BOB: %lx\n", block_offset_bits);

    sets = cache_size/(num_of_blocks*block_size);

    set_index_bits=log2(sets);
    //printf("SIB: %lx\n", set_index_bits);

    //make cache
cache=malloc(sets*sizeof(struct block*));
for(int i=0; i<sets; i++)
{
    cache[i]=malloc((num_of_blocks)*sizeof(struct block));
}

//initialize cache
for(int i=0; i<sets; i++)
{
    for(int j=0; j<num_of_blocks; j++)
    {
        cache[i][j].valid = false;
        cache[i][j].tag=0;
        cache[i][j].time=0;
    }
}

time_id=0;

while(fscanf(fp, "%c %lx\n", &operation, &address)!=EOF)
{
    //printf("addy: %lx\n", address);
    tag=address>>(block_offset_bits+set_index_bits);
    //printf("tag: %lx\n", tag);
    t1 = address>>block_offset_bits;
    index = t1 & ((1<<set_index_bits)-1);
   // printf("index: %lx\n", index);
    time_id++;


if(operation== 'R')
{
    hit_or_miss_read();
}
else if(operation=='W')
{
    
    hit_or_miss_write();
    
}

}

printf("memread:%d\n",memread);
printf("memwrite:%d\n", memwrite);
printf("cachehit:%d\n",cachehit);
printf("cachemiss:%d\n", cachemiss);

}




void hit_or_miss_read()
{
for(int j=0; j<num_of_blocks; j++)
{
 if(cache[index][j].valid==true)
{
    if(cache[index][j].tag == tag)
    {
        cachehit++;
        if(policy==2)
        {
          cache[index][j].time=time_id;
        }
        return;   
    }
}
}
 //there is nothing there and never was
 cachemiss++;
 memread++;
 block_write();

}



void hit_or_miss_write()
{
    for(int j=0; j<num_of_blocks; j++)
    {
        if(cache[index][j].valid==true) 
        {
            if(cache[index][j].tag ==tag) 
            {
            cachehit++;
            memwrite++;
            if(policy==2)
            {
            cache[index][j].time=time_id;
            }
            return;
            }
    
        }
    }

    // for(int j=0; j<num_of_blocks; j++)
    // {
    // if(cache[index][j].valid==false) 
    // {
    //     cachemiss++;
    //     memread++;
    //     block_write();
    //     return;
        
    // }
    // }
       cachemiss++;
       memread++;
       memwrite++;
       block_write(); //we want to write the block from memory into this cache
       return; 
    

}


void block_write()
{
    int least_time=INT_MAX;
    int kick_out=0;
    for(int j=0; j<num_of_blocks; j++)
    {
        if(cache[index][j].valid==false)
        {
            kick_out=j;
            break;
        }
        else if(cache[index][j].time<least_time)
        {
            least_time=cache[index][j].time;
            kick_out=j;
        }

    }
    cache[index][kick_out].tag = tag;
    cache[index][kick_out].valid = true;
    cache[index][kick_out].time = time_id;
    

}


