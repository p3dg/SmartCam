/* Linked List Implementation
 *
 * Author: Idan Warsawski
 */

#include "linked_list.h"

void linked_list_add_tail(struct node ** head, void * data)
{
  struct node * new_node, * tmp;
  
  new_node = malloc(sizeof(struct node));
  new_node->next = NULL;
  new_node->data = data;


  if(*head == NULL)
    (*head) = new_node;
  else
    {
      tmp = *head;
      while(tmp->next)
        {
          tmp = tmp->next;
        }
      tmp->next = new_node;
    }
}

void * linked_list_pop_head(struct node ** head)
{
  if(*head == NULL)
    return NULL;
  else
    {
      struct node * tmp;
      void * tmp_data;
      tmp = *head;
      *head = tmp->next;
      tmp_data = tmp->data;
      free(tmp);

      return tmp_data;
    }

}
