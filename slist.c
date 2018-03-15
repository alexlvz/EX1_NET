#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "slist.h"
#define EMPTY_LIST 0
#define SUCCESS 0
#define FAILURE -1
void slist_init(slist_t *list)
{
    if(list == NULL)
        return;
    
    slist_head(list)=NULL;
    slist_tail(list)=NULL;
    slist_size(list)= EMPTY_LIST ;
}
//private function for creating the node with a given data.
void *create_node(void *data)
{
    slist_node_t *node = (slist_node_t*)malloc(sizeof(slist_node_t));
    slist_data(node) = data;
    slist_next(node) = NULL;
    return node;
}

void *slist_pop_first(slist_t *list)
{
    if(list == NULL || slist_head(list) == NULL) //check this when showing to someone
        return (int*)FAILURE; 
    
     void *pop =slist_data(slist_head(list));
     
     slist_node_t *temp = slist_head(list);
     slist_head(list) = slist_next(slist_head(list));
     slist_size(list) --; 
     free(temp);
     temp = NULL;
     return pop;
}

int slist_append(slist_t *list,void *data)
{   
    if(list == NULL)
        return FAILURE;
    slist_node_t *node = create_node(data);
    if(node == NULL)
        return FAILURE;
    
    if(slist_head(list) == NULL)
    {
        slist_head(list) = node;
        slist_tail(list) = node;
    }
    else
    {
        slist_node_t *curr = slist_head(list);
        while(slist_next(curr)!=NULL)
            curr = slist_next(curr);
        
        slist_next(curr) = node;
        slist_tail(list) = slist_next(curr);
    }
    slist_size(list) ++;     
    return SUCCESS;
}

int slist_prepend(slist_t *list,void *data)
{
    if(list == NULL)
        return FAILURE;
    slist_node_t *node = create_node(data);
    if(node == NULL)
        return FAILURE;
    
    slist_node_t *temp = slist_head(list);
    slist_next(node) = temp;
    slist_head(list) = node;;
    slist_size(list) ++; 
    
    return SUCCESS;
}

int slist_append_list(slist_t *list1, slist_t *list2)
{
    if(list1 == NULL && list2 == NULL)
        return FAILURE;
    
    if(list2 == NULL)
        return FAILURE;
   
     slist_node_t *curr2 = slist_head(list2);
     
     while(curr2!=NULL)
     {
         slist_append(list1, slist_data(curr2));
         curr2 = slist_next(curr2);
     }    
    return SUCCESS;
}

void slist_destroy(slist_t *list,slist_destroy_t type)
{  
    if(list == NULL)
        return;
    
    void *data; //will hold the data to be freed.
    
    while(slist_head(list)!= NULL)
    {
        if(type == SLIST_FREE_DATA)
        {
            data = slist_pop_first(list);
            if(data != NULL)
                free(data);
            data = NULL;
        }
        else
            slist_pop_first(list);
    } 
}

