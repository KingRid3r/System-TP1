//R. Raffin IUT Aix-Marseille, dépt. Informatique, Arles
//Code Client pour les Sockets
//se connecte a un serveur dont l'Ip est passee en param du prog
//recoit une chaine de caracteres du serveur, l'affiche et quitte
//Compilation : g++ -Wall sockcli.cpp -o client

#include <iostream>

#include <cstdlib>
#include <unistd.h>
#include <cstring>

//pour les sockets
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define MAXDATASIZE 100

using namespace std;

class client {
	private:
		int fd;
		int port;
		struct sockaddr_in sockclient;
	public:
		client(const char*, int);
		~client(void);
		void connexion(void);
		int reception(void);
		int emission(const char*);
	};


client::client(const char* _serveur, int _port = 3550) {
struct hostent *he;

	//est-ce que le serveur existe ? (adresse ip valide)
	if ((he = gethostbyname(_serveur)) == NULL){
		cerr << "Client : erreur gethostbyname()" << endl;
		exit(-1);
	}

	//creation d'un socket
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		cerr << "Client : erreur socket()" << endl;
		exit(-1);
	}

	sockclient.sin_family = AF_INET;
	sockclient.sin_port = htons(_port);
	sockclient.sin_addr = *((struct in_addr *)he->h_addr);
	bzero(&(sockclient.sin_zero),8);
}

client::~client(void) {
	close(fd);
}

void client::connexion(void) {

	if(connect(fd, (struct sockaddr *)&sockclient,sizeof(struct sockaddr)) == -1){
		cerr << "Client : erreur connect()" << endl;
		exit(-1);
	}
}

int client::reception(void) {
int numbytes;
char buf[MAXDATASIZE];

	if ((numbytes = recv(fd,buf,MAXDATASIZE,0)) == -1){
		cerr << "Client : erreur recv()" << endl;
		exit(-1);
	}
	buf[numbytes] = '\0';
	cout << "Client : message du serveur : " << buf << endl;

return numbytes;
}


int client::emission(const char* _s) {
int r;

	r = send(fd, (void *) _s, (size_t) strlen(_s), 0);
	return r;
}

int main(int argc, char *argv[])
{

if (argc < 2) {
	cerr << "Usage: " << argv[0] << " <Addresse IP serveur> [<port>]" << endl;
	exit(-1);
}

client * monclient;

//Si le client veut se connecter sur un numéro de port particulier
if (argc == 3) {
	unsigned int port = strtol(argv[2], (char **) NULL, 10);
	monclient = new client (argv[1], port);
}
else {
	monclient = new client (argv[1]);
}

monclient -> connexion();

bool fincomm = false;
string message="bonjour";


while (fincomm == false) {

	monclient -> emission(message.c_str());

	cin >> message ;
	//monclient -> reception();
	if ( message == "FIN" )
		fincomm = true;
	}

return EXIT_SUCCESS;
}

