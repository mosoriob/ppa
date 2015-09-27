#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#define MASTER 0
#define TPOINTS 800
#define MAXSTEPS  10000
#define PI 3.14159265
#define NUMTASKS 4

double  etime,                /* elapsed time in seconds */
  values[TPOINTS+2],  /* values at time t */
  oldval[TPOINTS+2],  /* values at time (t-dt) */
  newval[TPOINTS+2];  /* values at time (t+dt) */


pthread_mutex_t* p_DynamicMutex;

typedef struct thread {   
     pthread_t thread_id;       
     int       thread_num;
     int       thread_step;
     int       thread_first;
} ThreadData;

void update(int id, int first, int step){
  //printf("Tengo que updatear los puntos %d a %d \n", first, (first+step) );
  printf("Thread %d, p.n. %d p.c.  %d \n", id, first-1, first);
  int j, mutex_exit;
  printf("first= %d - %d \n", first-1, (first+step-2));
  for (j = first-1; j <= (first-1+step); j++) {
     if (j == 0 || j == (TPOINTS-1)){
      printf("valores zero\n");
     }
     else{
      if(j==(first-1)){
        printf("DEPENDENCIA con %d - haciendo un lock %d \n", j-1,id-1);
        mutex_exit = pthread_mutex_lock(&p_DynamicMutex[id-1]);
        if (mutex_exit != 0)
          printf("mutex %d fail\n", id-1);
        else
          printf("mutex %d correcto\n", id-1);

        mutex_exit = pthread_mutex_unlock(&p_DynamicMutex[id-1]);
        if (mutex_exit != 0)
          printf("mutex %d fail\n", id-1);
        else
          printf("mutex desbloqueado %d correcto\n", id-1);
        
      }
     }
   }
        /* Use wave equation to update points */
       // newval[j] = (2.0 * values[j]) - oldval[j]
       //    + (sqtau * (values[j-1] - (2.0 * values[j]) + values[j+1]));
    
  // for (j = 1; j <= npoints; j++) {
  //    oldval[j] = values[j];
  //    values[j] = newval[j];
  //    }
  // }
  //printf("Punto para el vecino %d \n", step);

}
void *thread_start(void *thread)
{
 ThreadData *my_data  = (ThreadData*)thread;
 int id=my_data->thread_num;
 int step=my_data->thread_step;
 int first=my_data->thread_first;
 int left, right;
 int j;
 double x, fac = 2.0 * PI;

  for (j=first; j <= (first+step); j++){
    x = (double)j/(double)(TPOINTS - 1);
    values[j] = sin (fac * x);
  }
  update(id,first,step);

  if (id == NUMTASKS-1)
    right = 0;
  else
    right = id + 1;

  if (id == 0)
   left = NUMTASKS - 1;
  else
    left = id - 1;

 printf("thread %d: step=%d, left: %d, right: %d \n", id, step, left, right);
 return NULL;
}

int main(int argc, char *argv[])
{
  int i;
  ThreadData thread[NUMTASKS]; 
  int nmin = TPOINTS/NUMTASKS;
  int nleft = TPOINTS%NUMTASKS;
  int k=0, npoints;
  int first, npts;
  p_DynamicMutex = (pthread_mutex_t*) malloc (NUMTASKS*sizeof(pthread_mutex_t));
  
  for(i=0; i<NUMTASKS; i++){
    int iRC = pthread_mutex_init (&p_DynamicMutex[i], NULL);
    if (iRC != 0)
    {
      printf("fail\n");
      return 0;
    }
  }

  for(i=0; i<NUMTASKS; i++)
  {
    npts = (i < nleft) ? nmin + 1 : nmin;
    first = k + 1;
    npoints = npts;
    k += npts;
    thread[i].thread_num=i;
    thread[i].thread_step=npoints;
    thread[i].thread_first=first;
    pthread_create(&(thread[i].thread_id), NULL, thread_start, (void *)(thread+i));
  }

  //wait for all threads
  for (i = 0; i < NUMTASKS; i++)
     pthread_join(thread[i].thread_id, NULL); 

  //for (i = 0; i < NUMTASKS; i++)
  //   printf(" %i)thread: number %i\n",i,thread[i].thread_num);
  return 0;
}