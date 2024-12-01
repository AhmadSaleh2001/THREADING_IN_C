#pragma once
#include "rt_entry.h"

typedef struct rt_table
{
  rt_entry_t *head;
} rt_table_t;

void add_first_rt_entry(rt_table_t **route_table, rt_entry_t *entry);
void print_rt_table(rt_table_t *route_table);
rt_table_t *create_rt_table();
void subscribe(rt_table_t **route_table, gl_thread_t *element);