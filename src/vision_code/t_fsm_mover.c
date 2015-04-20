/* Testbench for finite state machine
 *
 * Author: Peter Galvin
 */
#include <stdio.h>
#include "fsm_mover.h"

static char * foo[] = {"state1", "state2", "state3", "state4", "state5", "state6", "state7"};

int main()
{
  struct fsm_mover * fsm = fsm_init_machine();
  fsm_add_state(fsm, "bb1", foo[0]);
  fsm_add_state(fsm, "bb2", foo[1]);
  fsm_add_state(fsm, "bb3", foo[2]);
  fsm_add_state(fsm, "bb1_2", foo[3]);
  fsm_add_state(fsm, "bb2_1", foo[4]);
  fsm_add_state(fsm, "bb2_3", foo[5]);
  fsm_add_state(fsm, "bb3_2", foo[6]);


  fsm_set_state_jump(fsm, "bb1", NULL, "bb1_2");
  fsm_set_state_jump(fsm, "bb1_2", "bb1", "bb2");

  fsm_set_state_jump(fsm, "bb2", "bb2_1", "bb2_3");
  fsm_set_state_jump(fsm, "bb2_1", "bb1", "bb2");
  fsm_set_state_jump(fsm, "bb2_3", "bb2", "bb3");

  fsm_set_state_jump(fsm, "bb3", "bb3_2", NULL);
  fsm_set_state_jump(fsm, "bb3_2", "bb2", "bb3");

  printf("Init FSM Machine\n");
  printf("Current state %s %s\n", fsm_get_current_state_name(fsm), (char *) fsm_get_current_state_data(fsm));
  fsm_move_right(fsm);
  printf("Jump Right\t");
  printf("Current state %s %s\n", fsm_get_current_state_name(fsm), (char *) fsm_get_current_state_data(fsm));
  fsm_move_right(fsm);
  printf("Jump Right\t");
  printf("Current state %s %s\n", fsm_get_current_state_name(fsm), (char *) fsm_get_current_state_data(fsm));
  fsm_move_right(fsm);
  printf("Jump Right\t");
  printf("Current state %s %s\n", fsm_get_current_state_name(fsm), (char *) fsm_get_current_state_data(fsm));
  fsm_move_right(fsm);
  printf("Jump Right\t");
  printf("Current state %s %s\n", fsm_get_current_state_name(fsm), (char *) fsm_get_current_state_data(fsm));
  fsm_move_right(fsm);
  printf("Jump Right\t");
  printf("Current state %s %s\n", fsm_get_current_state_name(fsm), (char *) fsm_get_current_state_data(fsm));
  fsm_move_right(fsm);
  printf("Jump Right\t");
  printf("Current state %s %s\n", fsm_get_current_state_name(fsm), (char *) fsm_get_current_state_data(fsm));
  fsm_move_right(fsm);
  printf("Jump Right\t");
  printf("Current state %s %s\n", fsm_get_current_state_name(fsm), (char *) fsm_get_current_state_data(fsm));
  fsm_move_right(fsm);
  printf("Jump Right\t");
  printf("Current state %s %s\n", fsm_get_current_state_name(fsm), (char *) fsm_get_current_state_data(fsm));
  fsm_move_left(fsm);
  printf("Jump Left\t");
  printf("Current state %s %s\n", fsm_get_current_state_name(fsm), (char *) fsm_get_current_state_data(fsm));
  fsm_move_left(fsm);
  printf("Jump Left\t");
  printf("Current state %s %s\n", fsm_get_current_state_name(fsm), (char *) fsm_get_current_state_data(fsm));
  fsm_move_left(fsm);
  printf("Jump Left\t");
  printf("Current state %s %s\n", fsm_get_current_state_name(fsm), (char *) fsm_get_current_state_data(fsm));
  fsm_move_left(fsm);
  printf("Jump Left\t");
  printf("Current state %s %s\n", fsm_get_current_state_name(fsm), (char *) fsm_get_current_state_data(fsm));
  fsm_move_left(fsm);
  printf("Jump Left\t");
  printf("Current state %s %s\n", fsm_get_current_state_name(fsm), (char *) fsm_get_current_state_data(fsm));

  return 0;
    

}
