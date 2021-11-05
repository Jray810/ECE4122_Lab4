/**
 * Author: Raymond Jia
 * Class: ECE 4122 - Dr. Hurley
 * Lab 4
 * Last Date Modified: 11/05/2021
 * Description: This is the TCP Server program that opens a server on the specified port number
 *              and listens for connection requests and messages. All connections and messages are
 *              logged in an output file called server.log and all logs are appended to the file.
 *              The server can handle multiple clients as well as disconnecting and reconnecting by
 *              clients. The program runs infinitely until quit via the command line.
 **/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
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
 *          Performs TCP server operations on desired port number. Runs infinitely
 *          until forced quit in the command line
 **/
int main(int argc, char* argv[])
{
    // Check for valid number of command line inputs
    if (argc != 2)
    {
        cerr << "Invalid command line argument detected. Expected 2 arguments, received " << argc;
        cerr << "\nPlease check your values and press ENTER key to end the program!\n";
        cin.ignore();
        return ERROR;
    }

    // Check if port number argument is a positive integer
    string portString = argv[1];
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

    // Create server log
    ofstream fout ("server.log", ios::app);

    // Create a server socket to accept new connections
    sf::TcpListener listener;
    if (listener.listen(portNumber) != sf::Socket::Done)
    {
        cerr << "Unable to open desired port number.";
        cerr << "\nPlease check your values and press ENTER key to end the program!\n";
        cin.ignore();
        return ERROR;
    }

    // Make listener non blocking
    listener.setBlocking(false);

    // Create a selector to handle multiple sockets
    sf::SocketSelector selector;

    // Add the listener to the selector
    selector.add(listener);

    // Create a vector to store sockets
    vector<sf::TcpSocket*> sockets;

    // Runtime loop
    bool userQuit = false;  // Since server is infinite, this will always be false
    while (!userQuit)
    {
        // Wait for activity on any item in the selector
        if (selector.wait())
        {
            // Check if listener is ready to receive a new connection
            if (selector.isReady(listener))
            {
                // Listener detected activity, receive connection request
                sf::TcpSocket* socket = new sf::TcpSocket;
                
                // Receive the socket
                if (listener.accept(*socket) == sf::Socket::Done)
                {
                    // Add new socket to vector of sockets
                    sockets.push_back(socket);

                    // Add new socket to selector so selector will be notified when activity occurs
                    selector.add(*socket);

                    // Make socket non-blocking
                    socket->setBlocking(false);
                    
                    // Open output file stream if not already open
                    if (!fout.is_open())
                    {
                        fout.open("server.log", ios::app);
                    }

                    // Record message in server log
                    cout << "Client Connected\n";
                    fout << "Client Connected\n";

                    // Close output file stream
                    fout.close();
                }
                else
                {
                    // Socket reception failed, free up allocated memory
                    delete socket;
                }
            }
            else
            {
                // Listener was not ready, test other sockets
                for (int i = 0; i < sockets.size(); ++i)
                {
                    sf::TcpSocket* socket = sockets[i];

                    // Check if socket is ready to read
                    if (selector.isReady(*socket))
                    {
                        // Prepare to receive data
                        char in[128];
                        size_t received;

                        // Test socket and check for data
                        if (socket->receive(in, sizeof(in), received) == sf::Socket::Done)
                        {
                            // Open output file stream if not already open
                            if (!fout.is_open())
                            {
                                fout.open("server.log", ios::app);
                            }

                            // Record message in server log
                            cout << in << endl;
                            fout << in << endl;

                            // Close output file stream
                            fout.close();
                        }
                        else
                        {
                            // receive() failed, socket was disconnected
                            // Open output file stream if not already open
                            if (!fout.is_open())
                            {
                                fout.open("server.log", ios::app);
                            }

                            // Record message in server log
                            cout << "Client Disconnected\n";
                            fout << "Client Disconnected\n";

                            // Close output file stream
                            fout.close();

                            // Remove the socket from the selector
                            selector.remove(*socket);

                            // Disconnect the socket on server's end
                            socket->disconnect();

                            // Free up allocated memory
                            delete socket;

                            // Remove entry from sockets vector
                            sockets.erase(sockets.begin() + i);

                            // Re-align the for loop iterator
                            i--;
                        }
                    }
                }
            }
        }
    }
    
    // End of main
    return SUCCESS;
}