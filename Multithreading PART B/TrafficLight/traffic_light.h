#pragma once
#include <pthread.h>
#include <stdbool.h>
#include "threadlib.h"

#define NUMBER_OF_LIGHTS 3
#define NUMBER_OF_FACES 4

enum Trafic_Light_Color
{
  RED,
  YELLOW,
  GREEN,
  LIGHT_COLOR_MAX
};

enum Trafic_Light_Type
{
  EAST,
  WEST,
  NORTH,
  SOUTH
};

typedef struct traffic_light_face
{
  wait_queue_t wq;
  pthread_mutex_t state_mutex;
  bool light[NUMBER_OF_LIGHTS];

} traffic_light_face_t;

typedef struct traffic_light_
{
  traffic_light_face_t traffic_light_faces[NUMBER_OF_FACES];
} traffic_light_t;

enum th_state_t
{

  TH_RUN_SLOW,   /* on yellow light */
  TH_RUN_NORMAL, /* on green light */
  TH_STOP        /* On red light */
};

typedef struct thread_pvt_data_
{
  enum Trafic_Light_Type th_dirn;
  enum th_state_t thread_state;
  thread_t *thread;
  traffic_light_face_t *tr_light;
} thread_pvt_data_t;

void init_traffic_light_face(traffic_light_face_t *tfl_face);
void set_traffic_light_face(traffic_light_face_t *tfl_face, enum Trafic_Light_Color tfl_color, bool status);
void destroy_traffic_light_face(traffic_light_face_t *tfl_face);

void init_traffic_light(traffic_light_t *tfl);
void set_traffic_light(traffic_light_t *tfl, enum Trafic_Light_Color tfl_color, enum Trafic_Light_Type tfl_type, bool status);
void destroy_traffic_light(traffic_light_t *tfl);