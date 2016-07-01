#include <iostream>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
using namespace std;

typedef struct mutex_thread_param_t
{
  pthread_mutex_t *mutex_ptr;
  unsigned long long usleep_time;

} mutex_thread_param_t;

typedef struct spinlock_thread_param_t
{
  pthread_spinlock_t *spinlock_ptr;
  unsigned long long usleep_time;

} spinlock_thread_param_t;

typedef struct rwlock_thread_param_t
{
  pthread_rwlock_t *rwlock_ptr;
  unsigned long long usleep_time;

} rwlock_thread_param_t;


void *mutex_thread(void *arg)
{
  mutex_thread_param_t *mtp = (mutex_thread_param_t *) arg;
  for(int i = 0; i < 10; i++)
  {
    pthread_mutex_lock(mtp->mutex_ptr);
    usleep(mtp->usleep_time);
    pthread_mutex_unlock(mtp->mutex_ptr);
  }
  return NULL;
}

void *spinlock_thread(void *arg)
{
  spinlock_thread_param_t *sltp = (spinlock_thread_param_t *) arg;
  for(int i = 0; i < 10; i++)
  {
    pthread_spin_lock(sltp->spinlock_ptr);
    usleep(sltp->usleep_time);
    pthread_spin_unlock(sltp->spinlock_ptr);
  }
  return NULL;
}

void *rdlock_thread(void *arg)
{
  rwlock_thread_param_t *rwltp = (rwlock_thread_param_t *) arg;
  for(int i = 0; i < 10; i++)
  {
    pthread_rwlock_rdlock(rwltp->rwlock_ptr);
    usleep(rwltp->usleep_time);
    pthread_rwlock_unlock(rwltp->rwlock_ptr);
  }
  return NULL;
}

void *wrlock_thread(void *arg)
{
  rwlock_thread_param_t *rwltp = (rwlock_thread_param_t *) arg;
  for(int i = 0; i < 10; i++)
  {
    pthread_rwlock_wrlock(rwltp->rwlock_ptr);
    usleep(rwltp->usleep_time);
    pthread_rwlock_unlock(rwltp->rwlock_ptr);
  }
  return NULL;
}

void save_pid_to_file()
{
  pid_t pid = getpid();
  FILE *fp = fopen("/home/box/main.pid", "w");
  if(fp != NULL)
  {
    fprintf(fp, "%d", (int) pid);
    fclose(fp);
  }
}

int main(int argc, char *argv[])
{
  save_pid_to_file();

  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  mutex_thread_param_t mut_params;
  mut_params.mutex_ptr = &mutex;
  mut_params.usleep_time = 500000;
  pthread_t mut_id_1;
  pthread_t mut_id_2;
  pthread_create(&mut_id_1, NULL, mutex_thread, &mut_params);
  pthread_create(&mut_id_2, NULL, mutex_thread, &mut_params);


  pthread_spinlock_t spin;
  pthread_spin_init(&spin, PTHREAD_PROCESS_PRIVATE);
  spinlock_thread_param_t spin_params;
  spin_params.spinlock_ptr = &spin;
  spin_params.usleep_time = 500000;
  pthread_t spin_id_1;
  pthread_t spin_id_2;
  pthread_create(&spin_id_1, NULL, spinlock_thread, &spin_params);
  pthread_create(&spin_id_2, NULL, spinlock_thread, &spin_params);


  pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
  rwlock_thread_param_t rwlock_params;
  rwlock_params.rwlock_ptr = &rwlock;
  rwlock_params.usleep_time = 500000;
  pthread_t rd_id;
  pthread_t wr_id;
  pthread_create(&rd_id, NULL, rdlock_thread, &rwlock_params);
  pthread_create(&wr_id, NULL, wrlock_thread, &rwlock_params);

  pthread_join(mut_id_1, NULL);
  pthread_join(mut_id_2, NULL);

  pthread_join(spin_id_1, NULL);
  pthread_join(spin_id_2, NULL);

  pthread_join(rd_id, NULL);
  pthread_join(wr_id, NULL);

  return 0;
}
