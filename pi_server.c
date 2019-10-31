/*
    Server program to compute the value of PI
    This program calls the library function 'get_pi'
    It will create child processes to attend requests
    It receives connections using sockets

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
#include "get_pi.h"
#include "sockets.h"
#include "fatal_error.h"

#define BUFFER_SIZE 1024
#define MAX_QUEUE 5


///// FUNCTION DECLARATIONS
void usage(char * program);
void waitForConnections(int server_fd);
void attendRequest(int client_fd);
void setupHandlers();
void onInterrupt(int signal);

int interrupted = 0;
///// MAIN FUNCTION
int main(int argc, char * argv[])
{
    int server_fd;

    printf("\n=== SERVER FOR COMPUTING THE VALUE OF pi ===\n");

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
    pid_t new_pid;

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
        // Wait for a client connection
        while(1)
        {
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
                printf("A client wants to connect 111\n");
                if (poll_fd[0].revents & POLLIN)
                {
                    // ACCEPT
                    // Get the file descriptor to talk with the client
                    client_fd = accept(server_fd, (struct sockaddr *) &client_address,
                                &client_address_size);
                    break;
                }
            }
        }
         
        // Get the data from the client
        inet_ntop(client_address.sin_family, &client_address.sin_addr, client_presentation, sizeof client_presentation);
        printf("Received incomming connection from %s on port %d\n", client_presentation, client_address.sin_port);

        // FORK
        // Create a new child process to deal with the client

        // Fork if not interrupted
        if (!interrupted)
        {            
            new_pid = fork();
            // Parent
            if (new_pid > 0)
            {
                // Close the new socket
                close(client_fd);
            }
            else if (new_pid == 0)
            {
                // Close the main server socket to avoid interfering with the parent
                close(server_fd);
                printf("Child process %d dealing with client\n", getpid());

                setupHandlers();
                // Deal with the client
                attendRequest(client_fd);
                // Close the new socket
                close(client_fd);
                // Terminate the child process
                exit(EXIT_SUCCESS);
            }
            else
            {
                fatalError("fork");
            }
        }
    }
}

/*
    Hear the request from the client and send an answer
*/
void attendRequest(int client_fd)
{
    char buffer[BUFFER_SIZE];
    unsigned long int iterations;
    // Variables used to calculate PI.
    double result = 4;
    int sign = -1;
    unsigned long int divisor = 3;
    unsigned long int counter = 0;
     // Variables for polling
    struct pollfd poll_fd[1];
    int poll_response;
    int timeout = 0;     // Timeout of one second
    int client_signal = 0;
    // Prepare for the poll
    poll_fd[0].fd = client_fd;
    poll_fd[0].events = POLLIN;

    // RECV
    // Receive the request
    recvString(client_fd, buffer, BUFFER_SIZE);
    sscanf(buffer, "%lu", &iterations);
    printf(" > Got request from client with iterations=%lu\n", iterations);
     
    // Loop used to calculate PI while not interruptes.
    while((counter < iterations) && !interrupted)
    {
        result += sign * (4.0/divisor);
        sign *= -1;
        divisor += 2;

        // Listen to client signals.
        poll_response = poll(poll_fd, 1, timeout);
        if (poll_response == 0)     // Nothing to receive
        {
            fflush(stdout);  
        }
        else if (poll_response == -1)
        {
            perror("poll");
            break;
        }
        else
        {
            if (poll_fd[0].revents & POLLIN)
            {
                // Receive client signal and break the loop.
                recvString(client_fd, buffer, BUFFER_SIZE); 
                sscanf(buffer, "%d", &client_signal);
                printf("\n The client sent a signal.\n");
                break;
            }
        }
        counter++;
    }

    // Prepare buffer.          
    sprintf(buffer, "%d %lu %.20lf", client_signal, counter, result);

    if (interrupted) 
    {
        printf(" The server was interrupted \n");
        // Send buffer.
        sendString(client_fd, buffer); 
    }
   
    // Verify results.
    printf(" > Sending PI=%.20lf\n", result);
    printf(" > Number of iterations: %lu\n", counter);

    // SEND
    // Send the response
    sendString(client_fd, buffer);
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