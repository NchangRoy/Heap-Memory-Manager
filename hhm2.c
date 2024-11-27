#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<stddef.h>
//#include"hmm2.h"

struct free_list {
    void *address;
    int size;
    int allocated;
    struct free_list *next;
};
const int max_initial_zone_size=1000;
char * free_zone_area_start=NULL;

struct free_list *head;

void init_zone(){
    free_zone_area_start=sbrk(0);//set begining of free zone to free_zone_area_start
    sbrk(max_initial_zone_size);
    //initializing head
    head=(struct free_list *)free_zone_area_start;
    free_zone_area_start+=sizeof(struct free_list);//increment pointer by the size of one struct free_list
    brk(free_zone_area_start);
    head->size=(max_initial_zone_size-sizeof(struct free_list));
    head->next=NULL;
    head->allocated=0;
    head->address==NULL;

}

struct free_list *create_node(void * pointer_to_insert){
    
    struct free_list *new_node=(struct free_list *)pointer_to_insert;
    //free_zone_area_start+=sizeof(struct free_list);
    //printf("%p",new_node);
    //adjusting pointer of free end
   // brk(free_zone_area_start);
   new_node->address=new_node+sizeof(struct free_list);
    printf("node created\n");
    return new_node;
}

/*struct free_list* create_rand_node(int size){
    struct free_list *temp=(struct free_list*)malloc(sizeof(struct free_list));
    temp->next=NULL;
    temp->size=size;
    temp->allocated=1;
    temp->address=NULL;
    return temp;
}*/

void add_node(void* node){
    struct free_list *temp=head;
    while(temp->next!=NULL){
        temp=temp->next;
    }
    temp->next=(struct free_list*)node;
}
void delete_node(struct free_list * node){
    
    struct free_list *temp=head;
    int present=1;//flag variable to test if node exist in linked list
    while(temp->next!=NULL){
        if(temp->next==node){
            present=0;
            break;
        }
        temp=temp->next;
    }
    if(present==1){
        printf("node not present in list");
        return ;
    }else{
        temp->next=node->next;
        head->size+=node->size;
    }

  
    
  
   // temp->next=node->next;
}
struct free_list * get_node(void * value_pointed){
    struct free_list*temp=head->next;
    //printf(" node %p\n",node);
    int size=0;
    while(temp+0x1+size!=value_pointed&&temp->next!=NULL){
        //printf("%p",sizeof(struct free_list));
        //printf("first %p\n",temp+0x1);
        temp=temp->next;
    }
    return temp;

}
void print_nodes(){
    struct free_list *temp=head;
    while(temp->next!=NULL){
        printf("%p\n",temp);
        printf("Adress:%p\n",temp->address);
        printf("Node size:%d\n",temp->size);
        printf("allocated:%d\n",temp->allocated);
        printf("%p\n",temp->next);
        temp=temp->next;
    }
    printf("%p\n",temp);
    printf("Node size:%d\n",temp->size);
    printf("allocated:%d\n",temp->allocated);
    printf("%p\n",temp->next);

}
struct free_list * best_fit(int size){
    
    
    struct free_list *best_fit_node=NULL;
    if(head->next==NULL){
        return NULL;
    }
    else{
        struct free_list *temp=head->next;
        //initialise best fit 
        //find first node with size greater than that of node to insert
        while(temp!=NULL){
            if(temp->size>size){
                if(temp->allocated==0){
                    best_fit_node=temp;
                    break;
                }
               
            }
            temp=temp->next;
        }
        if(best_fit_node==NULL){
            return best_fit_node;
        }

       
        temp=head->next;
        while(temp!=NULL){

            if(temp->size>size && temp->size<best_fit_node->size){
                if(temp->allocated==0){
                    best_fit_node=temp;
                }
                
            }

            temp=temp->next;

        }
        return best_fit_node;
    }
    

}



int coalesce(struct free_list * node_just_freed){
    struct free_list *temp=head->next;
    
    //first finding node just before node whose memory got freed
    if(node_just_freed!=temp){
        while(temp->next!=node_just_freed){
        temp=temp->next;
        
        }
        //check if this node is empty,if its allocation=0, we add the size of the just free node to its size and delete the freed node
    //delete next node if it is unallocated
    if(node_just_freed->next!=NULL){
        if(node_just_freed->next->allocated==0){
            node_just_freed->size+=node_just_freed->next->size;
        delete_node(node_just_freed->next);
        }
    }
    //merge with previous node if previous node is unallocated
   if(temp->allocated==0){
        temp->size+=node_just_freed->size;
        printf("enter here");
        //delete node just freed
        delete_node(node_just_freed);

        
    }
   
        //delete next node
    
    //handle coalescing for first node
    }else{
        if(node_just_freed->next!=NULL){
        if(node_just_freed->next->allocated==0){
            node_just_freed->size+=node_just_freed->next->size;
        delete_node(node_just_freed->next);
        }
    }
   

    
    
    }
}
/*void insert_node_into(struct free_list *node_to_insert,struct free_list * reference){
    struct free_list* temp=head;
    while(temp->next!=reference){
        temp=temp->next;
    }
    
    reference->size-=node_to_insert->size;
    node_to_insert->next=reference;
    temp->next=node_to_insert;


}*/



void *mon_malloc(int size){
    if(free_zone_area_start==NULL){
        init_zone();
    }

    struct free_list * best_node_to_insert=best_fit(size);
    if(best_node_to_insert!=NULL){
        printf("exist");
                
        best_node_to_insert->allocated=1;
        
        return best_node_to_insert+sizeof(struct free_list);
    }
    else{
    if((size +sizeof(struct free_list))>=head->size){
        //first increase the break of the heap
        sbrk(size+sizeof(struct free_list));
        head->size+=size+sizeof(struct free_list);
    }

        
        
        //we create a new node
        struct free_list *new_node=create_node(free_zone_area_start);
        new_node->size=size;
        new_node->next=NULL;
        new_node->allocated=1;

        //update start of free memory zone by  increasing its position by the size of one struct list
        free_zone_area_start+=sizeof(struct free_list);
        new_node->address=free_zone_area_start;
        //we adjust pointer to free end after allocating the memory we need in part of the free_zone_area
        void * copy=free_zone_area_start;
        free_zone_area_start+=size;
        brk(free_zone_area_start);
        //we add the new node tosizeof(struct free_list) the linked list
        add_node(new_node);
        head->size-=(size+sizeof(struct free_list));
        printf("size: %d %p\n",new_node->size,free_zone_area_start);
        return copy;//the begining of the alocated space is the end of the structure of
                                             // its node ie |new_node..sizeof(struct)..|free spae
    
    }
}
void mon_free(void *node){
    //to free memory means to change allocated bit to 0 and increase size of free space in head
    struct free_list*temp=get_node(node);
    printf("see %p",temp);
    //printf("first %p\n",temp);
   // printf("size %p\n",temp->size);
    if(temp==NULL){
        printf("No memory was allocated to the pointer %p",node);
    }
    else{
        temp->allocated=0;
        
        head->size+=temp->size;
        coalesce(temp);
        printf("removed successfuly");
    }

}

int main(){
    char *name =(char *)mon_malloc(2);
    printf("%pname \n",name);
    int *pronom=(int *)mon_malloc(5*sizeof(int));
    char *pronom2=(char *)mon_malloc(3);
    
   
   
    char *boys=(char *)mon_malloc(4*sizeof(char));
    char *boys2=(char *)mon_malloc(5);
    printf("hello %p %p\n",pronom2,pronom);
    //printf("%ld",sizeof(struct free_list));
    print_nodes();
    printf("%pname \n",name);
   // mon_free(name);
   // 
   //mon_free(pronom2);
    mon_free(pronom);
    print_nodes();
   // mon_free(boys);
   //printf("where to insert: %p\n",best_fit(2));
   
    char *boys3=(char *)mon_malloc(2);
    mon_free(name);
    char *boys4=(char *)mon_malloc(5);
    printf("node 4 %p\n",boys);
    
   //delete_node(get_node(pronom2)); 
    print_nodes();
    //struct free_list *test=create_rand_node(14);
    
    
   
    //insert_node_into(test,best_fit(test));
    //print_nodes();

    
    //printf("%p",get_node(pronom));
   
}

