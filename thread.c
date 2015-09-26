#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <pthread.h>

#define MASTER 0
#define TPOINTS 800
#define MAXSTEPS  10000
#define PI 3.14159265
#define NUMTASKS 4

unsigned int step;
double  etime,                /* elapsed time in seconds */
    values[TPOINTS+2],  /* values at time t */
    oldval[TPOINTS+2],  /* values at time (t-dt) */
    newval[TPOINTS+2];  /* values at time (t+dt) */

int *tab;

void init_line(void) {
  printf("Post \n");

  int nmin, nleft, npts, i, j, k;
  double x, fac;
  int id=  pthread_threadid_np();
  printf("id: %d ", id);

  /* calculate initial values based on sine curve */
  nmin = TPOINTS/NUMTASKS;
  nleft = TPOINTS%NUMTASKS;
  fac = 2.0 * PI;
  for (i = 0, k = 0; i < NUMTASKS; i++) {
    npts = (i < nleft) ? nmin + 1 : nmin;

  // if (taskid == i) {
  //      first = k + 1;
  //      npoints = npts;
  //      printf ("task=%3d  first point=%5d  npoints=%4d\n", taskid, 
  //              first, npts);
  //      for (j = 1; j <= npts; j++, k++) {
  //         x = (double)k/(double)(TPOINTS - 1);
  //         values[j] = sin (fac * x);
  //         } 
  //      }
  //   else k += npts;
  //   }
  // for (i = 1; i <= npoints; i++) 
  //   oldval[i] = values[i];
  }
}


void *filltab( void *start)
{
  unsigned int i;
  unsigned int *s;
  unsigned int x, left, right;

  s = (unsigned int *)start;
  x = *s;

  init_line();

  printf("Start=%d, to=%d\n", x, x+step-1);
  for(i=x; i<x+step; i++) 
    tab[i]=i;
  return NULL;
  //usleep(10000);
}

int main(int argc, char *argv[])
{
  unsigned int size;
  unsigned int threads;
  pthread_t *threads_tab;

  unsigned int i;
  unsigned int *thread_args;
  int error;

  size = atoi(argv[1]);
  threads = atoi(argv[2]);

  tab = (int *)calloc( 1, size*sizeof(int) );
  if ( !tab ) exit(1);

  //for(i=0; i<size; i++) printf("%d ", tab[i]);


  threads_tab = (pthread_t *)malloc( threads*sizeof(pthread_t));
  if (!threads) 
    exit(2);

  thread_args = (unsigned int *)malloc(sizeof(unsigned int)*threads);
  if (!thread_args) 
    exit(3);

  step=size / threads;
  
  for(i=0; i<threads; i++){
    thread_args[i]=i*step;
    error = pthread_create( &threads_tab[i], NULL, filltab, (void *)&thread_args[i]);
    if (error) 
      exit(4);
  }

  for(i=0; i<threads; i++) 
    pthread_join(threads_tab[i], NULL);

  //for(i=0; i<size; i++) printf("%d ", tab[i]);
  printf("\n");
}
