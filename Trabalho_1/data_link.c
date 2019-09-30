#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "data_link.h"
#include "statemachine.h"
#include "aux.h"

// global variables
struct linkLayer ll;
struct termios oldtio;

/**
 * Opens the connection for the receiver
 * @param File descriptor for the serial port
 * @return File descriptor; -1 in case of error
 */
int llOpenReceiver(int fd) {


    if(readSupervisionFrame(ll.frame, fd) == -1)
        return -1;

    if(ll.frame[1] != END_SEND){
        printf("NAO FUNCIONOU");
        return -1;
    }
        
    if(ll.frame[2] == SET){
        printf("FUNCIONOU");
    }
    else{
        printf("NAO FUNCIONOU");
        return -1;
    }
    

    if(createSupervisionFrame(ll.frame, UA, RECEIVER) != 0)
        return -1;
    
    // send SET frame to receiver
    if(sendFrame(ll.frame, fd) != 0)
        return -1;

    return fd;
}


/**
 * Opens the connection for the transmitter
 * @param File descriptor for the serial port
 * @return File descriptor; -1 in case of error
 */
int llOpenTransmitter(int fd) {

    // creates SET frame
    if(createSupervisionFrame(ll.frame, SET, TRANSMITTER) != 0)
        return -1;

    // send SET frame to receiver
    if(sendFrame(ll.frame, fd) != 0)
        return -1;
    
    //alarm(TIMEOUT);

    if(readSupervisionFrame(ll.frame, fd) == -1)
        return -1;
    
    if(ll.frame[1] != END_SEND){
        printf("NAO FUNCIONOU");
        return -1;
    }
        
    if(ll.frame[2] == UA){
        printf("FUNCIONOU");
    }
    else{
        printf("NAO FUNCIONOU");
        return -1;
    }

    return fd;
}

/**
 * Function that opens and establishes the connection between the receiver and the transmitter
 * @param port Port name
 * @param role Flag that indicates the transmitter or the receiver
 * @return File descriptor; -1 in case of error 
 */
int llopen(char* port, int role) {

    int fd;

    // open, in non canonical
    if((fd = openNonCanonical(port, &oldtio, VTIME_VALUE, VMIN_VALUE)) == -1)
      return -1;


    // fills linkLayer fields
    strcpy(ll.port, port);
    ll.baudRate = BAUDRATE;
    ll.numTransmissions = NUM_RETR;
    ll.timeout = TIMEOUT;


    if(role == TRANSMITTER) {
        return llOpenTransmitter(fd);
    }
    else if(role == RECEIVER) {
        return llOpenReceiver(fd);
    }

    perror("Invalid role");
    return -1;
}
