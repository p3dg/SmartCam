/* Linked List Implementation
 *
 * Author: Idan Warsawski
 */

#ifndef LINKED_LIST
#define LINKED_LIST

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LL_FOREACH(head, element) for(element=head; element; element=element->next)

#define LL_FOREACH_SAFE(head, element, tmp) for((element) = (head); (element) && (tmp = (element)->next, 1); (element) = tmp)

struct node;

struct node {
  struct node * next;
  void * data;
};

void linked_list_add_node_tail(struct node ** head, void * data);

void * linked_list_pop_head(struct node ** head);

#ifdef __cplusplus
}
#endif

#endif
