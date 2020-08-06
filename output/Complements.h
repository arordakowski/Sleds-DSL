
#ifndef COMPLEMENTS_H
#define COMPLEMENTS_H

enum {
	AM = 6,
	TIMER_PERIOD_MILLI = 150
};

typedef nx_struct msgMaxWinner{
	nx_uint16_t winnerID;
} msgMaxWinner;
 
typedef nx_struct msgMinWinner{
	nx_uint16_t winnerID;
} msgMinWinner;
 
typedef nx_struct msgCmAnnounce{
	nx_uint16_t myID;
} msgCmAnnounce;
 
typedef nx_struct msgElectedCh{
	nx_uint16_t myID;
	nx_uint16_t myCH;
} msgElectedCh;
 
typedef nx_struct msgGwAnnounce{
	nx_uint16_t myID;
} msgGwAnnounce;
 
#endif
