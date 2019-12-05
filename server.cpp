/*
    Server program to compute the value of PI
    This program calls the library function 'get_pi'
    It will create child processes to attend requests
    It receives connections using sockets


    Created  by
    Mauricio Peón García        A01024162
    Pablo Terán Ríos            A01421434
    Romeo Varela Nagore         A01020736

    09/11/2019
*/

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
// Sockets libraries
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>

#include <sys/poll.h>

#include <signal.h>

#include <map>
#include <iostream>
#include <vector>
#include <sstream>

// Custom libraries
#include "sockets.h"
#include "fatal_error.h"
#include "uno_functions.h"

#define BUFFER_SIZE 1024
#define MAX_QUEUE 5


///// FUNCTION DECLARATIONS
void usage(char * program);
void waitForConnections(int server_fd);
void game(map<int, pair<string, int> > &players, int client_fd, int client_fd2, int client_fd3, int client_fd4);
void setupHandlers();
void onInterrupt(int signal);
void setPlayer(int client, map<int, pair<string, int> > &players, int i);
void sendFirstHands(int client_fd, int client_fd2, int client_fd3, int client_fd4);

int interrupted = 0;
///// MAIN FUNCTION
int main(int argc, char * argv[])
{
    int server_fd;

    printf("\n=== UNO++ Server ===\n");

    // Check the correct arguments
    if (argc != 2)
    {
        usage(argv[0]);
    }

	// Show the IPs assigned to this computer
	printLocalIPs();

    //signal(SIGINT,onInterrupt)
    setupHandlers();

    // Start the server
    server_fd = initServer(argv[1], MAX_QUEUE);
	// Listen for connections from the clients
    waitForConnections(server_fd);
    // Close the socket
    close(server_fd);

    return 0;
}

///// FUNCTION DEFINITIONS

/*
    Explanation to the user of the parameters required to run the program
*/
void usage(char * program)
{
    printf("Usage:\n");
    printf("\t%s {port_number}\n", program);
    exit(EXIT_FAILURE);
}

/*
    Main loop to wait for incomming connections
*/
void waitForConnections(int server_fd)
{

    struct sockaddr_in client_address;
    socklen_t client_address_size;
    char client_presentation[INET_ADDRSTRLEN];
    int client_fd;
    int client_fd2;
    int client_fd3;
    int client_fd4;

    //Variables for clients
    map<int, pair<string, int> > players;
    vector<int> turns;
    bool direction;
    pair<int, int> current;
    int current_players = 0;


    // Variables for polling
    struct pollfd poll_fd[1];
    int poll_response;
    int timeout = 1000;     // Timeout of one second

    // Prepare for the poll
    poll_fd[0].fd = server_fd;
    poll_fd[0].events = POLLIN;
    // Get the size of the structure to store client information
    client_address_size = sizeof client_address;

    while (!interrupted)
    {

            //Always start on 4 players
            if (current_players == 4){
              break;
            }

            poll_response = poll(poll_fd, 1, timeout);
            if (poll_response == 0)     // Nothing to receive
            {
                //printf(".");
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
                    // ACCEPT
                    cout << "The number of players = " << current_players + 1 << endl;
                    // Get the file descriptor to talk with the client
                    if (current_players == 0){
                        client_fd = accept(server_fd, (struct sockaddr *) &client_address, &client_address_size);
                        current_players++;
                        setPlayer(client_fd, players, 1);

                        //break;
                    }
                    else if (current_players == 1){
                        client_fd2 = accept(server_fd, (struct sockaddr *) &client_address, &client_address_size);
                        current_players++;
                        setPlayer(client_fd2, players, 2);

                        //break;
                    }
                    else if (current_players == 2){
                        client_fd3 = accept(server_fd, (struct sockaddr *) &client_address, &client_address_size);
                        current_players++;
                        setPlayer(client_fd3, players, 3);

                        //break;
                    }
                    else{
                        client_fd4 = accept(server_fd, (struct sockaddr *) &client_address, &client_address_size);
                        current_players++;
                        setPlayer(client_fd4, players, 4);

                        //break;
                    }
                }
            }
        }

        // Get the data from the client
        inet_ntop(client_address.sin_family, &client_address.sin_addr, client_presentation, sizeof client_presentation);

        // If not interrupted
        if (!interrupted)
        {

            // Close the main server socket to avoid interfering with the parent
            close(server_fd);

            setupHandlers();

            // Deal with the client
            // Show all 4 players
            cout << " > Player 1 = " << players[1].first << endl;
            cout << " > Player 2 = " << players[2].first << endl;
            cout << " > Player 3 = " << players[3].first << endl;
            cout << " > Player 4 = " << players[4].first << endl;

            cout << "\n\n GAME STARTING \n\n" << endl;

            game(players, client_fd, client_fd2, client_fd3, client_fd4);

            //attendRequest(client_fd);
            // Close the new socket
            close(client_fd);
            close(client_fd2);
            close(client_fd3);
            close(client_fd4);


    }
}


/*
    Game handler
*/
void game(map<int, pair<string, int> > &players, int client_fd, int client_fd2, int client_fd3, int client_fd4)
{
    char buffer[BUFFER_SIZE];

    // Send the players their turn and the starting hand
    sendFirstHands(client_fd, client_fd2, client_fd3, client_fd4);
    // Recieve Confirmation of 4 players
    recvString(client_fd, buffer, BUFFER_SIZE);
    recvString(client_fd2, buffer, BUFFER_SIZE);
    recvString(client_fd3, buffer, BUFFER_SIZE);
    recvString(client_fd4, buffer, BUFFER_SIZE);



}


// Define signal handlers
void setupHandlers()
{
    struct sigaction new_action;
    new_action.sa_handler = onInterrupt;
    new_action.sa_flags = 0;
    // Block all signals except ctrl+c.
    sigfillset(&new_action.sa_mask);
    sigaction(SIGINT, &new_action, NULL);
}

// Handler for SIGINT
void onInterrupt(int signal)
{
    interrupted = 1;
}

void setPlayer(int client, map<int, pair<string, int> > &players, int i)
{
    char buffer[BUFFER_SIZE];
    stringstream ss;

    // Recieve name from player
    recvString(client, buffer, BUFFER_SIZE);
    //Change from char[] to string
    ss << buffer;
    ss >> players[i].first;
    //Set cards to 7
    players[i].second = 7;
    cout << " > " << players[i].first << " connected" <<  endl;
}

void sendFirstHands(int client_fd, int client_fd2, int client_fd3, int client_fd4)
{
    char buffer[BUFFER_SIZE];
    char cards[BUFFER_SIZE];
    vector< pair <int, int>> hand;

    hand = player_hand();
    vectorToString(&hand, cards);
    sprintf(buffer, "1:7:%s", cards);
    printf("%s\n", buffer);
    // Send the response
    sendString(client_fd, buffer);


    hand = player_hand();
    vectorToString(&hand, cards);
    sprintf(buffer, "2:7:%s", cards);
    printf("%s\n", buffer);
    // Send the response
    sendString(client_fd2, cards);


    hand = player_hand();
    vectorToString(&hand, cards);
    sprintf(buffer, "3:7:%s", cards);
    printf("%s\n", buffer);
    // Send the response
    sendString(client_fd3, buffer);


    hand = player_hand();
    vectorToString(&hand, cards);
    sprintf(buffer, "4:7:%s", cards);
    printf("%s\n", buffer);
    // Send the response
    sendString(client_fd4, buffer);

}
