#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include "student_list.h"
#include "refcount.h"

#if 0

 Signs that this program is erroneous :
 1. Any threads goes in Deadlock ( you will cease to see output from 1 or more threads )
 2. Marks of student update by more than INCR
 3. Any crash
 4. New Read/Write Operation is performed on a student whose deletion is deferred
 5. Student after "delete deferred" , not actually deleted
 6. Memory Leak ( of student objects )
 7. Ref Count value of student object goes too high ( more than 4 )

#endif

#define MAX_ROLL_NO 10
#define INCR 5

enum
{
    READER_TH,
    UPDATE_TH,
    CREATE_TH,
    DELETE_TH,
    THREAD_TYPE_MAX
} th_type_t;

static pthread_t threads[THREAD_TYPE_MAX];
static uint32_t loop_count[THREAD_TYPE_MAX];

static stud_lst_t stud_lst; /* Container Object */

static void *
reader_fn(void *arg)
{

    while (1)
    {
        pthread_rwlock_rdlock(&stud_lst.rw_lock);
        int rnd = rand() % MAX_ROLL_NO;
        student_t *s = student_lst_lookup(&stud_lst, rnd);
        if (s)
        {
            thread_using_object(&s->ref_count);
            pthread_rwlock_rdlock(&s->rw_lock);

            printf("Student with %d info:\n", rnd);
            printf("READ TH  ::  Roll No %u is READ, total marks = %u\n", rnd, s->total_marks);
            printf("\n----------\n");

            pthread_rwlock_unlock(&s->rw_lock);
            if (thread_using_object_done(&s->ref_count))
            {
                printf("Student %d will be freed from the memory\n", rnd);
                student_destroy(s);
            }
        }
        else
        {
            printf("Not able to find student with %d\n", rnd);
        }
        pthread_rwlock_unlock(&stud_lst.rw_lock);
    }

    return NULL;
}

static void *
update_fn(void *arg)
{

    while (1)
    {
        pthread_rwlock_rdlock(&stud_lst.rw_lock);
        int rnd = rand() % MAX_ROLL_NO;
        int newMarkVal = rand() % 100000;
        student_t *s = student_lst_lookup(&stud_lst, rnd);
        if (s)
        {
            thread_using_object(&s->ref_count);
            pthread_rwlock_wrlock(&s->rw_lock);

            printf("Student with %d will be updated\n", rnd);

            printf("\n----------\n");
            s->total_marks = newMarkVal;

            pthread_rwlock_unlock(&s->rw_lock);
            if (thread_using_object_done(&s->ref_count))
            {
                printf("Student %d will be freed from the memory\n", rnd);
                student_destroy(s);
            }
        }
        else
        {
            printf("Not able to find student with %d to update it\n", rnd);
        }
        pthread_rwlock_unlock(&stud_lst.rw_lock);
    }

    return NULL;
}

static void *
create_fn(void *arg)
{
    while (1)
    {
        pthread_rwlock_wrlock(&stud_lst.rw_lock);
        int rnd = rand() % MAX_ROLL_NO;

        student_t *s = malloc(sizeof(student_t));
        s->roll_no = rnd;
        s->total_marks = rnd + 1;
        ref_count_init(&s->ref_count);
        ref_count_inc(&s->ref_count);

        if (s)
        {
            thread_using_object(&s->ref_count);
            pthread_rwlock_wrlock(&s->rw_lock);

            if (student_lst_insert(&stud_lst, s))
            {
                printf("Student with %d will be added, his info:\n", rnd);
                printf("\n----------\n");
            }
            else
            {
                printf("Failed to add student with %d\n", rnd);
            }

            pthread_rwlock_unlock(&s->rw_lock);
            if (thread_using_object_done(&s->ref_count))
            {
                printf("Student %d will be freed from the memory\n", rnd);
                student_destroy(s);
            }
        }
        pthread_rwlock_unlock(&stud_lst.rw_lock);
    }
    return NULL;
}

static void *
delete_fn(void *arg)
{
    while (1)
    {
        pthread_rwlock_wrlock(&stud_lst.rw_lock);
        int rnd = rand() % MAX_ROLL_NO;

        student_t *s = student_lst_remove(&stud_lst, rnd);
        if (s)
        {
            thread_using_object(&s->ref_count);
            pthread_rwlock_wrlock(&s->rw_lock);

            printf("DELETE::");
            printf("Student with %d will be deleted, his info:\n", rnd);
            printf("\n----------\n");

            pthread_rwlock_unlock(&s->rw_lock);
            if (thread_using_object_done(&s->ref_count))
            {
                printf("Student %d will be freed from the memory\n", rnd);
                student_destroy(s);
            }
        }
        else
        {
            printf("Not able to find student with id to delete it %d\n", rnd);
        }
        pthread_rwlock_unlock(&stud_lst.rw_lock);
    }

    return NULL;
}

int main(int argc, char **argv)
{

    stud_lst.lst = init_singly_ll();
    pthread_rwlock_init(&stud_lst.rw_lock, NULL);

    loop_count[READER_TH] = 0;
    loop_count[UPDATE_TH] = 0;
    loop_count[CREATE_TH] = 0;
    loop_count[DELETE_TH] = 0;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_create(&threads[READER_TH], &attr, reader_fn, NULL);
    pthread_create(&threads[UPDATE_TH], &attr, update_fn, NULL);
    pthread_create(&threads[CREATE_TH], &attr, create_fn, NULL);
    pthread_create(&threads[DELETE_TH], &attr, delete_fn, NULL);

    pthread_exit(0);
    return 0;
}
