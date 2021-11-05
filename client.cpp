/**
 * Author: Raymond Jia
 * Class: ECE 4122 - Dr. Hurley
 * Lab 4
 * Last Date Modified: 11/05/2021
 * Description: This is the TCP Client program that opens a connection to the specified IP Address
 *              and port number and sends messages to the server. The program will repeatedly
 *              prompt the user for new messages until the user types "exit". * 
 **/

#include <iostream>
#include <cstring>
#include <SFML/Network.hpp>

#define ERROR 1
#define SUCCESS 0

using namespace std;

/**
 * Main
 * Inputs:
 *          argc - Number of command line arguments
 *          argv - Array of character pointers that point to command line arguments
 * Description:
 *          Performs TCP client operations on desired IP Address and port number.
 **/
int main(int argc, char* argv[])
{
    // Check for valid number of command line inputs
    if (argc != 3)
    {
        cerr << "Invalid command line argument detected. Expected 3 arguments, received " << argc;
        cerr << "\nPlease check your values and press ENTER key to end the program!\n";
        cin.ignore();
        return ERROR;
    }

    // Check if requested IP Address is valid
    string ipAddress = argv[1];
    sf::IpAddress server(ipAddress);
    if (server == sf::IpAddress::None)
    {
        cerr << "Invalid command line argument detected: " << ipAddress;
        cerr << "\nPlease check your values and press ENTER key to end the program!\n";
        cin.ignore();
        return ERROR;
    }

    // Check if port number argument is a positive integer
    string portString = argv[2];
    for (int i=0; i < portString.length(); ++i)
    {
        if (isdigit(portString[i]) == false)
        {
            cerr << "Invalid command line argument detected: " << portString;
            cerr << "\nPlease check your values and press ENTER key to end the program!\n";
            cin.ignore();
            return ERROR;
        }
    }

    // Check if port number is within bounds [61000,65535]
    unsigned int portNumber = stoi(portString, nullptr, 0);
    if (portNumber < 61000 || portNumber > 65535)
    {
        cerr << "Invalid command line argument detected: " << portNumber;
        cerr << "\nPlease check your values and press ENTER key to end the program!\n";
        cin.ignore();
        return ERROR;
    }

    // Create socket for communicating with the server
    sf::TcpSocket socket;

    // Connect to the server, timeout of 1 second
    if (socket.connect(server, portNumber, sf::seconds(1)) != sf::Socket::Done)
    {
        cerr << "Failed to connect to the server at " << ipAddress << " on " << portNumber;
        cerr << "\nPlease check your values and press ENTER key to end the program!\n";
        cin.ignore();
        return ERROR;
    }
    else
    {
        cout << "Connection successful. Type \"exit\" at any time to end the program.\n\n";
    }

    // Runtime loop
    bool userQuit = false;      // Becomes true when user types "exit"
    string userMessage;    // Used for collecting user input
    while (!userQuit)
    {
        // Ask for user input
        cout << "Please enter a message:\n";

        // Array to hold message
        char out[128];

        // Retrieve message from console
        getline(cin, userMessage);

        // Check if it was an "exit" request
        if (userMessage == "exit")
        {
            // Set conditions to exit loop
            userQuit = true;
            continue;
        }

        // Copy message over to char array for sending. Max 128 characters.
        strncpy(out, userMessage.c_str(), 128);

        // Send message to server
        if (socket.send(out, sizeof(out)) != sf::Socket::Done)
        {
            cerr << "Send failed. Check if server has been closed.";
            cerr << "\nPress ENTER key to end the program!\n";
            cin.ignore();

            // Set conditions to exit loop
            userQuit = true;
            continue;
        }
    }

    // Disconnect the socket
    socket.disconnect();
    
    // End of main
    return SUCCESS;
}