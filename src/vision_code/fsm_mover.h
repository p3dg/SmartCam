/* Finite State machine designed for runtime building of graphs and
 * up to two outputs
 *
 * Author: Idan Warsawski
 *         Peter Galvin
 */
#ifndef FSM_MOVER
#define FSM_MOVER

#include <stdlib.h>
#include <string.h>
#include "linked_list.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FSM_STATE_NAME_LEN 32

struct fsm_state;

struct fsm_state
{
  struct fsm_state * left;
  struct fsm_state * right;

  char name[FSM_STATE_NAME_LEN];

  void * state_data;
};


struct fsm_mover 
{
  struct fsm_state * current_state;
  
  struct node * states;
};




struct fsm_mover * fsm_init_machine();

void fsm_add_state(struct fsm_mover * mover, char * name, void * data);

void fsm_set_current_state(struct fsm_mover * mover, char * name);

int fsm_set_state_jump(struct fsm_mover * mover, char * state_name, char * left_name, char * right_name);

int fsm_move_right(struct fsm_mover * mover);

int fsm_move_left(struct fsm_mover * mover);

void * fsm_get_current_state_data(struct fsm_mover * mover);

char * fsm_get_current_state_name(struct fsm_mover * mover);

#ifdef __cplusplus
}
#endif

#endif
