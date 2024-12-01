#include <pthread.h>
#include "threadlib.h"
#include "traffic_light.h"

void init_traffic_light_face(traffic_light_face_t *tfl_face)
{
  wait_queue_init(&tfl_face->wq, 0);
  pthread_mutex_init(&tfl_face->state_mutex, NULL);
  for (int i = 0; i < NUMBER_OF_LIGHTS; i++)
    tfl_face->light[i] = 0;
}

void set_traffic_light_face(traffic_light_face_t *tfl_face, enum Trafic_Light_Color tfl_color, bool status)
{
  if (!status)
  {
    tfl_face->light[tfl_color] = false;
    return;
  }

  for (int i = 0; i < LIGHT_COLOR_MAX; i++)
  {
    if (i == tfl_color)
    {
      if (i == GREEN)
      {
        tfl_face->light[i] = status;
        wait_queue_broadcast(&tfl_face->wq, false);
      }
      else
      {
        tfl_face->light[i] = status;
      }
    }
    else
    {
      tfl_face->light[i] = 0;
    }
  }
}

void destroy_traffic_light_face(traffic_light_face_t *tfl_face)
{
  wait_queue_destroy(&tfl_face->wq);
  pthread_mutex_destroy(&tfl_face->state_mutex);
  free(tfl_face);
}

void init_traffic_light(traffic_light_t *tfl)
{
  for (int i = 0; i < NUMBER_OF_FACES; i++)
  {
    init_traffic_light_face(&tfl->traffic_light_faces[i]);
  }
}

void set_traffic_light(traffic_light_t *tfl, enum Trafic_Light_Color tfl_color, enum Trafic_Light_Type tfl_type, bool status)
{
  set_traffic_light_face(&tfl->traffic_light_faces[tfl_type], tfl_color, status);
}

void destroy_traffic_light(traffic_light_t *tfl)
{
  for (int i = 0; i < NUMBER_OF_FACES; i++)
  {
    destroy_traffic_light_face(&tfl->traffic_light_faces[i]);
  }
  free(tfl);
}