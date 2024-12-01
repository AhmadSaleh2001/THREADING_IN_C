#include "gl_thread.h"
#include "notf_element.h"
#include "string.h"

notf_element_t *get_notification_chain_element(gl_thread_t *item)
{
  notf_element_t *ans = (notf_element_t *)((char *)item - offsetof(notf_element_t, glue));

  return ans;
}

void print_notification_chain_element(notf_element_t *item)
{
  printf("key: %s\n", item->key);
  printf("key_size: %d\n", item->key_size);
  printf("is_key_set: %d\n", item->is_key_set);
  item->subscriber_callback(item->subscriber_id);
  printf("\n-----\n");
}

gl_thread_t **create_notf_element(char key[MAX_NOTIFICATION_KEY_SIZE], int key_size, int subscriber_id, subscriber_callback subscriber_callback)
{
  notf_element_t *new_element = malloc(sizeof(notf_element_t));
  memcpy(new_element->key, key, key_size);
  new_element->key_size = key_size;
  new_element->is_key_set = key_size > 0;
  new_element->subscriber_id = subscriber_id;
  new_element->subscriber_callback = subscriber_callback;
  new_element->glue = malloc(sizeof(gl_thread_t));
  new_element->glue->next = NULL;
  return &new_element->glue;
}