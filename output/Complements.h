
#ifndef COMPLEMENTS_H
#define COMPLEMENTS_H

enum {
	AM = 6,
	TIMER_PERIOD_MILLI = 150
};

typedef nx_struct msgMaxWinner{
	int winnerID;

} msgMaxWinner;
 
typedef nx_struct msgMinWinner{
	int winnerID;

} msgMinWinner;
 
typedef nx_struct msgCmAnnounce{
	int myID;

} msgCmAnnounce;
 
typedef nx_struct msgElectedCh{
	int myID;
	int myCH;

} msgElectedCh;
 
typedef nx_struct msgGwAnnounce{
	int myID;

} msgGwAnnounce;
 
#endif