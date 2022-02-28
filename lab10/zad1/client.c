#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include "includes.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/un.h>
#include <unistd.h>

#define MAX_MESSAGE_LENGTH 256

enum Field
{
    BLANK,
    O,
    X
};

enum GameStage
{
    WAITING_FOR_MOVE,
    RIVAL_MOVE,
    MOVE,
};

struct Board
{
    int move; //1-O 0-X
    enum Field fields[9];
};

struct Board board = {1, {BLANK}};

char name[20];
char buffer[MAX_MESSAGE_LENGTH + 1];
char command[200];
char argument[200];
int serverSocket;
int sign; //1 - O, 0 - X zaczyna O
enum GameStage stage;
int pos;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void interpret()
{
    char *ptr = strtok(buffer, " ");
    if (ptr == NULL)
        return;
    strcpy(command, ptr);
    if (ptr == NULL)
        return;
    ptr = strtok(NULL, " ");
    if (ptr == NULL)
        return;
    strcpy(argument, ptr);
}

void quit()
{
    printf("Exiting: %s.\n", argument);
    char buffer[MAX_MESSAGE_LENGTH + 1];
    sprintf(buffer, "%s quit", name);
    send(serverSocket, buffer, MAX_MESSAGE_LENGTH, 0);
    exit(0);
}

int move(struct Board *board, int position)
{
    if (position < 0 || position > 9 || board->fields[position] != BLANK)
        return 0;
    board->fields[position] = board->move ? O : X;
    board->move = !board->move;
    return 1;
}

void draw()
{
    char chr;
    int k;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            k = 3 * i + j;
            if (board.fields[k] == BLANK)
            {
                chr = k + '0' + 1;
            }
            else if (board.fields[k] == O)
            {
                chr = 'O';
            }
            else
            {
                chr = 'X';
            }
            printf("  %c  ", chr);
        }
        printf("\n********************\n\n");
    }
}

enum Field winns(struct Board *board)
{
    for (int i = 0; i < 3; i++) //column cheking
    {
        if (board->fields[i] == board->fields[i + 3] &&
            board->fields[i] == board->fields[i + 6])
            return board->fields[i];
    }
    for (int i = 0; i < 3; i++) //row cheking
    {
        if (board->fields[3 * i] == board->fields[3 * i + 1] &&
            board->fields[3 * i] == board->fields[3 * i + 2])
            return board->fields[3 * i];
    }

    // diagonal checking

    if (board->fields[0] == board->fields[4] &&
        board->fields[0] == board->fields[8])
        return board->fields[0];

    if (board->fields[2] == board->fields[4] &&
        board->fields[2] == board->fields[6])
        return board->fields[2];

    return BLANK;
}

void check_game()
{
    int win = 0;
    enum Field winner = winns(&board);
    if (winner != BLANK)
    {
        if (!sign)
            draw();
        if ((sign && winner == O) || (!sign && winner == X))
        {
            printf("WIN!\n");
        }
        else
        {
            printf("LOST!\n");
        }

        win = 1;
    }

    int draww = 1;

    for (int i = 0; i < 9; i++)
    {
        if (board.fields[i] == BLANK)
        {
            draww = 0;
            break;
        }
    }

    if (draww && !win)
        printf("DRAW\n");

    if (win || draww)
        quit();
}

void playGame()
{
    printf("Let's have some game ! \n");
    while (1)
    {
        pthread_mutex_lock(&mutex);
        if (stage == WAITING_FOR_MOVE)
        {
            printf("Waiting for rival's move...\n");

            while (stage != RIVAL_MOVE)
            {
                pthread_cond_wait(&cond, &mutex);
            }
        }
        else if (stage == RIVAL_MOVE)
        {
            printf("Rival's move:\n");
            move(&board, pos);
            check_game();
            stage = MOVE;
        }
        else if (stage == MOVE)
        {
            draw();
            int pos;
            pthread_mutex_unlock(&mutex);
            do
            {
                printf("Next move (%c): ", sign ? 'O' : 'X');
                scanf("%d", &pos);
                pos--;
            } while (!move(&board, pos));
            pthread_mutex_lock(&mutex);

            draw();

            char message[MAX_MESSAGE_LENGTH + 1];

            sprintf(message, "%s move %d", name, pos);
            send(serverSocket, message, MAX_MESSAGE_LENGTH, 0);

            check_game();
            stage = WAITING_FOR_MOVE;
        }

        pthread_mutex_unlock(&mutex);
    }
}

void initializeLocalConnection(char *path)
{
    serverSocket = socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un serverAdress;
    memset(&serverAdress, 0, sizeof(struct sockaddr_un));
    serverAdress.sun_family = AF_UNIX;
    strcpy(serverAdress.sun_path, path);

    if (connect(serverSocket, (struct sockaddr *)&serverAdress,
                sizeof(struct sockaddr_un)))
    {
        fprintf(stderr, "Conenction error! \n");
        exit(-1);
    }
}

void initializeWebConnection(char *port)
{
    struct sockaddr_in address;
    address.sin_family=AF_INET;
    address.sin_port=htons(atoi(port));
    address.sin_addr.s_addr=INADDR_ANY;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    connect(serverSocket, (struct sockaddr *)&address, sizeof(struct sockaddr_in));
}

void listenServer()
{
    while (1)
    {
        recv(serverSocket, buffer, MAX_MESSAGE_LENGTH, 0);
        interpret();

        if (strcmp(command, "add") == 0)
        {
            if (strcmp(argument, "name_taken") == 0)
            {
                printf("Name %s is already taken !\n", argument);
                exit(-1);
            }
            else if (strcmp(argument, "no_enemy") == 0)
            {
                printf("Connection succeed, waiting for enemy\n");
                continue;
            }
            sign = argument[0] == 'O';

            if (sign)
                stage = MOVE;
            else
                stage = WAITING_FOR_MOVE;
            pthread_t t;
            pthread_create(&t, NULL, playGame, NULL);
        }
        else if (strcmp(command, "move") == 0)
        {
            pthread_mutex_lock(&mutex);
            stage = RIVAL_MOVE;
            pos = atoi(argument);
            pthread_mutex_unlock(&mutex);
        }
        else if (strcmp(command, "quit") == 0)
        {
            pthread_mutex_lock(&mutex);
            check_game();
            pthread_mutex_unlock(&mutex);
            quit();
        }
        else if (strcmp(command, "ping") == 0)
        {
            sprintf(buffer, "%s ping", name);
            send(serverSocket, buffer, MAX_MESSAGE_LENGTH, 0);
        }
        pthread_mutex_lock(&mutex);
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mutex);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Invalid arguments number. [name] [way] [destination/path]");
    }

    strcpy(name, argv[1]);
    char mode[20];
    char destination[20];
    strcpy(mode, argv[2]);
    strcpy(destination, argv[3]);

    if (!strcmp(mode,"local"))   initializeLocalConnection(destination);
    else if (!strcmp(mode,"web")) initializeWebConnection(destination);
    else {
        fprintf(stderr, "Invali way of communication\n");
        exit(-1);}

    signal(SIGINT, quit);

    sprintf(buffer, "%s add", name);
    send(serverSocket, buffer, MAX_MESSAGE_LENGTH, 0);

    listenServer();

    return 0;
}
