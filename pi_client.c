/*
    Client program to get the value of PI
    This program connects to the server using sockets

    Gilberto Echeverria
    gilecheverria@yahoo.com
    21/10/2018


    Modified by Mauricio Peón García
    A01024162
    27/10/2019
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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

int interrupted = 0;

///// FUNCTION DECLARATIONS
void usage(char * program);
void requestPI(int connection_fd);
void setupHandlers();
void onInterrupt(int signal);

///// MAIN FUNCTION
int main(int argc, char * argv[])
{
    int connection_fd;

    printf("\n=== CLIENT FOR COMPUTING THE VALUE OF pi ===\n");

    // Check the correct arguments
    if (argc != 3)
    {
        usage(argv[0]);
    }

    setupHandlers();
    // Start the server
    connection_fd = connectSocket(argv[1], argv[2]);
	// Listen for connections from the clients
    requestPI(connection_fd);
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

void requestPI(int connection_fd)
{
    char buffer[BUFFER_SIZE];
    unsigned long int iterations;
    // Variables to receive PI.
    unsigned long int counter = 0;
    double result;
    int server_signal = 0;
     // Variables for polling
    struct pollfd poll_fd[1];
    int poll_response;
    int timeout = 1000;     // Timeout of one second

    // Prepare for the poll
    poll_fd[0].fd = connection_fd;
    poll_fd[0].events = POLLIN;

    printf("Enter the number of iterations for PI: ");
    scanf("%lu", &iterations);

    // Prepare the response to the client
    sprintf(buffer, "%lu", iterations);
    // SEND
    // Send the response
    sendString(connection_fd, buffer);
    
    // While not interrupted receive signals from the server.
    while(!interrupted)
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
    }
    
    // If interrupted, send signal to the server.
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