#include <stdio.h>
#include "notf_element.h"
#include "notf_chain.h"
#include "rt_entry.h"
#include "rt_table.h"
#include <pthread.h>

rt_table_t *tbl = NULL;
void fn(int subscriber_id)
{
  printf("hello subscriber: %d\n", subscriber_id);
}

rt_table_t **get_rt_table()
{
  if (tbl == NULL)
    tbl = create_rt_table();
  return &tbl;
}

void init_rt_table()
{
  rt_entry_t *t1 = create_rt_entry("192.168.1.1", 16, "th1", "192.168.1.0");
  rt_entry_t *t2 = create_rt_entry("192.168.1.2", 16, "th2", "192.168.1.0");

  add_first_rt_entry(get_rt_table(), t1);
  add_first_rt_entry(get_rt_table(), t2);

  // the remaining is just to add some while 1 in the publisher, to take input from the user
  // add when update something, notify registered threads
}

void create_publisher_thread(pthread_t *publisher)
{
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  pthread_create(publisher, &attr, init_rt_table, NULL);
}

void subscribe_to_rt(void *data)
{
  int thread_id = *((int *)data);
  free(data);
  char ipToTrack[MAX_NOTIFICATION_KEY_SIZE] = "192.168.1.";
  ipToTrack[strlen(ipToTrack)] = ((thread_id & 1) ? 1 : 2) + '0';

  gl_thread_t **i = create_notf_element(ipToTrack, 11, thread_id, fn);
  subscribe(get_rt_table(), i);
}

void create_subscriber_thread(int thread_id)
{
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  int *thread_id_ptr = calloc(1, sizeof(int));
  *thread_id_ptr = thread_id;
  pthread_t thread;
  pthread_create(&thread, &attr, subscribe_to_rt, (void *)thread_id_ptr);
}

int main()
{

  // gl_thread_t **el1 = create_notf_element("ahmad", 5, 1, fn);
  // gl_thread_t **el2 = create_notf_element("ali", 3, 2, fn);
  // gl_thread_t **el3 = create_notf_element("yousef", 6, 3, fn);
  // notf_chain_t *linkedlist = nfc_create_new_notif_chain("ahmad chain");
  // do_operation(&linkedlist, el1, NFC_ADD);
  // do_operation(&linkedlist, el2, NFC_ADD);
  // do_operation(&linkedlist, el3, NFC_ADD);

  // print_notification_chain(linkedlist);
  // printf("\n##########\n");
  // do_operation(&linkedlist, el2, NFC_DEL);
  // print_notification_chain(linkedlist);
  // printf("\n##########\n");

  pthread_t publisher;
  create_publisher_thread(&publisher);
  void *res;
  pthread_join(publisher, &res);
  create_subscriber_thread(1);
  create_subscriber_thread(2);
  print_rt_table(*get_rt_table());
  pthread_exit(0);

  return 0;
}