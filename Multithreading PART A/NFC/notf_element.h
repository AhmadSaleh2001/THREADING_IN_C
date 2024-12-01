#pragma once
#include "gl_thread.h"

#define offsetof(structName, fieldName) \
  (unsigned int)&(((structName *)0)->fieldName)

typedef void (*subscriber_callback)(int);

#define MAX_NOTIFICATION_KEY_SIZE 32

typedef enum
{

  NFC_UNKNOWN,

  NFC_ADD,

  NFC_MOD,

  NFC_DEL,

} nfc_op_t;

typedef struct notf_element
{
  char key[MAX_NOTIFICATION_KEY_SIZE];
  int key_size;
  int is_key_set;
  int subscriber_id;
  subscriber_callback subscriber_callback;
  gl_thread_t *glue;
} notf_element_t;

notf_element_t *get_notification_chain_element(gl_thread_t *item);
gl_thread_t **create_notf_element(char key[MAX_NOTIFICATION_KEY_SIZE], int key_size, int subscriber_id, subscriber_callback subscriber_callback);
void print_notification_chain_element(notf_element_t *item);