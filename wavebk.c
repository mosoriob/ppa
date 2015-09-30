#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#define MASTER 0
#define PI 3.14159265



int numtasks;
int npoints;
int times;
int debug=0;
pthread_mutex_t* mutex;
pthread_cond_t* cond;

typedef struct thread {   
     pthread_t thread_id;       
     int       thread_num;
     int       thread_step;
     int       thread_first;
} ThreadData;

int **states;
double *values, *oldval, *newval;

int** create_array( int X, int Y ){
  int **array,i;
  array = (int**) malloc( X * sizeof(int*) );
  for(i=0; i< X; i++) 
    array[i] = (int*) malloc( Y* sizeof(int) );
  return array;
}


void update(int id, int first, int step){
  int j;

  double dtime = 0.3;  
  double c = 1.0;
  double dx = 1.0;
  double tau = (c * dtime / dx);
  double sqtau = tau * tau;


  for (int i = 0; i < times; i++){
    for (j = first-1; j <= (first-1+step); j++){
      if(j==(first-2+step)){ 
        pthread_mutex_lock(&mutex[id]);
        states[id][i]=1;
        pthread_mutex_unlock(&mutex[id]);
        pthread_cond_signal(&cond[id]);
      }
      if(j==(first-1) && j!=0){
        while(states[id-1][i]==0){
          pthread_cond_wait(&cond[id-1], &mutex[id-1]);
          states[id-1][i]=1;
        }
      }
    }

    //Condicion de borde izquierda
    if(j==0){
      newval[j] = (2.0 * values[j]) - oldval[j] 
         + (sqtau * (values[j] - (2.0 * values[j]) + values[j+1]));
    }

    //Condicion de borde derecha
    else if(j==npoints-1){
      newval[j] = (2.0 * values[j]) - oldval[j] 
        + (sqtau * (values[j-1] - (2.0 * values[j]) + values[j])); 
    }

    //Caso genericos
    else{
     newval[j] = (2.0 * values[j]) - oldval[j] 
         + (sqtau * (values[j-1] - (2.0 * values[j]) + values[j+1]));
    }

  }
}


void *thread_start(void *thread)
{
  ThreadData *my_data  = (ThreadData*)thread;
  int id=my_data->thread_num;
  int step=my_data->thread_step;
  int first=my_data->thread_first;
  int j;
  double x, fac = 2.0 * PI;

  //Inicia la onda en la sin(x)
  for (j=first; j <= (first+step); j++){
    x = (double)j/(double)(npoints - 1);
    values[j] = sin (fac * x);
  }

  for (j=first; j <= (first+step); j++){
     oldval[j] = values[j];
  }

  //Hacer la iteración de los putnos
  update(id,first,step);

  if(debug)
    printf("Termina el thread %d \n", id);

  return NULL;
}

int main(int argc, char *argv[])
{

  if( argc != 5){
    printf("faltan args\n");
      exit(0);
  }
  else{
      npoints = atoi(argv[1]);
      numtasks = atoi(argv[2]);
      times = atoi(argv[3]);
      debug = atoi(argv[4]);
  }

  ThreadData thread[numtasks]; 
  int i, first, npts;
  int k=0;
  int nmin = npoints/numtasks;
  int nleft = npoints%numtasks;

  mutex = (pthread_mutex_t*) malloc (numtasks*sizeof(pthread_mutex_t));
  cond = (pthread_cond_t*) malloc (numtasks*sizeof(pthread_cond_t));
  values = (double*) malloc (npoints*sizeof(double));
  oldval = (double*) malloc (npoints*sizeof(double));
  newval = (double*) malloc (npoints*sizeof(double));

  int X = numtasks;
  int Y = times;
  states = create_array(X,Y);
  for(int i=0; i< X; i++){
    for(int j=0; j< Y; j++){
      if(j==0){
        states[i][j] = 1;
      }
      states[i][j] = 0;
    }
  }

  for(i=0; i<numtasks; i++){
    int iRC = pthread_mutex_init (&mutex[i], NULL);
    int iRCc = pthread_cond_init (&cond[i], NULL);
    if (iRC != 0 || iRCc !=0){
      printf("Problema al iniciar mutex\n");
      return 0;
    }
  }

  //Calculo de steps
  for(i=0; i<numtasks; i++){
    npts = (i < nleft) ? nmin + 1 : nmin;
    first = k + 1;
    npoints = npts;
    k += npts;
    thread[i].thread_num=i;
    thread[i].thread_step=npoints;
    thread[i].thread_first=first;
    pthread_create(&(thread[i].thread_id), NULL, thread_start, (void *)(thread+i));
  }

  for (i = 0; i < numtasks; i++)
     pthread_join(thread[i].thread_id, NULL); 

  free(mutex);
  free(cond);
  for(int i=0; i< X; i++)
    free(states[i]);
  free(states);
  free(values);
  free(oldval);
  free(newval);

  return 0;
}