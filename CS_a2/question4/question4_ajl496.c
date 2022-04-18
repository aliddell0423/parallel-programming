#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

struct arguments {
  int *counter;
  int *total_changes;
  pthread_mutex_t *mutex;
  pthread_cond_t *cond;
};

void *increment_work( void *arg );
void *decrement_work( void *arg );

int main() {

  pthread_t worker_thread1;
  pthread_t worker_thread2;
  pthread_mutex_t lock;
  pthread_cond_t cond;
  struct arguments *arg;
  pthread_mutex_init ( &lock, NULL);
  pthread_cond_init ( &cond, NULL);

  int counter = 0;
  int total_changes = 0;

  arg = ( struct arguments * )calloc( 1, sizeof( struct arguments ) );
  arg->counter = &counter;
  arg->total_changes = &total_changes;
  arg->mutex = &lock;
  arg->cond = &cond;

  // Create thread
  if ( pthread_create( &worker_thread1, NULL,
                       increment_work, ( void * ) arg ) ) {
    fprintf( stderr,"Error while creating thread\n" );
    exit( 1 );
  }

  if ( pthread_create( &worker_thread2, NULL,
                       decrement_work, ( void * ) arg ) ) {
    fprintf( stderr,"Error while creating thread\n" );
    exit( 1 );
  }

  if ( pthread_join( worker_thread1, NULL ) ) {
    fprintf( stderr,"Error while joining with child thread\n" );
    exit( 1 );
  }

  if ( pthread_join( worker_thread2, NULL ) ) {
    fprintf( stderr,"Error while joining with child thread\n" );
    exit( 1 );
  }

  exit( 0 );

  return 0;
}

void *increment_work( void *arg ) {

  struct arguments *argument;
  argument = ( struct arguments *) arg;

  int *counter = argument->counter;
  int *total_changes = argument->total_changes;
  pthread_mutex_t *mutex = argument->mutex;
  pthread_cond_t *cond = argument->cond;

  while( *total_changes < 50 )
    {
      pthread_mutex_lock( mutex );

      while( *counter < 10 )
      {
        *counter += 1;
        *total_changes += 1;
        printf( "Count is now (inc fn): %d\n", *counter );
      }

      pthread_cond_signal( cond );

      pthread_mutex_unlock( mutex );
    }
  return NULL;
}

void *decrement_work( void *arg ) {

  struct arguments *argument;
  argument = ( struct arguments *) arg;

  int *counter = argument->counter;
  int *total_changes = argument->total_changes;
  pthread_mutex_t *mutex = argument->mutex;
  pthread_cond_t *cond = argument->cond;

  while( *total_changes < 50 )
    {
      pthread_mutex_lock( mutex );

      pthread_cond_wait( cond, mutex );

      while( *counter > 0 && *total_changes < 50 )
      {
        *counter -= 1;
        *total_changes += 1;
        printf( "Count is now (dec fn): %d\n", *counter );
      }

      pthread_mutex_unlock( mutex );
    }
  return NULL;
}
