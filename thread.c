#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <pthread.h>

#define MASTER 0
#define TPOINTS 800
#define MAXSTEPS  10000
#define PI 3.14159265
#define NUMTASKS 4

int	taskid,               /* task ID */
	numtasks,             /* number of processes */
	nsteps,               /* number of time steps */
	npoints,              /* number of points handled by this processor */
	first;                /* index of 1st point handled by this processor */
double	etime,                /* elapsed time in seconds */
	values[TPOINTS+2],  /* values at time t */
	oldval[TPOINTS+2],  /* values at time (t-dt) */
	newval[TPOINTS+2];  /* values at time (t+dt) */


void init_line(void) {
  printf("Post \n");

  int nmin, nleft, npts, i, j, k;
  double x, fac;
  int id = (int)pthread_self();
  printf("id: %d ", id);

  //nmin = TPOINTS/NUMTASKS;
  //nleft = TPOINTS%NUMTASKS;
  //fac = 2.0 * PI;
  //for (i = 0, k = 0; i < NUMTASKS; i++) {
  //  npts = (i < nleft) ? nmin + 1 : nmin;
  //}
}


void *filltab( void *start)
{
  unsigned int i;
  unsigned int *s;
  unsigned int x, left, right;

  s = (unsigned int *)start;
  x = *s;

  init_line();
}

int main(int argc, char *argv[])
{
  unsigned int size;
  unsigned int threads;
  pthread_t *threads_tab;

  unsigned int i;
  unsigned int *thread_args;
  int error;

  nsteps = atoi(argv[1]);
  numtasks = atoi(argv[2]);

  threads_tab = (pthread_t *)malloc(numtasks*sizeof(pthread_t));
  if (!threads) 
    exit(2);

  thread_args = (unsigned int *)malloc(sizeof(unsigned int)*numtasks);
  if (!thread_args) 
    exit(3);

  for(i=0; i<numtasks; i++){
    thread_args[i]=i;
    error = pthread_create( &threads_tab[i], NULL, filltab, (void *)&thread_args[i]);
    if (error) 
      exit(4);
  }

  for(i=0; i<numtasks; i++) 
    pthread_join(threads_tab[i], NULL);

}
