#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "slist.h"
#include "pattern-matching.h"

#define SUCCESS 0
#define ERROR -1
#define state_id(s) s->id
#define state_depth(s) s->depth
#define state_output(s) s->output
#define state_transitions(s) s->_transitions
#define state_fail(s) s->fail
#define pm_zerostate(pm) pm->zerostate
#define pm_newstate(pm) pm->newstate
#define edge_state(e) e->state
#define edge_label(e) e->label
#define match_pattern(m) match->pattern
#define match_fstate(m) match->fstate
#define match_start_pos(m) match->start_pos
#define match_end_pos(m) match->end_pos

//==================================================================================//
//Private function for creating a new state. it recieves id and depth 
//and returns an allocated state
void *createState(int id, int depth)
{   
    slist_t *outputList =(slist_t*)malloc(sizeof(slist_t)); //list of patterns
    slist_t *transitionsList =(slist_t*)malloc(sizeof(slist_t)); //list of edges
    pm_state_t *state = (pm_state_t*)malloc(sizeof(pm_state_t));
    
    if(outputList == NULL || transitionsList == NULL || state == NULL) //denies segmentatation
        return NULL;
    
    state_id(state) = id;
    state_depth(state) =depth;
    slist_init(outputList);
    state_output(state) = outputList;
    slist_init(transitionsList);
    state_transitions(state) = transitionsList;
	
    return state;
}
//==================================================================================//
//Initialize the PM
int pm_init(pm_t *pm)
{
    if(pm == NULL) //denies segmentatation fault
        return ERROR;
    
    pm_newstate(pm) = 0;   
	pm_zerostate(pm) = createState( 0, 0);
    if(pm_zerostate(pm) == NULL) //for malloc fail
        return ERROR;
    
    state_fail(pm_zerostate(pm)) = NULL;
    
    return SUCCESS;
}
//==================================================================================//
/* Adds a new string to the fsm, given that the string is of length n. 
   Returns 0 on success, -1 on failure.*/
int pm_addstring(pm_t *pm,unsigned char *label, size_t n)
{
    if(pm == NULL || pm_zerostate(pm) == NULL || label == NULL || n<0) //denies segmentatation fault
        return ERROR;

    unsigned int state; //holds state id
    pm_state_t *fromState, *toState, *tempState; 
    fromState = pm_zerostate(pm);
    
    for(int i = 0 ; i<n ;i++)
    {
        tempState = pm_goto_get(fromState,label[i]); //will hold the result of checking if a new state is needed
        
        if(tempState == NULL) //new state needed
        {
			pm_newstate(pm)++;
            state = pm_newstate(pm);
            toState = createState(state, i+1);
            pm_goto_set(fromState, label[i], toState);  
            
            printf("Allocating state %d\n",(int)toState->id);
            printf("%d->%c->%d\n",(int) state_id(fromState),(char)label[i],(int) state_id(toState));
                    
            fromState = toState;
        }
        else //a state for the character already exists
            fromState = tempState;
    }
    if(slist_size(state_output(fromState)) == 0) //if the state is a new final state
        slist_append(state_output(fromState),label);
    return SUCCESS;
}
//==================================================================================//
/* Finalizes construction by setting up the failrue transitions, as
   well as the goto transitions of the zerostate. 
   Returns 0 on success, -1 on failure.*/
int pm_makeFSM(pm_t *pm)
{
    if(pm == NULL ||pm_zerostate(pm) == NULL) //denies segmentatation fault
        return ERROR;
	
    slist_t *list = (slist_t *)malloc(sizeof(slist_t));
    if(list == NULL)
        return ERROR;
	
    slist_init(list); //will initialize a help queue for making the failure transitions
    
    pm_state_t *nextState; //will hold the state the current state fail will point to
    pm_state_t *tempState; //will hold the fail of the nextState
    pm_state_t *gotoFreturnState; //the state that comes from goto_get function with tempState. will tell where to go 
    pm_labeled_edge_t *currentEdge; //for looping through the queue and list

    for(slist_node_t *curr = slist_head(state_transitions(pm_zerostate(pm))); curr!= NULL; curr=slist_next(curr))
    {//setting the depth 1 fail transitions
        currentEdge = slist_data(curr);
        slist_append(list,edge_state(currentEdge));
        state_fail(edge_state(currentEdge)) = pm_zerostate(pm);
        printf("Setting f(%d) = %d\n", (int)state_id(edge_state(currentEdge)), (int)state_id(pm_zerostate(pm)));
    }
    
    while(slist_size(list) > 0) //implementing the BFS algorithm
    {
        nextState = slist_pop_first(list);

        for(slist_node_t *curr = slist_head(state_transitions(nextState)); curr!= NULL; curr=slist_next(curr))
        {      
            currentEdge = slist_data(curr);
            slist_append(list,edge_state(currentEdge));      
            tempState = state_fail(nextState);
            
            gotoFreturnState = pm_goto_get(tempState ,edge_label(currentEdge));
            while(gotoFreturnState == NULL)
            {
                if(tempState != pm_zerostate(pm)) //if no label and not root -> go back
                    tempState = state_fail(tempState);

                gotoFreturnState= pm_goto_get(tempState,edge_label(currentEdge));
                
                if(tempState == pm_zerostate(pm) && gotoFreturnState == NULL) //only if root and nowhere to go
                    gotoFreturnState = pm_zerostate(pm); // set returnState to zeroState      
            }
            state_fail(edge_state(currentEdge)) = gotoFreturnState; //finally set the return edge
            printf("Setting f(%d) = %d\n", (int)state_id(edge_state(currentEdge)), (int)state_id(gotoFreturnState));
            slist_append_list(state_output(edge_state(currentEdge)),state_output(gotoFreturnState)); //conect the output lists of the final states
        }     
    }
    slist_destroy(list,SLIST_FREE_DATA); //distroys the queue
    free(list);
    list = NULL;
    return SUCCESS;
}
//==================================================================================//
/* Set a transition arrow from this from_state, via a symbol, to a
   to_state. will be used in the pm_addstring and pm_makeFSM functions.
   Returns 0 on success, -1 on failure.*/   
int pm_goto_set(pm_state_t *from_state, unsigned char symbol, pm_state_t *to_state)
{
    if(from_state == NULL || to_state == NULL)
        return ERROR;
    pm_labeled_edge_t *edge = (pm_labeled_edge_t*)malloc(sizeof(pm_labeled_edge_t));
    if(edge == NULL)
        return ERROR;
    edge_state(edge) = to_state;
    edge_label(edge) = symbol;
    slist_append(state_transitions(from_state),edge);
  
    return SUCCESS;
}
//==================================================================================//
/* Returns the transition state.  If no such state exists, returns NULL. 
   will be used in pm_addstring, pm_makeFSM, pm_fsm_search, pm_destroy functions. */
pm_state_t* pm_goto_get(pm_state_t *state, unsigned char symbol)
{
    if(state == NULL)
        return NULL;   
    slist_node_t *curr = slist_head(state_transitions(state)); 
    pm_labeled_edge_t *edge;

    for(slist_node_t *temp = curr ; temp != NULL ; temp= slist_next(temp)) //looping through the arrows of the state 
    {
        edge = slist_data(temp);
        if(symbol == edge_label(edge)) //if found the label on one of the arrows...
           return edge_state(edge);
    }
    return NULL;
}
//==================================================================================//
/* Search for matches in a string of size n in the FSM. 
   if there are no matches return empty list */
slist_t* pm_fsm_search(pm_state_t *state, unsigned char *text ,size_t size)
{
    if(state == NULL || text == NULL || slist_head(state_transitions(state)) == NULL ||size<0)
        return NULL; 
	
	pm_labeled_edge_t *edge = slist_data(slist_head(state_transitions(state)));
	pm_state_t *tempState = edge_state(edge);
	
	if(state_fail(tempState) == NULL) //if search was ran without makeFSM
		return NULL;
	
    slist_t *matches = (slist_t*)malloc(sizeof(slist_t)); //will hold the matches info
    slist_init(matches);
    
    pm_state_t *gotoState; //with this state we will traverse our PM
    
    for(int i=0; i<size; i++) //loop the text
    {
        gotoState = pm_goto_get(state, text[i]);
        
        if(gotoState == NULL && state!= NULL && state_depth(state) == 0) //if from zerostate there is no where to go, eat the label and continue
            continue;
        
        while(gotoState == NULL && state!= NULL && state_depth(state) != 0) //if path not found , try another way by going back to the fail state of the state
        {    
             state = state_fail(state);
             gotoState = pm_goto_get(state, text[i]);
            
        }
        if(gotoState != NULL) //if there is a path go and find it
            state = pm_goto_get(state, text[i]);
        
        if(state!=NULL && slist_size(state_output(state)) > 0) //if there is a match and the state is a final state
        {
           for(slist_node_t *curr = slist_head(state_output(state)); curr!=NULL; curr = slist_next(curr)) //for each match in the list - make a match struct
            {
                pm_match_t *match = (pm_match_t*)malloc(sizeof(pm_match_t));
                match_pattern(match) = slist_data(curr);
                match_fstate(match) = state;
                match_start_pos(match) = i -  strlen(match->pattern) + 1;
                match_end_pos(match) = i ;
                slist_append(matches,match);   
                
                printf("Pattern: %s, starts at: %d, ends at: %d, last state = %d\n",  match_pattern(match), match_start_pos(match), match_end_pos(match),(int)state_id(match_fstate(match)));
            }        
        }            
    }
    return matches;
}
//==================================================================================//
//A private recursive function for traversing the PM and freeing up the allocated memory
//going from the lowest leaf to root (like DFS)
void pm_destroy_helper(pm_state_t *root)
{
    pm_labeled_edge_t *edge; //will hold each edge of every state
    if(root == NULL)  //stop condition 
        return;
    
    for(slist_node_t *curr = slist_head(state_transitions(root)); curr!=NULL; curr = slist_next(curr)) //looping the edges of the current state
    {
        edge = slist_data(curr);
        pm_destroy_helper(edge_state(edge)); //recursive call for the next edge in the PM
    }
    if(root != NULL) //freeing the the allocated data of the state
    {
        slist_destroy(state_output(root),SLIST_LEAVE_DATA);
        slist_destroy(state_transitions(root),SLIST_FREE_DATA);
        free(state_output(root));
        free(state_transitions(root));
        state_output(root) = NULL;
        state_transitions(root) = NULL;
        free(root);
        root = NULL;
    }  
}
//==================================================================================//
/* Destroys the fsm, deallocating memory. */
void pm_destroy(pm_t *pm)
{
    if(pm == NULL || pm_zerostate(pm) == NULL) //if not allocated
        return;
    pm_destroy_helper(pm_zerostate(pm)); //call the recuresive destroy function helper
}
//==================================================================================//

