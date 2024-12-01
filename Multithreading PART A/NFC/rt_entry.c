#include "rt_entry.h"
#include "notf_chain.h"
#include <string.h>

void print_rt_entry(rt_entry_t *r)
{
  printf("dest: %s\n", r->dest);
  printf("mask: %d\n", r->mask);
  printf("output interface: %s\n", r->oif);
  printf("gateway: %s\n", r->gateway);
}

rt_entry_t *create_rt_entry(char dest[MAX_ADDRESS], int mask, char oif[MAX_ADDRESS], char gateway[MAX_ADDRESS])
{
  rt_entry_t *entry = malloc(sizeof(rt_entry_t));
  memcpy(entry->dest, dest, MAX_ADDRESS);
  entry->mask = mask;
  memcpy(entry->oif, oif, MAX_ADDRESS);
  memcpy(entry->gateway, gateway, MAX_ADDRESS);
  entry->notf_chain = nfc_create_new_notif_chain((char *)dest);
  return entry;
}