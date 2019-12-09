/*
    Server program to play UNO game
    It receives connections using sockets


    Created  by
    Mauricio Peón García        A01024162
    Pablo Terán Ríos            A01421434
    Romeo Varela Nagore         A01020736

    07/12/2019

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
void game(map<int, pair<string, int> > &players, vector <int> &clients);
void setupHandlers();
void onInterrupt(int signal);
void setPlayer(int client, map<int, pair<string, int> > &players, int i);
void sendFirstHands(vector <int> &clients);
void confirmations(vector <int> &clients);
pair<int, int> gameConfirmations(map<int, pair<string, int> > &players, int who, vector <int> &clients, pair<int, int> &current);
void sendNames( map<int, pair<string, int> > &players, vector <int> &clients);
void turnHandle ( map<int, pair<string, int> > &players, vector<int> &clients, int who, pair<int, int> &current);
void sendTurn(string s, vector <int> &clients);
string createString(map<int, pair<string, int> > &players, vector<int> &clients, int who, pair<int, int> &current);
string createRequest(map<int, pair<string, int> > &players, vector<int> &clients, int who, pair<int, int> &current);
void finishGame(vector<int> &clients, string winer);

int interrupted = 0;
///// MAIN FUNCTION
int main(int argc, char * argv[])
{
    int server_fd;
    srand(time(NULL));
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
    vector <int> clients;
    int client_fd;
    int client_fd2;
    int client_fd3;
    int client_fd4;
    //Variables for clients
    map<int, pair<string, int> > players;
    int current_players = 0;
    // Variables for polling
    struct pollfd poll_fd[1];
    int poll_response;
    int timeout = 1000;     // Timeout of one second
    int starter = 10;
    // Prepare for the poll
    poll_fd[0].fd = server_fd;
    poll_fd[0].events = POLLIN;
    // Get the size of the structure to store client information
    client_address_size = sizeof client_address;
    while (!interrupted)
    {
        //Always start on 4 players
        if (current_players == 4 || starter == -1)
        {
            break;
        }
        poll_response = poll(poll_fd, 1, timeout);
        if (poll_response == 0)     // Nothing to receive
        {
            if (current_players >= 2){
                printf("\rGame starting in %d          ", starter);
                fflush(stdout);
                starter--;
            }
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
                starter = 10;
                cout << "The number of players = " << current_players + 1 << endl;
                // Get the file descriptor to talk with the client
                if (current_players == 0){
                    client_fd = accept(server_fd, (struct sockaddr *) &client_address, &client_address_size);
                    current_players++;
                    setPlayer(client_fd, players, 1);
                    clients.push_back(client_fd);
                }
                else if (current_players == 1){
                    client_fd2 = accept(server_fd, (struct sockaddr *) &client_address, &client_address_size);
                    current_players++;
                    setPlayer(client_fd2, players, 2);
                    clients.push_back(client_fd2);
                }
                else if (current_players == 2){
                    client_fd3 = accept(server_fd, (struct sockaddr *) &client_address, &client_address_size);
                    current_players++;
                    setPlayer(client_fd3, players, 3);
                    clients.push_back(client_fd3);
                }
                else{
                    client_fd4 = accept(server_fd, (struct sockaddr *) &client_address, &client_address_size);
                    current_players++;
                    setPlayer(client_fd4, players, 4);
                    clients.push_back(client_fd4);
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
        cout << endl;
        for (int i = 1; i <= clients.size(); i++){
            cout << " > Player " << i << " = " << players[i].first << endl;
        }
        cout << "\n\n GAME STARTING \n\n" << endl;
        game(players, clients);
        // Close the socket
        close(client_fd);
        close(client_fd2);
        close(client_fd3);
        close(client_fd4);
}
}
/*
    Game handler
*/
void game(map<int, pair<string, int> > &players, vector <int> &clients)
{
    // Game variables
    int who_plays = 1;
    bool direction = true;
    pair <int, int> current = get_card();
    // Send the players their turn and the starting hand
    sendFirstHands(clients);
    // Recieve Confirmation of 4 players
    confirmations(clients);
    // Send all names
    sendNames(players, clients);
    // Recieve Confirmation of 4 players
    confirmations(clients);
    // Game starts
    while (!interrupted)
    {
        // If a player wins, send his user
        for (int i = 1; i <= clients.size(); i++){
            if (players[i].second == 0){
                cout << " > Player " << i << " wins." << endl;
                finishGame(clients,players[i].first);
                break;
            }
        }
        turnHandle(players, clients, who_plays, current);
        // If a reverse is played
        if (current.first == 11){
            direction = !direction;
        }
        if (direction){
            who_plays++;
            if (current.first == 10){
                who_plays++;
            }
            if (who_plays > clients.size())
            {
                who_plays = who_plays%clients.size();
            }
        }
        else{
            who_plays--;
            if (current.first == 10){
                who_plays--;
            }
            if (who_plays == 0)
            {
                who_plays = clients.size();
            }
            else if (who_plays == -1)
            {
                who_plays = clients.size() - 1;
            }
        }
    }

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
// Set players and names
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
// Send first hand to each player
void sendFirstHands(vector<int> &clients)
{
    char buffer[BUFFER_SIZE];
    char cards[BUFFER_SIZE];
    vector< pair <int, int> > hand;
    for (int i = 1; i <= clients.size(); i++){
        hand = player_hand();
        vectorToString(&hand, cards);
        sprintf(buffer, "%d:7:%s", i, cards);
        printf("%s\n", buffer);
        // Send the response
        sendString(clients[i-1], buffer);
    }
}
// Recieve confirmations
void confirmations(vector <int> &clients)
{
    char buffer[BUFFER_SIZE];
    for (int i = 0; i < clients.size(); i++){
        recvString(clients[i], buffer, BUFFER_SIZE);
    }
}
// Handle users
pair<int, int> gameConfirmations(map<int, pair<string, int> > &players, int who, vector <int> &clients, pair<int, int> &current)
{
    char buffer[BUFFER_SIZE];
    int number;
    int color;
    pair<int, int> new_card;
    string request;
    for (int i = 0; i < clients.size(); i++)
    {
        if (who == (i + 1))
        {
            recvString(clients[i], buffer, BUFFER_SIZE);
            // if player needs a new card
            if ((strcmp(buffer,"p") == 0))
            {
                request = createRequest(players, clients, who, current);
                sendTurn(request, clients);
                new_card = gameConfirmations(players, who, clients, current);
                break;
            }
            while (!interrupted)
            {
                // Reads the card
                number = (atoi(strtok(buffer,":")));
                color = (atoi(strtok(NULL,":")));
                new_card = make_pair(number, color);
                if (verifyCard(&new_card, &current)){
                    sprintf(buffer, "Ok");
                    sendString(clients[i], buffer);
                    players[i+1].second--;
                    break;
                }
                sprintf(buffer, "No");
                sendString(clients[i], buffer);
                recvString(clients[i], buffer, BUFFER_SIZE);
                // if player needs a new card after selecting a bad one
                if ((strcmp(buffer,"p") == 0))
                {
                    request = createRequest(players, clients, who, current);
                    sendTurn(request, clients);
                    i = -1;
                    break;
                }
            }
        }
        else
        {
            recvString(clients[i], buffer, BUFFER_SIZE);
        }
    }
    return new_card;
}
// Send all names to each player
void sendNames( map<int, pair<string, int> > &players, vector <int> &clients)
{
    char buffer[BUFFER_SIZE];
    string client_num = "" + to_string(clients.size());
    string names = client_num + ":" + players[1].first;
    // Make name string
    for (int i = 2; i <= clients.size(); i++){
        names += ":" + players[i].first;
    }
    // Cast to char []
    strcpy(buffer, names.c_str());
    // Send names
    for (int i = 0; i < clients.size(); i++){
        sendString(clients[i], buffer);
    }
}
// Turn handle
void turnHandle ( map<int, pair<string, int> > &players, vector<int> &clients, int who, pair<int, int> &current)
{
    // Create a string
    string temp = createString(players, clients, who, current);
    sendTurn(temp, clients);
    current = gameConfirmations(players, who, clients, current);
}
// Send turn
void sendTurn(string s, vector <int> &clients)
{
    char buffer[BUFFER_SIZE];
    // Cast to char []
    strcpy(buffer, s.c_str());
    for (int i = 0; i < clients.size(); i++){
        sendString(clients[i], buffer);
    }
}
// Create a string that will be sent
string createString(map<int, pair<string, int> > &players, vector<int> &clients, int who, pair<int, int> &current)
{
    //Create buffer
     string temp = "TURN:" + to_string(who);
     pair <int, int> card;

    for (int i = 1; i <= clients.size(); i++)
    {
        if (current.first == -2 && who == i){
            players[i].second += 4;
        }
        else if (current.first == 12 && who == i){
            players[i].second += 2;
        }
        temp += ":" + to_string(players[i].second);
    }
    temp += ":" + to_string(current.first) + ":" + to_string(current.second);
    if (current.first == -2){
        temp += ":4";
        for (int i = 0; i < 4; i++){
            card = get_card();
            temp += ":" + to_string(card.first) + ":" + to_string(card.second);
        }
    }
    else if (current.first == 12){
        temp += ":2";
        for (int i = 0; i < 2; i++){
            card = get_card();
            temp += ":" + to_string(card.first) + ":" + to_string(card.second);
        }
    }else{
        temp += ":0";
    }
    return temp;
}
// Create a string with a player requested card
string createRequest(map<int, pair<string, int> > &players, vector<int> &clients, int who, pair<int, int> &current)
{
    //Create buffer
    string temp = "TURN:" + to_string(who);
    pair <int, int> card;

    for (int i = 1; i <= clients.size(); i++)
    {
        if (i == who)
        {
            players[i].second += 1;
        }
        temp += ":" + to_string(players[i].second);
    }
    card = get_card();
    temp += ":" + to_string(current.first) + ":" + to_string(current.second) + ":1:" + to_string(card.first) + ":" + to_string(card.second);
    return temp;
}
//Send to all players who won
void finishGame(vector<int> &clients, string winer)
{
    char buffer[BUFFER_SIZE];
    // Send who wins
    sprintf(buffer,"win:%s",winer.c_str());
    for (int i = 0; i < clients.size(); i++){
        sendString(clients[i], buffer);
    }
}
