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


#define BUFFER_SIZE 1024

using namespace std;

int interrupted = 0;

///// FUNCTION DECLARATIONS
void usage(char * program);
void attendRequest(int connection_fd);
void initGame(char *buffer, vector<pair<int,int> > *hand, char *turn);
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
    int server_signal = 0;
    char *player;
    int option;
    pair <int, int> current_card;
    char player_name[BUFFER_SIZE];
    vector <pair<int,int> > hand;

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
            
            initGame(buffer, &hand, player);

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

    sscanf(buffer, "%d %lu %lf", &server_signal, &counter, &result);
        
    // Print the result
    printf(" > Number of iterations: %lu\n", counter);    
    printf(" > The value for PI is: %.20lf\n", result);
    */
}

void initGame(char *buffer, vector<pair<int,int> > *hand, char *turn)
{
    char *string,*number,*color;
    pair<int,int> card;
    

    turn = strtok(buffer, ":");

    for(int i=0; i<7; i++)
    {
        number = strtok (NULL, ":");
        color = strtok (NULL, ":");

        card = make_pair(atoi(number),atoi(color));

        hand->push_back(card);
    

    }
    //sscanf(buffer, "%d", &player, );

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