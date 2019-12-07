/*
    Client program to get the value of PI
    This program connects to the server using sockets


    Modified by Mauricio Peón García
    A01024162
    27/10/2019
*/
//C
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

//C++
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
// Sockets libraries
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>
#include <sys/poll.h>

#include <signal.h>
// Custom libraries
#include "sockets.h"
#include "fatal_error.h"
#include "uno_functions.h"
#include "graphics.h"


#define BUFFER_SIZE 1024

using namespace std;

int interrupted = 0;

///// FUNCTION DECLARATIONS
void usage(char * program);
void attendRequest(int connection_fd);
void initGame(char *buffer, vector<pair<int,int> > *hand, int *turn);
void saveCards(char *buffer, vector<pair<int,int> > *hand);
void dealWithTurn(char *buffer, pair<int, int> *current_card ,int * current_turn, const int my_turn, const char total_players, vector<pair<string,int> > *players_names, vector<pair<int,int> > *hand);
void savePlayersName(char *buffer, vector<pair<string,int> > *players_names, int *total_players);
void sendConfirmation(char *buffer, int connection_fd);
int makeMove(int connection_fd ,int option, vector<pair<int,int> > *hand);
void printMyCards(vector<pair<int,int> > hand);
void setupHandlers();
void onInterrupt(int signal);

///// MAIN FUNCTION
int main(int argc, char * argv[])
{
    int connection_fd;

    printf("\n=== CLIENT FOR Uno ++  ===\n");

    // Check the correct arguments
    if (argc != 3)
    {
        usage(argv[0]);
    }

    setupHandlers();
    // Start the server
    connection_fd = connectSocket(argv[1], argv[2]);
	// Listen for connections from the clients
    attendRequest(connection_fd);
    // Close the socket
    close(connection_fd);

    return 0;
}

///// FUNCTION DEFINITIONS

/*
    Explanation to the user of the parameters required to run the program
*/
void usage(char * program)
{
    printf("Usage:\n");
    printf("\t%s {server_address} {port_number}\n", program);
    exit(EXIT_FAILURE);
}

void attendRequest(int connection_fd)
{
    char buffer[BUFFER_SIZE];
    char player_name[BUFFER_SIZE];
    int server_signal = 0;
    int option;
    int my_turn;
    int current_turn;
    int total_players;
    char * text;
    pair <int, int> current_card;
    //pair <string,int> player_names;
    vector <pair<int,int> > hand;
    vector <pair<string,int> > players_names;

     // Variables for polling
    //struct pollfd poll_fd[1];
    //int poll_response;
    //int timeout = 1000;     // Timeout of one second

    // Prepare for the poll
    //poll_fd[0].fd = connection_fd;
    //poll_fd[0].events = POLLIN;

    while(option!=2)
    {
        cout<<"************** Main menu **************\n";

        cout<<"1) Start game\n";
        cout<<"2) Exit\n";
        cin>>option;

        if(option == 1)
        {
            printf("Player name: ");
            scanf("%s",player_name);


            // Prepare the response to the client
            sprintf(buffer,"%s",player_name);

            // SEND
                // Send the response to the server to join a uno ++ game
            sendString(connection_fd, buffer);

            //Recive the players turn and cards hand
            recvString(connection_fd, buffer, BUFFER_SIZE);

            initGame(buffer, &hand, &my_turn);

            sendConfirmation(buffer,connection_fd);

            //Recive players name
            recvString(connection_fd, buffer, BUFFER_SIZE);

            savePlayersName(buffer, &players_names, &total_players);

            sendConfirmation(buffer,connection_fd);

            while(1)
            {
                printf("Entro\n");
                //Recive current turn
                recvString(connection_fd, buffer, BUFFER_SIZE);
                printf("Salida\n");

                printf("%s\n",buffer);
                text = strtok(buffer, ":");
                if(strncmp(text, "TURN", 5) != 0)
                {
                    text = strtok(NULL, ":");
                    printf("The winner is: %s", text);
                    break;

                }
                //printf("2.%s\n",buffer);
                dealWithTurn(buffer, &current_card,&current_turn, my_turn, total_players, &players_names, &hand);
                cout<<"NOMBRE"<<players_names[0].first<<endl;
                renderWindow(&hand,&current_card,&players_names,(current_turn -1));

                if(my_turn == current_turn)
                {
                    while(1)
                    {
                        printMyCards(hand);
                        printf("0. Ask card\nn. Card number\n Your turn: ");
                        scanf("%d",&option);

                        if(option == 0)
                        {
                            sprintf(buffer,"p");
                            sendString(connection_fd, buffer);
                            break;

                        }else
                        {
                            if(makeMove(connection_fd ,option-1, &hand))
                            {
                                deleteCardAtPosition(&hand,option-1);
                                break;
                            }

                            printf("Invalid card\n");

                        }

                    }
                }else{
                    sendConfirmation(buffer,connection_fd);
                }


            }

        }

    }



    // While not interrupted receive signals from the server.
    /*while(!interrupted)
    {
        poll_response = poll(poll_fd, 1, timeout);
        if (poll_response == 0)     // Nothing to receive
        {
            fflush(stdout);
        }
        else if (poll_response == -1)
        {
            perror("poll");
            // Exit the inner loop
            break;
        }
        else
        {
            if (poll_fd[0].revents & POLLIN)
            {
                recvString(connection_fd, buffer, BUFFER_SIZE);
                sscanf(buffer, "%d", &server_signal);
                break;
            }
        }
    }*/

    /* If interrupted, send signal to the server.
    if (interrupted)
    {
        printf("\n > Interrupted by a signal. Sending to the server.\n");
        sprintf(buffer, "%d", interrupted);
        sendString(connection_fd, buffer);
        recvString(connection_fd, buffer, BUFFER_SIZE);
    }

    // Receive the request
    if (server_signal) {
        printf("\n > The server was interrupted, Here are the results.\n");
    }

    */
}

void initGame(char *buffer, vector<pair<int,int> > *hand, int *turn)
{
    char *string;

    *turn = atoi(strtok(buffer, ":"));

    saveCards(buffer,hand);
    //sscanf(buffer, "%d", &player, );

}

void saveCards(char *buffer, vector<pair<int,int> > *hand)
{
    char *number,*color;
    pair<int,int> card;
    int total_cards;

    total_cards = atoi(strtok(NULL, ":"));

    for(int i=0; i<total_cards; i++)
    {
        number = strtok (NULL, ":");
        color = strtok (NULL, ":");

        card = make_pair(atoi(number),atoi(color));
        //printf("1: %d 2: %d\n", card.first, card.second);
        hand->push_back(card);

    }

}

void dealWithTurn(char *buffer, pair<int, int> *current_card ,int * current_turn, const int my_turn, const char total_players, vector<pair<string,int> > *players_names, vector<pair<int,int> > *hand)
{
    int i;

    *current_turn = atoi(strtok(NULL, ":"));


    for(i=0; i<total_players; i++)
    {
        players_names->at(i).second = atoi(strtok(NULL, ":")) ;
    }

    current_card->first = atoi(strtok(NULL, ":"));
    current_card->second = atoi(strtok(NULL, ":"));

    if(*current_turn == my_turn)
    {
        saveCards(buffer,hand);
    }

}

void savePlayersName(char *buffer, vector<pair<string,int> > *players_names, int *total_players)
{
    string player_name;
    stringstream ss;

    *total_players = atoi(strtok(buffer, ":"));

    for(int i=0; i<*total_players; i++)
    {
        ss << strtok(NULL, ":");
        ss >> player_name;

        players_names->push_back(make_pair(player_name,0));

    }

}

void sendConfirmation(char *buffer, int connection_fd)
{
    sprintf(buffer,"OK");
    sendString(connection_fd, buffer);
}

int makeMove(int connection_fd ,int option, vector<pair<int,int> > *hand)
{
    char buffer[BUFFER_SIZE];
    int color = 0;

    if(option >= 0 && option < hand->size())
    {
        // Change wildcard colors
        if (hand->at(option).second == 0)
        {
            printf("Please select one of the 4 colors\n");
            printf(" 1 - Red\n");
            printf(" 2 - Blue\n");
            printf(" 3 - Green\n");
            printf(" 4 - Yellow\n");

            // Validate color
            while (color < 1 || color > 4)
            {
                scanf("%d", &color);
            }
            hand->at(option).second = color;


        }
        printf("op:%d\n",option);
        printf("Antes:\n");
        sprintf(buffer,"%d:%d",hand->at(option).first,hand->at(option).second);
        printf("En medio:\n");
        sendString(connection_fd, buffer);
        printf("Despues:\n");

        recvString(connection_fd, buffer, BUFFER_SIZE);

        if(strncmp(buffer, "No", 3) != 0)
        {
            return 1;
        }

    }

    return 0;


}

void printMyCards(vector<pair<int,int> > hand)
{
    printf("***** My cards ******\n");
    for(int i=0; i<hand.size(); i++)
    {
        printf("%d-> %d %d\n",i+1,hand[i].first,hand[i].second);

    }

}
// Define signal handlers
void setupHandlers()
{
    struct sigaction new_action;

    new_action.sa_handler = onInterrupt;
    new_action.sa_flags = 0;
    sigfillset(&new_action.sa_mask);

    sigaction(SIGINT, &new_action, NULL);
}


// Handler for SIGINT
void onInterrupt(int signal)
{
    interrupted = 1;
}
