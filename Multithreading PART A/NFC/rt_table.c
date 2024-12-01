#include <stdio.h>
#include <string.h>
#include "notf_chain.h"
#include "rt_entry.h"
#include "rt_table.h"

void add_first_rt_entry(rt_table_t **route_table, rt_entry_t *entry)
{
  if ((*route_table)->head == NULL)
  {
    (*route_table)->head = entry;
    return;
  }
  entry->next = (*route_table)->head;
  (*route_table)->head = entry;
}

void print_rt_table(rt_table_t *route_table)
{
  rt_entry_t *head = route_table->head;
  while (head)
  {
    print_rt_entry(head);
    print_notification_chain(head->notf_chain);
    head = head->next;
    printf("\n");
  }
}

rt_table_t *create_rt_table()
{
  rt_table_t *tbl = malloc(sizeof(rt_table_t));
  return tbl;
}

void subscribe(rt_table_t **route_table, gl_thread_t *element)
{
  rt_entry_t *curr = (*route_table)->head;
  notf_element_t *item = get_notification_chain_element(element);
  while (curr)
  {

    if (strcmp(curr->dest, item->key) == 0)
    {
      do_operation(&curr->notf_chain, element, NFC_ADD);
      break;
    }
    curr = curr->next;
  }
}