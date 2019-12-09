/*
    Client program to play UNO game
    This program connects to the server using sockets


    Created  by
    Mauricio Peón García        A01024162
    Pablo Terán Ríos            A01421434
    Romeo Varela Nagore         A01020736

    07/12/2019
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
#include <SFML/Graphics.hpp>
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

//Attend client request
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
    vector <pair<int,int> > hand;
    vector <pair<string,int> > players_names;
    while(!interrupted)
    {
        cout<<"************** Main menu **************\n";
        cout<<"1) Start game\n";
        cout<<"2) Exit\n";
        cin>>option;
        if(option == 2)
        {
            break;
        }
        if(option == 1)
        {
            printf("Player name: ");
            scanf("%s",player_name);
            // Prepare the response to the client
            sprintf(buffer,"%s",player_name);
            // SEND
            // Send the response to the server to join a uno ++ game
            sendString(connection_fd, buffer);
            // Recive the players turn and cards hand
            recvString(connection_fd, buffer, BUFFER_SIZE);
            // Save players hand and turn
            initGame(buffer, &hand, &my_turn);
            // Send a confirmation to the server
            sendConfirmation(buffer,connection_fd);
            // Recive players name
            recvString(connection_fd, buffer, BUFFER_SIZE);
            // Save total number of players and players name
            savePlayersName(buffer, &players_names, &total_players);
            // Send a confirmation message to the server
            sendConfirmation(buffer,connection_fd);
            // Render window where the UNO game graphics are going to be displayed
            sf::RenderWindow window(sf::VideoMode(1000, 600), "UNO++");
            window.setPosition(sf::Vector2i(0,20));
            while (window.isOpen())
            {
                sf::Event event;
                while (window.pollEvent(event))
                {
                    // Start game
                    while(!interrupted)
                    {
                        // Receive current turn
                        recvString(connection_fd, buffer, BUFFER_SIZE);
                        text = strtok(buffer, ":");
                        if(strncmp(text, "TURN", 5) != 0)//If there is a winner, the game finish and the winner is going to be displayed
                        {
                            text = strtok(NULL, ":");
                            printf("The winner is: %s\n", text);
                            window.close();
                            interrupted = 1;
                            break;
                        }
                        // Save all data related with the current turn
                        dealWithTurn(buffer, &current_card,&current_turn, my_turn, total_players, &players_names, &hand);
                        // Display graphics
                        renderWindow(&hand,&current_card,&players_names,(current_turn -1),&window);
                        // If it`s your turn send a card or ask for one
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
                                    //Ask a card to the server
                                    sendString(connection_fd, buffer);
                                    break;
                                }else
                                {
                                    //Send a card to the server
                                    if(makeMove(connection_fd ,option-1, &hand))
                                    {
                                        //If the card is good, remove card from the hand
                                        deleteCardAtPosition(&hand,option-1);
                                        break;
                                    }
                                    printf("Invalid card\n");
                                }
                            }
                        }else{
                            // If it's not your turn send a confirmation message to the server
                            sendConfirmation(buffer,connection_fd);
                        }
                    }
                }
            }
        }
    }
}
// Save initial cards and player's turn
void initGame(char *buffer, vector<pair<int,int> > *hand, int *turn)
{
    char *string;
    *turn = atoi(strtok(buffer, ":"));
    saveCards(buffer,hand);
}
// Save player cards
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
        hand->push_back(card);
    }
}
// Save all data related with the current turn
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
// Save the name of all the players
void savePlayersName(char *buffer, vector<pair<string,int> > *players_names, int *total_players)
{
    char *player_name;
    *total_players = atoi(strtok(buffer, ":"));
    for(int i=0; i<*total_players; i++)
    {
        player_name = strtok(NULL, ":");
        players_names->push_back(make_pair(player_name,0));
    }
}
//Confirmation message that will be send to the server
void sendConfirmation(char *buffer, int connection_fd)
{
    sprintf(buffer,"OK");
    sendString(connection_fd, buffer);
}
//Send card, selected by the player ,to the server
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
        sprintf(buffer,"%d:%d",hand->at(option).first,hand->at(option).second);
        //Send card to the server
        sendString(connection_fd, buffer);
        //Receive server response if the card was valid
        recvString(connection_fd, buffer, BUFFER_SIZE);
        if(strncmp(buffer, "No", 3) != 0)
        {
            return 1;
        }
    }
    return 0;
}
// Print players card in console
void printMyCards(vector<pair<int,int> > hand)
{
    printf("***** My cards ******\n");
    for(int i=0; i<hand.size(); i++)
    {
        cout<<i + 1<<"-> "<< hand[i].first<<" "<<getColor(&hand[i])<<endl;
        //printf("%d-> %d %s\n",i+1,hand[i].first,getColor(&hand[i]));
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
