#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>
// Threads 0 - 3 are the players thread 4 is the dealer
#define NUMOFPLAYERS    5
#define DICEVALUE       6

int myArray[NUMOFPLAYERS - 1][3];
int randPlayer;
int seedValue = 0; 
int doneFillingDice = 0;
int startFound = 0;
int roundCount = 1; 
pthread_t threads[NUMOFPLAYERS];
pthread_attr_t imDone;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t diceMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t informDealer;
pthread_cond_t informRandPlayer;
bool playersAC = false;
bool playersBD = false;
bool doneFilling = false;
void *status;
int myThread;
int myDealerThread;
void getSeed(char *argv[]);
int randNumGen(int SIZE);
int getDiceValues();
void *fillingUpDice(void *threadid);
void *dealerThread(void *threadid);
void generatingDice();
void printResults();
void printResult(long tid);
void populateTheThreads(long tid);
FILE *fp; // For writting to a .log file

int main(int argc, char *argv[])
{
  fp = fopen("outputFile.log", "w"); // opening the file
  getSeed(argv); // getting the seed from the user
  srand(seedValue); // generating the seed from the given seed value
  generatingDice(); // will run the thread 
  printResults(); // printing the final results
  fclose(fp); // closing the .log file
  pthread_exit(NULL); // exiting the trheads and deliting them 
}

void getSeed(char *argv[]) // We will get the seed value from either the script or makefile 
{
  seedValue = atoi(argv[1]);
}

int randNumGen(int size) // will give a random number given a size
{
  int randValue;
  randValue = rand() % size;
  return randValue;
}

int getDiceValues() // This will generate the random dice value 1 - 6
{
  int myDiceValue;
  myDiceValue = randNumGen(DICEVALUE) + 1;
  return myDiceValue; // returning the dice value
}

// Prints the individual statements as they come 
void printResult(long tid)
{
  switch (tid)
  {
  case 0:
    fprintf(fp, "PLAYER A : ");
    fprintf(fp, "gets %d and %d with a sum : %d\n\n", myArray[tid][0], myArray[tid][1], myArray[tid][2]);
    break;
  case 1:
    fprintf(fp, "PLAYER B : ");
    fprintf(fp, "gets %d and %d with a sum : %d\n\n", myArray[tid][0], myArray[tid][1], myArray[tid][2]);
    break;
  case 2:
    fprintf(fp, "PLAYER C : ");
    fprintf(fp, "gets %d and %d with a sum : %d\n\n", myArray[tid][0], myArray[tid][1], myArray[tid][2]);
    break;
  case 3:
    fprintf(fp, "PLAYER D : ");
    fprintf(fp, "gets %d and %d with a sum : %d\n\n", myArray[tid][0], myArray[tid][1], myArray[tid][2]);
    break;
  }
}

// prints the final results
void printResults()
{
  for (int i = 0; i < NUMOFPLAYERS - 1; i++) // print the values of each player
  {
    switch (i)
    {
    case 0:
      printf("\nPLAYER A : ");
      printf("%d %d\n\n", myArray[i][0], myArray[i][1]);
      break;
    case 1:
      printf("PLAYER B : ");
      printf("%d %d\n\n", myArray[i][0], myArray[i][1]);
      break;
    case 2:
      printf("PLAYER C : ");
      printf("%d %d\n\n", myArray[i][0], myArray[i][1]);
      break;
    case 3:
      printf("PLAYER D : ");
      printf("%d %d\n\n", myArray[i][0], myArray[i][1]);
      break;
    }
  }
}

// This will fill the threads with values
void populateTheThreads(long tid)
{
  // states which thread is being worked on
  for (int i = 0; i < 2; i++)
  {
    // A 2D array that will contain the 4 players and 2 dice val and a sum
    myArray[tid][i] = getDiceValues();
    startFound = 1; // start to populating the rest of threads since initial found
  }
  // filling the 3rd value with the sum of the two previous given the thread
  myArray[tid][2] = myArray[tid][0] + myArray[tid][1];
  printResult(tid);
}

void *dealerThread(void *threadid)
{
  long tid; 
  tid = 5;
  randPlayer = randNumGen(3); // Generating the random Player 
  pthread_cond_signal(&informRandPlayer); // signalling the players the random player
  pthread_mutex_lock(&myMutex);
  pthread_cond_wait(&informDealer, &myMutex); // waiting on a signal from the players
  if (playersAC)
  {
    fprintf(fp, "DEALER : The Winning Team is A & C");
    pthread_exit(NULL); // Will exit when matched
  }
  if (playersBD)
  {
    fprintf(fp, "DEALER : The Winning Team is B & D");
    pthread_exit(NULL); // Will exit when matched
  }
  pthread_mutex_unlock(&myMutex);
  pthread_exit(NULL);
}

void *fillingUpDice(void *threadid)
{
  pthread_cond_wait(&informRandPlayer, &mutex);
  int value = randPlayer; // decalring the random player
  long tid;
  tid = (long)threadid;
  // Once we've found the the randPlayer or we've already given a value to the initial player
  if ((long)value == tid || startFound != 0)
  {
    // once we've given a value to rand player we will give the rest values
    if (startFound == 1) 
    {
      // We dont want to keep give different values until finished 
      if (tid == value || doneFillingDice == 1)
      {
        doneFillingDice = 1;
        pthread_exit(NULL);
      }
      // This will fill the remaining players with dice values
      else
      {
        populateTheThreads(tid);
      }
      doneFilling = true;
    }
    // The random player has been found and setting the value to start the rest of threads
    else
    {
      populateTheThreads(tid);
    }
  }
  pthread_exit(NULL);
}

void generatingDice()
{
  long index;
  pthread_mutex_init(&mutex, NULL);
  pthread_mutex_init(&myMutex, NULL);
  pthread_mutex_init(&diceMutex, NULL);
  pthread_cond_init(&informDealer, NULL);
  pthread_attr_init(&imDone);
  pthread_attr_setdetachstate(&imDone, PTHREAD_CREATE_JOINABLE);

  myDealerThread = pthread_create(&threads[4], NULL, dealerThread, (void *)4); // creating dealer thread
  while (!playersAC && !playersBD)
  {
    do // Creating the threads and will continue until they are filled
    {
      pthread_mutex_lock(&diceMutex); // Lock the threads until they finish 
      for (index = 0; index < NUMOFPLAYERS - 1; index++) // Creating the threads
      {
        // Executing the fillingUpDice Function
        myThread = pthread_create(&threads[index], &imDone, fillingUpDice, (void *)index);
        if (myThread) // returns an error if anything goes bad
        {
          printf("ERROR, return code form pthread_create() is %d\n", myThread);
          exit(-1);
        }
      }
      pthread_mutex_unlock(&diceMutex);
    } while (doneFillingDice != 1);
    doneFillingDice = 0; // reseting the values
    startFound = 0;

    // pthread_attr_destroy(&imDone); // would give me errors
    // for (index = 0; index < NUMOFPLAYERS - 1; index++)
    // {
    //   myThread = pthread_join(threads[index], &status);
    //   if (myThread)
    //   {
    //     printf("Error, return code from pthread_join() is %d\n", myThread);
    //     exit(-1);
    //   }
    //   printf("Main : Completed joining thread %ld, having a status of %ld\n", index, (long)status);
    // }

    // will compare the corresponding values and check if they are eqyal and signal the dealers
    pthread_mutex_lock(&myMutex);
    if (myArray[0][2] == myArray[2][2] && doneFillingDice != 1) // If Players A & C Match 
    {
      playersAC = true;
      // printf("AC Has informed Dealer\n");
      pthread_cond_signal(&informDealer);
    }
    else if (myArray[1][2] == myArray[3][2] && doneFillingDice != 1) // If Players B & D Match 
    {
      playersBD = true;
      // printf("BD Has informed Dealer\n");
      pthread_cond_signal(&informDealer);
    }
    pthread_mutex_unlock(&myMutex);
  }
}