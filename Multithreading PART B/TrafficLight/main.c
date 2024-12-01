#include <stdio.h>
#include <stdbool.h>
#include "traffic_light.h"

// Function to get the traffic light type from the user
enum Trafic_Light_Type get_traffic_light_type()
{
  int tfl_type_input;
  printf("Select traffic light type:\n");
  printf("0 - EAST\n");
  printf("1 - WEST\n");
  printf("2 - NORTH\n");
  printf("3 - SOUTH\n");
  printf("Enter your choice: ");
  scanf("%d", &tfl_type_input);

  if (tfl_type_input < 0 || tfl_type_input > 3)
  {
    printf("Invalid traffic light type. Exiting.\n");
    return -1; // Invalid type
  }
  return (enum Trafic_Light_Type)tfl_type_input;
}

// Function to get the traffic light color from the user
enum Trafic_Light_Color get_traffic_light_color()
{
  int tfl_color_input;
  printf("Select traffic light color:\n");
  printf("0 - RED\n");
  printf("1 - YELLOW\n");
  printf("2 - GREEN\n");
  printf("Enter your choice: ");
  scanf("%d", &tfl_color_input);

  if (tfl_color_input < 0 || tfl_color_input > 2)
  {
    printf("Invalid traffic light color. Exiting.\n");
    return -1; // Invalid color
  }
  return (enum Trafic_Light_Color)tfl_color_input;
}

static void
file_write(char *buff)
{

  static FILE *fptr = NULL;

  if (!fptr)
  {
    fptr = fopen("log", "w");
  }

  fwrite(buff, sizeof(char), strlen(buff), fptr);
  fflush(fptr);
}

static char log_buff[256];

static bool
thread_block_on_tr_light(void *arg, pthread_mutex_t **mutex)
{

  thread_pvt_data_t *pvt_data = (thread_pvt_data_t *)arg;

  if (mutex)
  {
    *mutex = &pvt_data->tr_light->state_mutex;
    pthread_mutex_lock(*mutex);
  }

  return pvt_data->tr_light->light[RED];
}

static void *
thread_move(void *arg)
{

  thread_pvt_data_t *pvt_data = (thread_pvt_data_t *)arg;
  thread_t *th = pvt_data->thread;
  traffic_light_face_t *tr_light = pvt_data->tr_light;
  enum Trafic_Light_Type dirn = pvt_data->th_dirn;

  while (1)
  {

    wait_queue_test_and_wait(&tr_light->wq, thread_block_on_tr_light, arg);
    /* C.S after unblock from Wait Queue */
    sprintf(log_buff, "Thread %s Running \n", pvt_data->thread->name);
    file_write(log_buff);
    pthread_mutex_unlock(&tr_light->state_mutex);
    sleep(2);
  }
}

int main()
{

  thread_t *th;
  traffic_light_t *traffic_light;

  traffic_light = calloc(1, sizeof(traffic_light_t));
  init_traffic_light(traffic_light);

  th = thread_create(0, "TH_EAST1");
  thread_pvt_data_t *pvt_data = calloc(1, sizeof(thread_pvt_data_t));
  pvt_data->th_dirn = EAST;
  pvt_data->thread_state = TH_RUN_NORMAL;
  pvt_data->thread = th;
  pvt_data->tr_light = &traffic_light->traffic_light_faces[EAST];
  thread_run(th, thread_move, (void *)pvt_data);

#if 1
  th = thread_create(0, "TH_EAST2");
  pvt_data = calloc(1, sizeof(thread_pvt_data_t));
  pvt_data->th_dirn = EAST;
  pvt_data->thread_state = TH_RUN_NORMAL;
  pvt_data->thread = th;
  pvt_data->tr_light = &traffic_light->traffic_light_faces[EAST];
  thread_run(th, thread_move, (void *)pvt_data);

  th = thread_create(0, "TH_WEST1");
  pvt_data = calloc(1, sizeof(thread_pvt_data_t));
  pvt_data->th_dirn = WEST;
  pvt_data->thread_state = TH_RUN_NORMAL;
  pvt_data->thread = th;
  pvt_data->tr_light = &traffic_light->traffic_light_faces[WEST];
  thread_run(th, thread_move, (void *)pvt_data);

  th = thread_create(0, "TH_WEST2");
  pvt_data = calloc(1, sizeof(thread_pvt_data_t));
  pvt_data->th_dirn = WEST;
  pvt_data->thread_state = TH_RUN_NORMAL;
  pvt_data->thread = th;
  pvt_data->tr_light = &traffic_light->traffic_light_faces[WEST];
  thread_run(th, thread_move, (void *)pvt_data);
#endif

  while (1)
  {
    // Get traffic light type from user
    enum Trafic_Light_Type tfl_type = get_traffic_light_type();
    if (tfl_type == -1)
    {
      destroy_traffic_light(&traffic_light);
      return 1;
    }

    // Get traffic light color from user
    enum Trafic_Light_Color tfl_color = get_traffic_light_color();
    if (tfl_color == -1)
    {
      destroy_traffic_light(&traffic_light);
      return 1;
    }

    // Set the traffic light based on user input
    set_traffic_light(traffic_light, tfl_color, tfl_type, true);
    printf("for type: %d\n", tfl_type);
    for (int i = 0; i < NUMBER_OF_LIGHTS; i++)
    {
      printf("status: %d => %d\n", i, traffic_light->traffic_light_faces[tfl_type].light[i]);
    }

    // Display the result
    printf("Traffic light set: Type = %d, Color = %d, Status = ON\n", tfl_type, tfl_color);
  }

  // // Clean up
  // destroy_traffic_light(&traffic_light);
  return 0;
}
