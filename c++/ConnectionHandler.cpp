#include "ConnectionHandler.h"

#include <iostream>
#include <string>
#include <fstream>

#include <set>


#include "MessageIdentifiers.h"
#include "RakNetworkFactory.h"
#include "RakPeerInterface.h"
#include "RakNetTypes.h"
#include "BitStream.h"
#include "GetTime.h"
#include "StringCompressor.h"



//npt:
#include "RakSleep.h"
#include "NatPunchthrough.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"


// npt:
/*
#include "RakPeerInterface.h"
#include "RakSleep.h"
#include <stdio.h>
#include <stdlib.h>
#include "RakNetworkFactory.h"
#include <string.h>
#include "Kbhit.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "RakSleep.h"
#include "ConnectionGraph2.h"
#include "PacketLogger.h"
#include "StringCompressor.h"
#include "PacketFileLogger.h"
#include "GetTime.h"
#include "NatPunchthroughClient.h"
#include "NatPunchthroughServer.h"
#include "SocketLayer.h"
#include "RakString.h"
#include "UDPProxyServer.h"
#include "UDPProxyCoordinator.h"
#include "UDPProxyClient.h"
#include "NatTypeDetectionServer.h"
*/






#define DEFAULT_NAT_FACILITATOR_IP "127.0.0.1" //"8.17.250.34"	// free server provided by RakNet
#define DEFAULT_NAT_FACILITATOR_PORT 60481
// 94.198.81.195 for version 3.7 and later
// 8.17.250.34 otherwise, however this server seems to be down

#define DEFAULT_NAT_RECEIVER_IP "127.0.0.1"
#define DEFAULT_NAT_RECEIVER_PORT 60524

#define DEFAULT_NAT_SENDER_IP "127.0.0.1"
#define DEFAULT_NAT_SENDER_PORT 60503


//#define NAT_PUNCHTHROUGH_FACILITATOR_PASSWORD ""



using namespace std;
using namespace RakNet;

ConnectionHandler::ConnectionHandler() {
	peer = RakNetworkFactory::GetRakPeerInterface();

	natPunchthrough = new NatPunchthrough();
	//NatPunchthroughLogger l;
	//natPunchthrough->SetLogger(&l);

	peer->AttachPlugin(natPunchthrough);


	isConnected = false;
	isRunning = false;
}


ConnectionHandler::~ConnectionHandler() {
	// kick all users
	peer->Shutdown(300);
	RakNetworkFactory::DestroyRakPeerInterface(peer);
}

char *ConnectionHandler::startServer(int maxConnectionsAllowed, bool behindNAT, char *facilitatorAddress) {
	if(!isRunning) {
		isServer = true;
		isRunning = true;
		this->serverBehindNAT = behindNAT;
		if(behindNAT) { // receiver

			SocketDescriptor socketDescriptor(DEFAULT_NAT_RECEIVER_PORT, 0);
			peer->Startup(maxConnectionsAllowed+1, 0, &socketDescriptor, 1);	// 2 incoming?
			peer->SetMaximumIncomingConnections(1);
			peer->Connect(facilitatorAddress, DEFAULT_NAT_FACILITATOR_PORT, 0, 0);
			peer->SetMaximumIncomingConnections(maxConnectionsAllowed+1);

			/*
			this->natPunchthroughClient = new NatPunchthroughClient();
			//NatPunchthroughClient::SetDebugInterface();

			peer->AttachPlugin(natPunchthroughClient);		// NAT Step 2
			peer->Connect(DEFAULT_NAT_PUNCHTHROUGH_FACILITATOR_IP, NAT_PUNCHTHROUGH_FACILITATOR_PORT, 0, 0);	// NAT Step 3

			RakNetGUID myGUID = peer->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS);
			char* myGUIDString;
			myGUID.ToString(myGUIDString);
			return myGUIDString;
			*/
			return NULL;
		}
		else {
			cout << "2.2.1.1\n";
			peer->Startup(maxConnectionsAllowed, SLEEP_TIME, &SocketDescriptor(SERVER_PORT, 0), 1);
			peer->SetMaximumIncomingConnections(maxConnectionsAllowed);
			return NULL;
		}
	}
	else {
		// raise something
		return NULL;
	}
}

void ConnectionHandler::startFacilitator() {
	if(!isRunning) {
		isServer = true;
		isRunning = true;

		SocketDescriptor socketDescriptor(DEFAULT_NAT_FACILITATOR_PORT, 0);
		peer->Startup(32, 0, &socketDescriptor, 1);
		peer->SetMaximumIncomingConnections(32);
		natPunchthrough->FacilitateConnections(true);
	}
}


char *ConnectionHandler::startClient(char *serverIP, bool serverBehindNAT, char *facilitatorAddress) {
	if(!isRunning) {
		isRunning = true;
		isServer = false;
		this->serverBehindNAT = serverBehindNAT;
		if(serverBehindNAT) {	// sender
			SocketDescriptor socketDescriptor(DEFAULT_NAT_SENDER_PORT, 0);
			peer->Startup(2, 0, &socketDescriptor, 1);
			peer->Connect(facilitatorAddress, DEFAULT_NAT_FACILITATOR_PORT, 0, 0);
			this->facilitatorAddress = facilitatorAddress;	// when is the value of *facilitatorAddress released??

			//NatPunchthroughClient::SetDebugInterface();
			/*
			this->natPunchthroughClient = new NatPunchthroughClient();	// NAT Step 1
			peer->AttachPlugin(natPunchthroughClient);		// NAT Step 2
			peer->Connect(DEFAULT_NAT_PUNCHTHROUGH_FACILITATOR_IP, NAT_PUNCHTHROUGH_FACILITATOR_PORT, 0, 0);	// NAT Step 3

			RakNetGUID myGUID = peer->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS);
			char* myGUIDString;
			myGUID.ToString(myGUIDString);
			return myGUIDString;
			*/
			return NULL;
		}
		else {
			peer->Startup(1, SLEEP_TIME, &SocketDescriptor(), 1);
			peer->Connect(serverIP, SERVER_PORT, 0, 0);
			return NULL;
		}
	}
	else {
		// raise something
		return NULL;
	}
}

/*
void ConnectionHandler::natSetRemoteGUID(char *remoteGUIDString) {
	remoteGUID.FromString(remoteGUIDString);
	cout << "remoteGUID to string: " << remoteGUID.ToString() << endl;
	// Be sure to wait before this method is called so our client is connected to the server
	SystemAddress serverSystemAddress(DEFAULT_NAT_PUNCHTHROUGH_FACILITATOR_IP, NAT_PUNCHTHROUGH_FACILITATOR_PORT);
	cout << 2 << endl;
	natPunchthroughClient->OpenNAT(remoteGUID, serverSystemAddress);	// NAT Step 4
	cout << 3 << endl;
}
*/


void ConnectionHandler::sendData(char *pickledObject, int priority, int reliability, int channel, bool timeStamp, 
								 char *svUser, bool svBroadcast) {

	BitStream myBitStream;
	if(timeStamp) {
		MessageID useTimeStamp = ID_TIMESTAMP; // Assign this to ID_TIMESTAMP
		RakNetTime theTimeStamp = RakNet::GetTime(); // Put the system time in here returned by RakNet::GetTime()
		myBitStream.Write(useTimeStamp);
		myBitStream.Write(theTimeStamp);	// don't mix with the boolean timeStamp!!
	}

	MessageID typeId = ID_PYTHON_PICKLED_OBJECT; // This will be assigned to a type I've added after ID_USER_PACKET_ENUM, lets say ID_SET_TIMED_MINE
	myBitStream.Write(typeId);
	stringCompressor->EncodeString(pickledObject, MAX_SIZE_OF_PACKET, &myBitStream);
	
	if(isServer) {
		/*
		RakNetGUID guid;
		guid.FromString(svUser);
		if( guids.find(guid) != guids.end() ) {
			peer->Send(&myBitStream, (PacketPriority)priority, (PacketReliability)reliability, 0, guid, svBroadcast);
		}
		else if(svUser == NULL) {
			peer->Send(&myBitStream, (PacketPriority)priority, (PacketReliability)reliability, 0, UNASSIGNED_SYSTEM_ADDRESS, svBroadcast);
		}
		else {
			printf("Server error: Non-existing user was referenced.\n");
		}
		*/
		peer->Send(&myBitStream, (PacketPriority)priority, (PacketReliability)reliability, 0, UNASSIGNED_SYSTEM_ADDRESS, svBroadcast);
	}
	else {
		peer->Send(&myBitStream, (PacketPriority)priority, (PacketReliability)reliability, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
}


void ConnectionHandler::receiveData(char type[], char data[], int *delay, int *timeStamped, char from[], int *packetArrived) { // string *pickledObject, int *delay, int *fromUser
	char str[512];
	//type = str;

	//type = NULL;
	Packet *packet = peer->Receive();

	*packetArrived = false;
	if(packet) {
		*packetArrived = true;

		BitStream myBitStream(packet->data, packet->length, false); // The false is for efficiency so we don't make a copy of the passed data

		MessageID typeID;
		myBitStream.Read(typeID);

		RakNetTime theTimeStamp;
		if (typeID == ID_TIMESTAMP) {
			*timeStamped = true;

			myBitStream.Read(theTimeStamp);
			myBitStream.Read(typeID);
			*delay = (int)(RakNet::GetTime() - theTimeStamp);
		}
		else {
			*timeStamped = false;
		}

		//RakString *rakString = new RakString();
		//cout << "typeID: " << (unsigned int)typeID << endl;
		switch (typeID)
		{
		case ID_PYTHON_PICKLED_OBJECT:
			stringCompressor->DecodeString(str, MAX_SIZE_OF_PACKET, &myBitStream);
			strcpy(data, str);
			strcpy(type, "userDefinedData");
			break;
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			printf("Another client has disconnected.\n");
			strcpy(type, "remoteClientDisconnected");
			break;
		case ID_REMOTE_CONNECTION_LOST:
			printf("Another client has lost the connection.\n");
			strcpy(type, "remoteClientLostConnection");
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION:
			printf("Another client has connected.\n");
			strcpy(type, "remoteClientConnected");
			break;
		case ID_CONNECTION_REQUEST_ACCEPTED:
			printf("Our connection request has been accepted.\n");

			if (packet->systemAddress.port==DEFAULT_NAT_FACILITATOR_PORT && serverBehindNAT && !isServer) {
				//SystemAddress facilitatorSystemAddress;
				//facilitatorSystemAddress.SetBinaryAddress(this->facilitatorAddress);	// might already been released
				//facilitatorSystemAddress.port = DEFAULT_NAT_FACILITATOR_PORT;

				natPunchthrough->Connect(DEFAULT_NAT_RECEIVER_IP, DEFAULT_NAT_RECEIVER_PORT, 0, 0, packet->systemAddress);
			}
			if(packet->systemAddress.port==DEFAULT_NAT_FACILITATOR_PORT && serverBehindNAT) {
				strcpy(type, "connectedToFacilitator");
			}
			else {
				printf("Connected to host!\n");
				strcpy(type, "connected");
				isConnected = true;
			}
			break;
			
		case ID_NEW_INCOMING_CONNECTION:
			printf("A connection is incoming.\n");
			strcpy(type, "incomingConnection");
			//guids.insert(packet->guid);
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			printf("The server is full.\n");
			strcpy(type, "serverFull");
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			if (isServer){
				printf("A client has disconnected.\n");
				strcpy(type, "clientDisconnected");
				//guids.erase(packet->guid);
			} else {
				printf("We have been disconnected.\n");
				strcpy(type, "disconnected");
			}
			break;
		case ID_CONNECTION_LOST:
			if (isServer){
				printf("A client lost the connection.\n");
				strcpy(type, "clientLostConnection");
				//guids.erase(packet->guid);
			} else {
				printf("Connection lost.\n");
				strcpy(type, "lostConnection");
			}
			break;
/*
		case ID_NAT_PUNCHTHROUGH_SUCCEEDED:	// NAT Step (5 and) 6
			printf("Connection through NPT succeeded!\n");
			strcpy(type, "connected");
			isConnected = true;
			break;
*/
		default:
			printf("Message with identifier %i has arrived.\n", typeID);
			strcpy(type, "anotherMessageID"); //"messageID_%i", typeID);
			break;
		}
	}

	peer->DeallocatePacket(packet);
	//delete packet;
}








bool ConnectionHandler::getIsServer() {
	return isServer;
}






/* Private methods: */
/*
unsigned char ConnectionHandler::getPacketIdentifier(Packet *p) {
	if ((unsigned char)p->data[0] == ID_TIMESTAMP) {
		return (unsigned char) p->data[sizeof(unsigned char) + sizeof(unsigned long)];
	}
	else {
		return (unsigned char) p->data[0];
	}
}
*/
