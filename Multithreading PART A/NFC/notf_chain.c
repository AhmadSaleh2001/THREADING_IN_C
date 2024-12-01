#include <stdio.h>
#include "gl_thread.h"
#include "notf_element.h"
#include "notf_chain.h"

void print_notification_chain(notf_chain_t *chain)
{
  if (!chain)
    return;

  printf("###### start printing chain ###### \n");
  printf("chain name: %s\n", chain->notification_chain_name);
  gl_thread_t **curr = chain->head;

  while (curr != NULL && (*curr) != NULL)
  {
    print_notification_chain_element(get_notification_chain_element(curr));
    curr = (*curr)->next;
  }
  printf("###### end printing chain ###### \n");
}

void add_first(notf_chain_t **chain, gl_thread_t **new_item)
{

  if (((*chain)->head) == NULL)
  {
    (*chain)->head = new_item;
    return;
  }
  (*new_item)->next = (*chain)->head;
  (*chain)->head = new_item;
}

void do_operation(notf_chain_t **chain, gl_thread_t **new_item, nfc_op_t operation)
{
  if (operation == NFC_ADD)
  {
    add_first(chain, new_item);
  }
  else if (operation == NFC_DEL)
  {
    deleteItem(chain, new_item);
  }
}

gl_thread_t **deleteItemRec(gl_thread_t **curr, gl_thread_t **item)
{
  if (curr == NULL)
    return NULL;
  if (*curr == *item)
  {
    gl_thread_t **nxt = (*curr)->next;

    free(get_notification_chain_element(item));
    free(*item);

    return nxt;
  }
  (*curr)->next = deleteItemRec((*curr)->next, item);
  return curr;
}

void deleteItem(notf_chain_t **chain, gl_thread_t **item)
{
  (*chain)->head = deleteItemRec((*chain)->head, item);
}

notf_chain_t *nfc_create_new_notif_chain(char *notification_chain_name)
{
  notf_chain_t *notf_chain = malloc(sizeof(notf_chain_t));
  notf_chain->notification_chain_name = notification_chain_name;
  return notf_chain;
}