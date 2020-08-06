
#include "Complements.h"

module Coordinator @safe(){
	uses{
		interface Boot;
		interface Packet;
		interface AMPacket;
		interface AMSend;
		interface Receive;
		interface SplitControl as AMControl;
		interface Timer<TMilli> as Timer;
		interface ComponentesSensor as compSensor;
		interface ComponentesCluster as compCluster;
		interface ComponentesLibMessage as compLibMSG;
		interface ComponentesLibAggregation as compLibAggregation;
	}
} implementation {
	enum Message_Type { MAXWINNER = 1, MINWINNER, CMANNOUNCE, ELECTEDCH, GWANNOUNCE } IDMSG;
	uint16_t controlState;
	message_t pkt;
    	const int dHops = 2;
	const int tFloodMAX = 25;
	const int tFloodMIN = 25;
	const int tCluster = 25;
	const int tElectedCH = 25;
	const double tExit = 0.10;
	int round, msgId, myID, winnerID, myCH;
	int sensorList[20];
	int ListMaxWinner[20];
	int ListMinWinner[20];
	int listKnownChs[20];

	void state_INI(){
		controlState = 1;
		myID = call compSensor.getSensorId( );
		winnerID = myID;
		state_FloodMax(winnerID);
	}

	void state_FloodMax(int maxWinnerID){
		controlState = 2;
		msgId = call compLibMSG.GetNextMsgId( );
		msgMaxWinner* pktMaxWinner = (msgMaxWinner*)(call Packet.getPayload(&pkt, sizeof(msgMaxWinner)));
		pktMaxWinner->winnerID = maxWinnerID;
		call AMSend.send(AM_BROADCAST_ADDR, &pkt, sizeof(msgMaxWinner));
		state_StoreMaxWinner();
	}

	void state_StoreMaxWinner(){
		controlState = 3;
		call Timer.startOneShot(tFloodMAX);
	}

	void state_SelectMaxWinner(){
		controlState = 4;
		winnerID = call compLibAggregation.MAX(ListMaxWinner );
		if( round<dHops ){
			round++;
			state_FloodMax(winnerID);
		} else {
			round = 0;
			state_FloodMin(myID);
		}
	}

	void state_FloodMin(	int minWinnerID){
		controlState = 5;
		msgId = call compLibMSG.GetNextMsgId( );
		msgMinWinner* pktMinWinner = (msgMinWinner*)(call Packet.getPayload(&pkt, sizeof(msgMinWinner)));
		pktMinWinner->winnerID = minWinnerID;
		call AMSend.send(AM_BROADCAST_ADDR, &pkt, sizeof(msgMinWinner));
		state_StoreMinWinner();
	}

	void state_StoreMinWinner(){
		controlState = 6;
		call Timer.startOneShot(tFloodMIN);
	}

	void state_SelectMinWinner(){
		controlState = 7;
		winnerID = call compLibAggregation.MIN(ListMinWinner );
		if( round<dHops ){
			round++;
			state_FloodMin(winnerID);
		} else {
			round = 0;
			state_SelectCH();
		}
	}

	void state_SelectCH(){
		controlState = 8;
		myCH = call compCluster.selectCH(ListMaxWinner, ListMinWinner );
		state_ClusterFormation();
	}

	void state_ClusterFormation(){
		controlState = 9;
		msgCmAnnounce* pktCmAnnounce = (msgCmAnnounce*)(call Packet.getPayload(&pkt, sizeof(msgCmAnnounce)));
		pktCmAnnounce->myID = myID;
		compList.insert(sensorList, myCH);
		msgId = call compLibMSG.GetNextMsgId( );
		int auxState9;
		while(sensorList[auxState9]!=0){
			call AMSend.send(sensorList[auxState9], &pkt, sizeof(msgCmAnnounce));
			auxState9++;
		}
		state_StoreMembers();
	}

	void state_StoreMembers(){
		controlState = 10;
		compSensor->role = CM;
		call Timer.startOneShot(tCluster);
	}

	void state_ElectedCH(){
		controlState = 11;
		msgElectedCh* pktElectedCh = (msgElectedCh*)(call Packet.getPayload(&pkt, sizeof(msgElectedCh)));
		pktElectedCh->myID = myID;
		pktElectedCh->myCH = myCH;
		msgId = call compLibMSG.GetNextMsgId( );
		call AMSend.send(AM_BROADCAST_ADDR, &pkt, sizeof(msgElectedCh));
		state_StoreKnownCHs();
	}

	void state_StoreKnownCHs(){
		controlState = 12;
		call Timer.startOneShot(tElectedCH);
	}

	void state_BackboneFormation(){
		controlState = 13;
		if( call compGW.selectGW(ListKnownChs) ){
				msgGwAnnounce* pktGwAnnounce = (msgGwAnnounce*)(call Packet.getPayload(&pkt, sizeof()));
				pktGwAnnounce->myID = myID;
				msgId = call compLibMSG.GetNextMsgId( );
				int auxState13;
				while(listKnownChs[auxState13]!=0){
					call AMSend.send(listKnownChs[auxState13], &pkt, sizeof());
					auxState13++;
				}
			compSensor->role = GW;
			state_exit();
		} else {
			state_StoreGateways();
		}
	}

	void state_StoreGateways(){
		controlState = 14;
		call Timer.startOneShot(tExit);
	}

	void state_exit( ){
		call AMControl.stop();
	}

	/* NesC language envents */
	event void Boot.booted() {
		call AMControl.start();
		state_INI();
	}

	// Timer fired
	event void Timer.fired(){
		if(controlState == 3){
	 		state_SelectMaxWinner( );
 		} else if(controlState == 6){
 			state_SelectMinWinner( );
 		} else if(controlState == 10){
 			state_ElectedCH( );
 		} else if(controlState == 12){
 			state_BackboneFormation( );
 		} else if(controlState == 14){
 			state_exit();
 		}
	}

	// Receive
	event message_t* Receive.receive(message_t* msg, void* payload, uint8_t len){
		//O desempacotamento deve ser realizado em cada IF
		if(controlState == 3){
			msgMaxWinner* pktMaxWinner = (msgMaxWinner*)payload;
			compList.insert(ListMaxWinner, pktMaxWinner->winnerID);
		} else if(controlState == 6){
			msgMinWinner* pktMinWinner = (msgMinWinner*)payload;
			ListMinWinner( pktMinWinner->winnerID );
		} else if(controlState == 10){
			msgCmAnnounce* pktCmAnnounce = (msgCmAnnounce*)payload;
			compSensor->role = CH;
			call compCH.setMembers(pktCmAnnounce->myID );
		} else if(controlState == 12){
			msgElectedCh* pktElectedCh = (msgElectedCh*)payload;
			compList.insert(ListKnownChs, pktElectedCh->myCH);
		} else if(controlState == 14){
			msgGwAnnounce* pktGwAnnounce = (msgGwAnnounce*)payload;
			call compCH.setListGW(pktGwAnnounce->myID );
		}
		return msg;
	}

	event void AMControl.startDone(error_t err){ }
	event void AMControl.stopDone(error_t err){ }
	event void AMSend.sendDone(message* msg, error_t err) { }
}
