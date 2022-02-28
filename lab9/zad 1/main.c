#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <sys/times.h>
#include <time.h>
#include <stdio.h>
#include <time.h>

//W poleceniu nie zostalo jasno zdefiniowane co robic gdy:
//a) zebralo sie 3 elfow, a mikolaj zajmuje sie rozwozeniem prezentow
//b) zebralo sie 9 reniferow, a mikolaj rozwiazuje problemy elfow
//zakladame, ze w obu przypadkach nikt nic nie wypisuje i po prostu czeka na
//wykonianie obowiazkow mikolaja korzystajac z tego samego mutexa, ktory
//sluzy do wybudzania mikolaja

pthread_mutex_t meetingMutex = PTHREAD_MUTEX_INITIALIZER; //obsluga mikolaja
pthread_cond_t meetingCondition = PTHREAD_COND_INITIALIZER;

pthread_cond_t queuePermission = PTHREAD_COND_INITIALIZER; //czekanie na wejscie do kolejki
pthread_mutex_t queueMutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t elfSantaMeetingCon = PTHREAD_COND_INITIALIZER; //czekanie w kolejce i spotkanie sie z mikolajem - elfy
pthread_mutex_t elfSantaMeetingMut = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t reindeerSantaMeetingCon = PTHREAD_COND_INITIALIZER; //czekanie w kolejce i spotkanie sie z mikolajem - renifery
pthread_mutex_t reindeerSantaMeetingMut = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t santaBusyForReindeer = PTHREAD_COND_INITIALIZER; //renifery chca obudzic mikolaja, ale ten spi
pthread_mutex_t santaBusyForReindeerMutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t santaBusyForElf = PTHREAD_COND_INITIALIZER; // elfy chca obudzic mikolaja, ale ten spi
pthread_mutex_t santaBusyForElfMutex = PTHREAD_MUTEX_INITIALIZER;

int santaSleeping = 0;
int elfCounter = 0;
int reindeerCounter = 0;
int deliveredTimes = 0;
pthread_t helpedElfs[3];

int random_int(int a, int b) //closed interval random_int number
{
    return rand() % (b - a + 1) + a;
}

void *santa()
{
    pthread_t santaId = pthread_self();
    while (deliveredTimes < 3)
    {
        santaSleeping = 1;
        pthread_mutex_lock(&meetingMutex);

        pthread_mutex_lock(&santaBusyForReindeerMutex);
        pthread_cond_broadcast(&santaBusyForReindeer);
        pthread_mutex_unlock(&santaBusyForReindeerMutex);

        pthread_cond_wait(&meetingCondition, &meetingMutex);
        pthread_mutex_unlock(&meetingMutex);

        santaSleeping = 0;

        pthread_mutex_lock(&reindeerSantaMeetingMut);
        if (reindeerCounter == 9)
        {
            deliveredTimes++;
            printf("Dostarczam zabawki\n");
            sleep(random_int(2, 4));
            pthread_cond_broadcast(&reindeerSantaMeetingCon);
            reindeerCounter = 0;
        }
        pthread_mutex_unlock(&reindeerSantaMeetingMut);

        pthread_mutex_lock(&santaBusyForElfMutex);
        pthread_cond_broadcast(&santaBusyForElf);
        pthread_mutex_unlock(&santaBusyForElfMutex);

        pthread_mutex_lock(&queueMutex);
        if (elfCounter == 3)
        {
            printf("Mikolaj: rozwiazuje problemy elfow %ld %ld %ld, %ld\n", helpedElfs[0],
                   helpedElfs[1], helpedElfs[2], santaId);
            sleep(random_int(1, 2));
            elfCounter = 0;
            pthread_mutex_lock(&elfSantaMeetingMut);
            pthread_cond_broadcast(&elfSantaMeetingCon);
            pthread_mutex_unlock(&elfSantaMeetingMut);

            pthread_cond_broadcast(&queuePermission);
        }
        pthread_mutex_unlock(&queueMutex);

        printf("Mikolaj: zasypiam.\n");
    }
   exit(0); 
}

void *elf()
{
    pthread_t elfId = pthread_self();
    while (1)
    {
        sleep(random_int(2, 5));

        pthread_mutex_lock(&queueMutex);//dostep do elf counter, wpuszczenie do kolejki
        while (elfCounter == 3)
        {
            printf("Elf: czekam na powrot elfow, %ld\n", elfId);
            pthread_cond_wait(&queuePermission, &queueMutex);
        }

        elfCounter++;
        helpedElfs[elfCounter - 1] = elfId;

        pthread_mutex_lock(&elfSantaMeetingMut);
        if (elfCounter < 3)
        {
            printf("Elf: czeka %d elfow na Mikolaja, %ld\n", elfCounter, elfId);
            pthread_mutex_unlock(&queueMutex);//odblokowanie queuemutex
            pthread_cond_wait(&elfSantaMeetingCon, &elfSantaMeetingMut);
        }
        else
        {
            pthread_mutex_unlock(&queueMutex);//odblokowanie ququemutex
            if (santaSleeping) 
            {
                printf("Elf: wybudzam mikolaja, %ld\n", elfId);
                pthread_mutex_lock(&meetingMutex);
                pthread_cond_broadcast(&meetingCondition);
                pthread_mutex_unlock(&meetingMutex);
            }
            else
            {
                pthread_mutex_lock(&santaBusyForElfMutex);
                pthread_cond_wait(&santaBusyForElf, &santaBusyForElfMutex);
                pthread_mutex_unlock(&santaBusyForElfMutex);
            }
            printf("Elf: Mikolaj rozwiazuje problem, %ld\n", elfId);
            pthread_cond_wait(&elfSantaMeetingCon, &elfSantaMeetingMut);
        }
        
        pthread_mutex_unlock(&elfSantaMeetingMut);
    }
}

void *reindeer()
{
    pthread_t reindeerId = pthread_self();
    while (1)
    {
        sleep(random_int(5, 10));

        pthread_mutex_lock(&reindeerSantaMeetingMut);
        if (reindeerCounter < 8)
        {
            reindeerCounter++;
            printf("Renifer: czeka %d reniferow na Mikolaja, %ld\n", reindeerCounter, reindeerId);
            pthread_cond_wait(&reindeerSantaMeetingCon, &reindeerSantaMeetingMut);
            pthread_mutex_unlock(&reindeerSantaMeetingMut);
        }
        else
        {

            reindeerCounter++;
            pthread_mutex_unlock(&reindeerSantaMeetingMut);

            if (!santaSleeping)
            {
                pthread_mutex_lock(&santaBusyForReindeerMutex);
                pthread_cond_wait(&santaBusyForReindeer, &santaBusyForReindeerMutex);
                pthread_mutex_unlock(&santaBusyForReindeerMutex);
            }

            printf("Renifer: Wybudzam mikolaja %ld \n", reindeerId);
            pthread_mutex_lock(&meetingMutex);
            pthread_cond_broadcast(&meetingCondition);
            pthread_mutex_unlock(&meetingMutex);

            pthread_mutex_lock(&reindeerSantaMeetingMut);
            pthread_cond_wait(&reindeerSantaMeetingCon, &reindeerSantaMeetingMut);
            pthread_mutex_unlock(&reindeerSantaMeetingMut);
        }
        
    }
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    elfCounter = 0;
    reindeerCounter = 0;
    santaSleeping = 1;

    pthread_t elfs[10];
    pthread_t reindeers[9];
    pthread_t santaThread;
    pthread_create(&santaThread, NULL, santa, NULL);
    for (int i = 0; i < 10; i++)
    {
        pthread_create(&elfs[i], NULL, elf, NULL);
    }
    for (int i = 0; i < 9; i++)
    {
        pthread_create(&reindeers[i], NULL, reindeer, NULL);
    }

    for (int i = 0; i < 10; i++)
    {
        pthread_join(elfs[i], NULL);
    }
    for (int i = 0; i < 9; i++)
    {
        pthread_join(reindeers[i], NULL);
    }
    pthread_join(santaThread, NULL);

    return 0;
}