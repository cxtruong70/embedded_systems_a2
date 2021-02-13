// Socket.h
// Module to listen and respond to commands from a given port
// where each packet is to be treated as a new command to respond to.
// This thread will then reply back to the sender with one or more
// UDP packets containing the "return" message (plain text)

#ifndef _Socket_H_
#define _Socket_H_

// Begin/end the background thread which listens for packets
void Socket_startListening(void);
void Socket_stopListening(void);



#endif