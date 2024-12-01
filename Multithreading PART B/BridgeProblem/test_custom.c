#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include "rw_lock.h"
#include "bridge.h"

static bridge_t b;
static int n_r = 0;
static int n_w = 0;
pthread_mutex_t state_check_mutex;

static void
cs_status_check()
{

    pthread_mutex_lock(&state_check_mutex);
    assert(n_r >= 0 && n_w >= 0); /* Cannot be negative */

    if (n_r == 0 && n_w == 0)
    {
        // valid condition
    }
    else if (n_r > 0 && n_r <= b.n_LR && n_w == 0)
    {
        // valid condition
    }
    else if (n_r == 0 && n_w > 0 && n_w <= b.n_RL)
    {
        // valid condition
    }
    else
    {
        assert(0);
    }

    printf("n_r = %d, n_w = %d\n", n_r, n_w);
    pthread_mutex_unlock(&state_check_mutex);
}

static void
execute_cs()
{
    cs_status_check();
}

static void
reader_enter_cs()
{
    pthread_mutex_lock(&state_check_mutex);
    n_r++;
    pthread_mutex_unlock(&state_check_mutex);
}

static void
reader_leave_cs()
{
    pthread_mutex_lock(&state_check_mutex);
    n_r--;
    pthread_mutex_unlock(&state_check_mutex);
}

static void
writer_enter_cs()
{
    pthread_mutex_lock(&state_check_mutex);
    n_w++;
    pthread_mutex_unlock(&state_check_mutex);
}

static void
writer_leave_cs()
{
    pthread_mutex_lock(&state_check_mutex);
    n_w--;
    pthread_mutex_unlock(&state_check_mutex);
}

void *
bus_enter_lr(void *arg)
{
    int cnt = 0;
    while (cnt++ < 100)
    {

        bridge_enter_lr(&b);
        reader_enter_cs();

        execute_cs();

        reader_leave_cs();
        bridge_leave_lr(&b);
    }
}

void *
bus_enter_rl(void *arg)
{
    int cnt = 0;
    while (cnt++ < 100)
    {

        bridge_enter_rl(&b);
        writer_enter_cs();

        execute_cs();

        writer_leave_cs();
        bridge_leave_rl(&b);
    }
}

int main(int argc, char **argv)
{

    static pthread_t th1, th2, th3, th4, th5, th6;
    init_bridge(&b, 3, 3);
    pthread_mutex_init(&state_check_mutex, NULL);
    pthread_create(&th1, NULL, bus_enter_lr, NULL);
    pthread_create(&th2, NULL, bus_enter_lr, NULL);
    pthread_create(&th3, NULL, bus_enter_lr, NULL);

    pthread_create(&th4, NULL, bus_enter_rl, NULL);
    pthread_create(&th5, NULL, bus_enter_rl, NULL);
    pthread_create(&th6, NULL, bus_enter_rl, NULL);

    pthread_exit(0);
    return 0;
}