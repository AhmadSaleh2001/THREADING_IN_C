#pragma once
#include "gl_thread.h"
#include "notf_element.h"

typedef struct notf_chain
{
  char *notification_chain_name;
  gl_thread_t **head;
} notf_chain_t;

void print_notification_chain(notf_chain_t *chain);
void do_operation(notf_chain_t **chain, gl_thread_t **new_item, nfc_op_t operation);
notf_chain_t *nfc_create_new_notif_chain(char *notification_chain_name);