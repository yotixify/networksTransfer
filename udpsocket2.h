#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>

#define MAXBUFLEN 1048576
#define BUFF_SIZE 1048576  // 1MB
#define NUM_TRIALS 100

#define MYPORT2 "9015"	// the port users will be connecting to


using namespace std;

class Udpsocket2
{
	public:
		Udpsocket2(string serverUrl, string clientUrl);
		//~Udpsocket2();
		int sendData(string data, int dataSize);
		int openSocket();
		int getNumBytes();
		
		void listener(int &size, char* &dataString);
		
	private:
		string serverUrl;
		string clientUrl;
		int numBytes;
		
		void* get_in_addr(struct sockaddr *sa);
		
		int sockfd;
		struct addrinfo hints, *servinfo, *p;
		int rv;
		int numbytes;
		struct sockaddr_storage their_addr;
		
		socklen_t addr_len;
		char s[INET6_ADDRSTRLEN];
	
};

//Udpsocket2::~Udpsocket2()
//{
	//free(serializedData);
//}

Udpsocket2::Udpsocket2(string a, string b)
{
	serverUrl = a;
	clientUrl = b;

	numBytes = 0;
}

int Udpsocket2::openSocket()
{

	
	memset(&hints, 0, sizeof hints);
	hints.ai_socktype = SOCK_DGRAM;
	
	if ((rv = getaddrinfo(NULL, MYPORT2, &hints, &servinfo)) != 0) {
	#ifdef SOCTEXT
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
	#endif
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
			//perror("listener: socket");
			continue;
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			//perror("listener: bind");
			continue;
		}

		break;
	}

	//freeaddrinfo(servinfo);

	
	//close(sockfd);
	
	return 0;
}

void Udpsocket2::listener(int &numbytes, char* &dataString) {
	#ifdef SOCTEXT
		printf("listener: waiting to recvfrom...\n");
	#endif
	dataString = new char[MAXBUFLEN + 1];
	addr_len = sizeof their_addr;
	if ((numbytes = recvfrom(sockfd, dataString, MAXBUFLEN-1 , 0,
		(struct sockaddr *)&their_addr, &addr_len)) == -1) {
		//perror("recvfrom");
		exit(1);
	}
	
	dataString[MAXBUFLEN] = '\0';
	//cout << dataString << endl;
	#ifdef SOCTEXT
		printf("listener: got packet from %s\n",inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s));
	#endif
			
	FILE *binFile = fopen("sample2","wb");
        fwrite(dataString,sizeof(char),numbytes,binFile);
        fclose(binFile);
	
	
	//Packet p1(temp, numbytes);
	close(sockfd);
}

int Udpsocket2::sendData(string data, int dataSize)
{
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
	
	unsigned char* dataToSend = (unsigned char*)malloc(sizeof(unsigned char*) * dataSize + 1);
	for(int i = 0; i < dataSize; i++){
		int temp = (int)data.at(i);
		dataToSend[i] = (unsigned char)temp;
	}
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	
	if ((rv = getaddrinfo(clientUrl.c_str(), MYPORT2, &hints, &servinfo)) != 0) {
		#ifdef SOCTEXT
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		#endif
		return 1;
	}

	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
			//perror("listener: socket");
			continue;
		}



		break;
	}



	if ((numbytes = sendto(sockfd, dataToSend, dataSize, 0, p->ai_addr, p->ai_addrlen)) == -1) {
		//perror("talker: sendto");
		exit(1);
	}

	freeaddrinfo(servinfo);
	#ifdef SOCTEXT
		printf("talker: sent %d bytes to %s\n", numbytes, clientUrl.c_str());
	#endif
	close(sockfd);
	free(dataToSend);
	return 0;
}

// get sockaddr, IPv4 or IPv6:
void* Udpsocket2::get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int Udpsocket2::getNumBytes() {
	return numBytes;
}


