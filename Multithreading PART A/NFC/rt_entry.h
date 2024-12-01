#pragma once
#include "notf_chain.h"

#define MAX_ADDRESS 32

typedef struct rt_entry
{
  char dest[MAX_ADDRESS];
  int mask;
  char oif[MAX_ADDRESS];
  char gateway[MAX_ADDRESS];

  struct rt_entry *next;
  notf_chain_t *notf_chain;
} rt_entry_t;

void print_rt_entry(rt_entry_t *r);
rt_entry_t *create_rt_entry(char dest[MAX_ADDRESS], int mask, char oif[MAX_ADDRESS], char gateway[MAX_ADDRESS]);