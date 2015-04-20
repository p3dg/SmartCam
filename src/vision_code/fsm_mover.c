/* Finite State machine designed for runtime building of graphs and
 * up to two outputs
 *
 * Author: Idan Warsawski
 *         Peter Galvin
 */

#include "fsm_mover.h"


struct fsm_mover * fsm_init_machine()
{
  struct fsm_mover * mover;
  mover = malloc(sizeof(struct fsm_mover));

  mover->current_state = NULL;
  mover->states = NULL;

  return mover;
}

void fsm_add_state(struct fsm_mover * mover, char * name, void * data)
{
  struct fsm_state * new_state;
  new_state = malloc(sizeof(struct fsm_state));
  memset(new_state, 0x00, sizeof(struct fsm_state));
  new_state->state_data = data;

  strncpy(new_state->name, name, FSM_STATE_NAME_LEN);

  linked_list_add_tail(&mover->states, new_state);

  if(mover->current_state == NULL)
    {
      mover->current_state = new_state;
    }
}

void fsm_set_current_state(struct fsm_mover * mover, char * name)
{
  struct node * element;
  LL_FOREACH(mover->states, element)
    {
      if(strncmp(((struct fsm_state *) element->data)->name, name, FSM_STATE_NAME_LEN) == 0)
        {
          mover->current_state = (struct fsm_state *) element->data;
        }
    }
}

int fsm_set_state_jump(struct fsm_mover * mover, char * state_name, char * left_name, char * right_name)
{
  struct fsm_state * cur_state, * left_state, * right_state; 
  struct node * element;

  cur_state = NULL;
  left_state = NULL;
  right_state = NULL;

  LL_FOREACH(mover->states, element)
    {
      if(strncmp(((struct fsm_state *) element->data)->name, state_name, FSM_STATE_NAME_LEN) == 0)
        {
          cur_state = (struct fsm_state *) element->data;
        }

      if(left_name != NULL)
        if(strncmp(((struct fsm_state *) element->data)->name, left_name, FSM_STATE_NAME_LEN) == 0)
          {
            left_state = (struct fsm_state *) element->data;
          }

      if(right_name != NULL)
        if(strncmp(((struct fsm_state *) element->data)->name, right_name, FSM_STATE_NAME_LEN) == 0)
          {
            right_state = (struct fsm_state *) element->data;
          }
    }

  if(cur_state == NULL)
    return -1;

  cur_state->left = left_state;
  cur_state->right = right_state;

  return 0;
}

int fsm_move_right(struct fsm_mover * mover)
{
  if(mover->current_state->right != NULL)
    mover->current_state = mover->current_state->right;
  else
    return -1;

  return 0;
}

int fsm_move_left(struct fsm_mover * mover)
{
  if(mover->current_state->left != NULL)
    mover->current_state = mover->current_state->left;
  else
    return -1;

  return 0;
}

void * fsm_get_current_state_data(struct fsm_mover * mover)
{
  if(mover->current_state == NULL)
    return NULL;

  return mover->current_state->state_data;
}

char * fsm_get_current_state_name(struct fsm_mover * mover)
{
  if(mover->current_state == NULL)
    return NULL;

  return mover->current_state->name;
}

