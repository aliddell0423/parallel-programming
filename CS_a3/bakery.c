#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

void random_sleep(double a, double b);
void *do_left_work(void *arg);
void *do_right_work(void *arg);
void *do_cautious_work(void *arg);

#define NUM_ITERATIONS 10
#define NUM_LEFT_OVEN_MITTS 3
#define NUM_RIGHT_OVEN_MITTS 3
#define LEFT_OVEN_BAKER 0
#define RIGHT_OVEN_BAKER 1
#define CAUTIOUS_OVEN_BAKER 2



//Helper function to sleep a random number of microseconds
//picked between two bounds (provided in seconds)
//pass 0.2 and 0.5 into this function as arguments
void random_sleep(double lbound_sec, double ubound_sec) {
   int num_usec;
   num_usec = lbound_sec*1000000 +
              (int)((ubound_sec - lbound_sec)*1000000 * ((double)(rand()) / RAND_MAX));
   usleep(num_usec);
   return;
}

struct arguments {
  pthread_mutex_t *left_mutex;
  pthread_mutex_t *right_mutex;
  int baker_type;
  int baker_id;
  int *left_mittens;
  int *right_mittens;
  pthread_cond_t *left_mit_available;
  pthread_cond_t *right_mit_available;
};

/*
 * Main function
 */
int main(int argc, char **argv) {

  int num_left_handed_bakers;
  int num_right_handed_bakers;
  int num_cautious_bakers;
  int seed;

  /* Process command-line arguments */
  if (argc != 5) {
    fprintf(stderr,"Usage: %s <# left-handed bakers> <# right-handed bakers> <# cautious bakers> <seed>\n",argv[0]);
    exit(1);
  }

  if ((sscanf(argv[1],"%d",&num_left_handed_bakers) != 1) ||
      (sscanf(argv[2],"%d",&num_right_handed_bakers) != 1) ||
      (sscanf(argv[3],"%d",&num_cautious_bakers) != 1) ||
      (sscanf(argv[4],"%d",&seed) != 1) ||
      (num_left_handed_bakers < 1) ||
      (num_right_handed_bakers < 1) ||
      (num_cautious_bakers < 1) ||
      (seed < 0)) {
    fprintf(stderr,"Invalid command-line arguments... Aborting\n");
    exit(1);
  }

  /* Seed the RNG */
  srand(seed);

  // init mutex and number of bakers
  int left_mittens = NUM_LEFT_OVEN_MITTS;
  int right_mittens = NUM_RIGHT_OVEN_MITTS;
  int total_bakers = num_left_handed_bakers +  \
    num_right_handed_bakers +                  \
    num_cautious_bakers;
  pthread_mutex_t left_lock;
  pthread_mutex_t right_lock;
  pthread_mutex_init( &left_lock, NULL );
  pthread_mutex_init( &right_lock, NULL );
  pthread_t left_baker[num_left_handed_bakers];
  pthread_t right_baker[num_right_handed_bakers];
  pthread_t cautious_baker[num_cautious_bakers];
  pthread_cond_t left_mit_available;
  pthread_cond_t right_mit_available;
  pthread_cond_init ( &left_mit_available, NULL);
  pthread_cond_init ( &right_mit_available, NULL);
  struct arguments *left_arg[num_left_handed_bakers];
  struct arguments *right_arg[num_right_handed_bakers];
  struct arguments *cautious_arg[num_cautious_bakers];

  // begin for loop to initialize threads
  for( int i = 0; i < total_bakers; i = i + 1 )
    {
    if( i < num_left_handed_bakers )
      {
        left_arg[i] = ( struct arguments * )calloc( 1, sizeof( struct arguments ) );
        left_arg[i]->left_mutex = &left_lock;
        left_arg[i]->right_mutex = &right_lock;
        left_arg[i]->baker_type = LEFT_OVEN_BAKER;
        left_arg[i]->baker_id = i;
        left_arg[i]->left_mit_available = &left_mit_available;
        left_arg[i]->right_mit_available = &right_mit_available;
        left_arg[i]->left_mittens = &left_mittens;
        left_arg[i]->right_mittens = &right_mittens;

        if ( pthread_create( &left_baker[i], NULL,
                             do_left_work, ( void * )left_arg[i])) {
          fprintf( stderr,"Error while creating thread\n" );
          exit( 1 );
        }
      }
    if( i < num_right_handed_bakers )
      {
        right_arg[i] = ( struct arguments * )calloc( 1, sizeof( struct arguments ) );
        right_arg[i]->left_mutex = &left_lock;
        right_arg[i]->right_mutex = &right_lock;
        right_arg[i]->baker_type = RIGHT_OVEN_BAKER;
        right_arg[i]->baker_id = i;
        right_arg[i]->left_mit_available = &left_mit_available;
        right_arg[i]->right_mit_available = &right_mit_available;
        right_arg[i]->left_mittens = &left_mittens;
        right_arg[i]->right_mittens = &right_mittens;

        if ( pthread_create( &right_baker[i], NULL,
                             do_right_work, ( void * )right_arg[i])) {
          fprintf( stderr,"Error while creating thread\n" );
          exit( 1 );
        }
      }
    if( i < num_cautious_bakers )
      {
        cautious_arg[i] = ( struct arguments * )calloc( 1, sizeof( struct arguments ) );
        cautious_arg[i]->left_mutex = &left_lock;
        cautious_arg[i]->right_mutex = &right_lock;
        cautious_arg[i]->baker_type = CAUTIOUS_OVEN_BAKER;
        cautious_arg[i]->baker_id = i;
        cautious_arg[i]->left_mit_available = &left_mit_available;
        cautious_arg[i]->right_mit_available = &right_mit_available;
        cautious_arg[i]->left_mittens = &left_mittens;
        cautious_arg[i]->right_mittens = &right_mittens;

        if ( pthread_create( &cautious_baker[i], NULL,
                              do_cautious_work, ( void * )cautious_arg[i])) {
          fprintf( stderr,"Error while creating thread\n" );
          exit( 1 );
        }
      }
    }

  // join threads
  for( int i = 0; i < total_bakers; i = i + 1 )
    {
      if( i < num_left_handed_bakers )
        {
          // Join with thread
          if ( pthread_join( left_baker[i], NULL ) ) {
            fprintf( stderr,"Error while joining with child thread\n" );
            exit( 1 );
          }
        }
      if( i < num_right_handed_bakers )
        {
          // Join with thread
          if ( pthread_join( right_baker[i], NULL ) ) {
            fprintf( stderr,"Error while joining with child thread\n" );
            exit( 1 );
          }
        }
      if( i < num_cautious_bakers )
        {
          // Join with thread
          if ( pthread_join( cautious_baker[i], NULL ) ) {
            fprintf( stderr,"Error while joining with child thread\n" );
            exit( 1 );
          }
        }
    }

  exit(0);
}


void *do_left_work( void *arg ) {

  struct arguments *argument;
  struct timeval before, after;
  double total_time = 0;
  double elapsed_time;
  argument = ( struct arguments *) arg;

  pthread_mutex_t *left_mutex = argument->left_mutex;
  pthread_cond_t *left_mit_available = argument->left_mit_available;
  int *left_mittens = argument->left_mittens;
  int baker_id = argument->baker_id;


  for( int batches_baked = 0; batches_baked < NUM_ITERATIONS; batches_baked = batches_baked + 1 )
    {
      printf("[Left-handed baker %d] is working...\n", baker_id );

      pthread_mutex_lock( left_mutex );

      printf("[Left-handed baker %d] wants a left-handed mitt...\n", baker_id );

      gettimeofday( &before, NULL );
      if( *left_mittens == 0 )
        {
          pthread_cond_wait( left_mit_available, left_mutex );
        }
      gettimeofday( &after, NULL );

      elapsed_time = ( 1000000.0*(after.tv_sec - before.tv_sec) + (after.tv_usec - before.tv_usec))/1000000.0;

      total_time = total_time + elapsed_time;

      printf("[Left-handed baker %d]  WAIT_TIME: %.31f seconds...\n", baker_id, elapsed_time );

      *left_mittens = *left_mittens - 1;

      printf("[Left-handed baker %d] has got a left-handed mitt...\n", baker_id );

      pthread_mutex_unlock( left_mutex );

      printf("[Left-handed baker %d] has put cookies in the oven and is waiting...\n", baker_id );

      random_sleep( .2, .5 );

      printf("[Left-handed baker %d] has taken cookies out of the oven...\n", baker_id );

      pthread_mutex_lock( left_mutex );

      *left_mittens = *left_mittens + 1;

      if( *left_mittens == 1 )
        {
          pthread_cond_signal( left_mit_available );
        }

      printf("[Left-handed baker %d] has put back a left-hand mit...\n", baker_id );

      pthread_mutex_unlock( left_mutex );
    }

  printf("[Left-handed baker %d]  TOTAL_WAIT_TIME: %.31f seconds...\n", baker_id, total_time );
  printf("[Left-handed baker %d]  TOTAL_AVERAGE_WAIT_TIME: %.31f seconds...\n", baker_id, total_time/10.0 );

  return NULL;
}

void *do_right_work( void *arg ) {

  struct arguments *argument;
  struct timeval before, after;
  double total_time = 0;
  double elapsed_time;
  argument = ( struct arguments *) arg;

  pthread_mutex_t *right_mutex = argument->right_mutex;
  pthread_cond_t *right_mit_available = argument->right_mit_available;
  int *right_mittens = argument->right_mittens;
  int baker_id = argument->baker_id;


  for( int batches_baked = 0; batches_baked < NUM_ITERATIONS; batches_baked = batches_baked + 1 )
    {
      printf("[Right-handed baker %d] is working...\n", baker_id );

      pthread_mutex_lock( right_mutex );

      printf("[Right-handed baker %d] wants a right-handed mitt...\n", baker_id );

      gettimeofday( &before, NULL );
      while( *right_mittens == 0 )
        {
          pthread_cond_wait( right_mit_available, right_mutex );
        }
      gettimeofday( &after, NULL );

      elapsed_time = ( 1000000.0*(after.tv_sec - before.tv_sec) + (after.tv_usec - before.tv_usec))/1000000.0;

      total_time = total_time + elapsed_time;

      printf("[Right-handed baker %d] WAIT_TIME: %.31f seconds...\n", baker_id, elapsed_time );

      *right_mittens = *right_mittens - 1;

      printf("[Right-handed baker %d] has got a right-handed mitt...\n", baker_id );

      pthread_mutex_unlock( right_mutex );

      printf("[Right-handed baker %d] has put cookies in the oven and is waiting...\n", baker_id );

      random_sleep( .2, .5 );

      printf("[Right-handed baker %d] has taken cookies out of the oven...\n", baker_id );

      pthread_mutex_lock( right_mutex );

      *right_mittens = *right_mittens + 1;

      if( *right_mittens == 1 )
        {
          pthread_cond_signal( right_mit_available );
        }

      printf("[Right-handed baker %d] has put back a right-hand mit...\n", baker_id );

      pthread_mutex_unlock( right_mutex );
    }

  printf("[Right-handed baker %d] TOTAL_WAIT_TIME: %.31f seconds...\n", baker_id, total_time );
  printf("[Right-handed baker %d] TOTAL_AVERAGE_WAIT_TIME: %.31f seconds...\n", baker_id, total_time / 10.0 );

  return NULL;
}

void *do_cautious_work( void *arg ) {

  struct arguments *argument;
  struct timeval before, after;
  double total_time = 0;
  double elapsed_time;
  argument = ( struct arguments *) arg;

  pthread_mutex_t *left_mutex = argument->left_mutex;
  pthread_mutex_t *right_mutex = argument->right_mutex;
  pthread_cond_t *right_mit_available = argument->right_mit_available;
  pthread_cond_t *left_mit_available = argument->left_mit_available;
  int *left_mittens = argument->left_mittens;
  int *right_mittens = argument->right_mittens;
  int baker_id = argument->baker_id;


  for( int batches_baked = 0; batches_baked < NUM_ITERATIONS; batches_baked = batches_baked + 1 )
    {
      printf("[Cautious baker %d] is working...\n", baker_id );

      pthread_mutex_lock( left_mutex );

      printf("[Cautious baker %d] wants a left-handed mitt...\n", baker_id );

      gettimeofday( &before, NULL );
      if( *left_mittens == 0 )
        {
          pthread_cond_wait( left_mit_available, left_mutex );
        }
      gettimeofday( &after, NULL );

      elapsed_time = ( 1000000.0*(after.tv_sec - before.tv_sec) + (after.tv_usec - before.tv_usec))/1000000.0;

      total_time = total_time + elapsed_time;

      printf("[Cautious baker %d]     WAIT_TIME: %.31f seconds...\n", baker_id, elapsed_time );

      *left_mittens = *left_mittens - 1;

      printf("[Cautious baker %d] has got a left-handed mitt...\n", baker_id );

      pthread_mutex_unlock( left_mutex );

      pthread_mutex_lock( right_mutex );

      printf("[Cautious baker %d] wants a right-handed mitt...\n", baker_id );

      gettimeofday( &before, NULL );
      while( *right_mittens == 0 )
        {
          pthread_cond_wait( right_mit_available, right_mutex );
        }
      gettimeofday( &after, NULL );

      elapsed_time = ( 1000000.0*(after.tv_sec - before.tv_sec) + (after.tv_usec - before.tv_usec))/1000000.0;

      total_time = total_time + elapsed_time;

      printf("[Cautious baker %d]     WAIT_TIME: %.31f seconds...\n", baker_id, elapsed_time );

      *right_mittens = *right_mittens - 1;

      printf("[Cautious baker %d] has got a right-handed mitt...\n", baker_id );

      pthread_mutex_unlock( right_mutex );

      printf("[Cautious baker %d] has put cookies in the oven and is waiting...\n", baker_id );

      random_sleep( .2, .5 );

      printf("[Cautious baker %d] has taken cookies out of the oven...\n", baker_id );

      pthread_mutex_lock( left_mutex );

      *left_mittens = *left_mittens + 1;

      if( *left_mittens == 1 )
        {
          pthread_cond_signal( left_mit_available );
        }

      printf("[Cautious baker %d] has put back a left-hand mit ...\n", baker_id );

      pthread_mutex_unlock( left_mutex );

      pthread_mutex_lock( right_mutex );

      *right_mittens = *right_mittens + 1;

      if( *right_mittens == 1 )
        {
          pthread_cond_signal( right_mit_available );
        }

      printf("[Cautious baker %d] has put back a right-hand mit...\n", baker_id );

      pthread_mutex_unlock( right_mutex );
    }

  printf("[Cautious baker %d]     TOTAL_WAIT_TIME: %.31f seconds...\n", baker_id, total_time );
  printf("[Cautious baker %d]     TOTAL_AVERAGE_WAIT_TIME: %.31f seconds...\n", baker_id, total_time / 10.0 );

  return NULL;
}
