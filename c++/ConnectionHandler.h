#include "MessageIdentifiers.h"
#include "RakNetworkFactory.h"
#include "RakPeerInterface.h"
#include "RakNetTypes.h"
#include "NatPunchthrough.h"

#include <set>

#define SERVER_PORT 60005
#define CLIENT_PORT 60006
#define SLEEP_TIME 30
#define MAX_SIZE_OF_PACKET 256

using namespace std;
using namespace RakNet;

enum{
	ID_PYTHON_PICKLED_OBJECT = ID_USER_PACKET_ENUM + 1
};

class ConnectionHandler {
public:
	ConnectionHandler();
	~ConnectionHandler();
	
	char *startServer(int maxConnectionsAllowed, bool behindNAT, char *facilitatorAddress);
	void startFacilitator();
	char *startClient(char *serverIP, bool serverBehindNAT, char *facilitatorAddress);
	//void natSetRemoteGUID(char *remoteGUIDString);
	void sendData(char *pickledObject, int priority, int reliability, int channel, bool timeStamp, 
								 char *svUser, bool svBroadcast);
	void receiveData(char *type, char *data, int *delay, int *timeStamped, char *from, int *packetArrived);
	bool getIsServer();


private:
	/* Methods: */
	//unsigned char getPacketIdentifier(Packet *p);


	/* Variables: */
	RakPeerInterface* peer;
	bool isServer;
	bool isRunning;
	bool isConnected;

	/* NAT Stuff: */
	NatPunchthrough *natPunchthrough;
	bool serverBehindNAT;
	char *facilitatorAddress;

	//set<RakNetGUID> guids;

	/* NAT Stuff: */
	/*NatPunchthroughClient *natPunchthroughClient;	// NAT Step 1
	bool serverBehindNAT;
	RakNetGUID remoteGUID;*/
};
