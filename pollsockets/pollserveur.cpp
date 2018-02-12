//R. Raffin IUT Aix-Marseille, dépt. Informatique, Arles
//Code Serveur pour les Sockets sans blocage (polling par une alarme)

#include <iostream>
#include <cstdlib>

//pour errno()
#include <errno.h>


//pour bzero()
#include <cstring>

//pour htons()
#include <arpa/inet.h>

//pour struct sockaddr_in, socket(), listen(), bind(), ...
#include <sys/types.h>
#include <sys/socket.h>

//pour ioctl
#include <sys/ioctl.h>

//pour la struct pollfd et poll()
#include <poll.h>


#define BACKLOG 1
#define MAXDATASIZE 100


using namespace std;

char * buffer;

void terminaison(void) {

delete[] buffer;

cerr << "Fin du programme ..." << endl;
}

int main(void) {

//installation de la fonction de ramasse-miette
if ( atexit(terminaison) != 0 ) {
	cerr << "Impossible de faire fonctionner atexit" << endl;
	exit(EXIT_FAILURE);
}

//~ struct pollfd {
//~ int fd;		/* Descripteur de fichier */
//~ short events;	/* Événements attendus */
//~ short revents;	/* Événements détectés */
//~ };

//int poll(struct pollfd *fds, nfds_t nfds, int timeout);

buffer = new char [MAXDATASIZE];


	struct sockaddr_in sockserver;

	sockserver.sin_family = AF_INET;
	sockserver.sin_port = htons(2011);
	sockserver.sin_addr.s_addr = INADDR_ANY;
	bzero(&(sockserver.sin_zero), 8);

	//Creation d'un socket
	int sockserverfd;
	if ((sockserverfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {
		cerr << "Serveur : erreur creation socket()" << endl;
		exit(EXIT_FAILURE);
	}

	//pour pouvoir réutiliser la socket
	int options = 1;
	if ( setsockopt(sockserverfd, SOL_SOCKET, SO_REUSEADDR, (void *)&options, (socklen_t) sizeof(options)) == -1 ) {
		cerr << "Serveur : erreur setsockopts()" << endl;
		close(sockserverfd);
		exit(EXIT_FAILURE);
		}

	//pour mettre le socket en non-bloquant
	if ( ioctl(sockserverfd, FIONBIO, (char *)& options) == -1) {
		cerr << "Serveur : erreur ioctl()" << endl;
		close(sockserverfd);
		exit(EXIT_FAILURE);
	}

	//liaison (bind) avec le serveur
	if( bind(sockserverfd, (struct sockaddr*)& sockserver, sizeof(struct sockaddr)) == -1) {
		cerr << "Serveur : erreur bind()" << endl;
		close(sockserverfd);
		exit(EXIT_FAILURE);
	}

	//mise sur écoute
	if(listen(sockserverfd, BACKLOG) == -1) {
		cerr << "Serveur : erreur listen()" << endl;
		close(sockserverfd);
		exit(EXIT_FAILURE);
	}

	//la socket est créée, on peut répondre aux connexions
	unsigned int timeout;
	struct pollfd fds[200];
	nfds_t nfds = 1;

	memset(fds, 0 , sizeof(fds));

	fds[0].fd = sockserverfd;
	fds[0].events = POLLIN;
	timeout = (60 * 1000);	//en millisecondes, *1000 = secondes
	int pollreturn = 0;

	socklen_t sin_size;
	int  fd_temp;
	struct sockaddr_in client;
	sin_size = sizeof(struct sockaddr_in);

	bool connecte = false;

	while( connecte == false) {
		cerr << "poll en attente ..." << endl;

		if ( (pollreturn = poll(fds, nfds, timeout)) < 0 ) {
			cerr << "Serveur : erreur poll()" << endl;
			close(sockserverfd);
			exit(EXIT_FAILURE);
		}

		else if (pollreturn == 0) {
			cerr << "timeout de poll, aucune connexion (timeout = " << timeout/1000.0f  << " secondes)" << endl;
		}
		else { //une connexion s'est faite
			cout << "connexion" << endl;

			if ( fds[0].revents != POLLIN ) {
				cerr << "l'événement n'est pas POLLIN" << endl;
			}
			else if (fds[0].fd == sockserverfd) { //on peut accepter la connexion entrante

				//est-ce qu'on peut accepter cette connexion ?
				if ( (fd_temp = accept(sockserverfd, (struct sockaddr *)&client, &sin_size)) == -1 ) {
					cerr << "Serveur : erreur accept()" << endl;
					close(sockserverfd);
					exit(EXIT_FAILURE);
				}
				cout << "Serveur : connexion de " << inet_ntoa(client.sin_addr) << endl;
				//ajout dans le pool de clients
				fds[nfds].fd = fd_temp;
				fds[nfds].events = POLLIN;
				nfds++;
			}
			connecte = true;
		}
	}

	//on attend maintenant les messages
	string message;
	while ( message != "FIN" ) {
		cerr << "poll en attente de réception ..." << endl;

		if ( (pollreturn = poll(fds, nfds, timeout)) < 0 ) {
			cerr << "Serveur : erreur poll() réception" << endl;
			close(sockserverfd);
			exit(EXIT_FAILURE);
		}

		else if (pollreturn == 0) {
			cerr << "timeout de poll en réception, aucune connexion (timeout = " << timeout/1000.0f  << " secondes)" << endl;
		}

		else { //une connexion s'est faite
			cout << "connexion réception" << endl;
			//ce n'est pas une connexion entrante, mais des messages reçus
			memset(buffer, '\0', MAXDATASIZE);
			int nbrec = recv(fds[1].fd, buffer, sizeof(buffer), 0);
			//buffer[ nbrec ] = '\0';

			//on vérifie qu'on a bien reçu un message
			if ( (nbrec < 0) && ( errno != EWOULDBLOCK) ) {
				cerr << "Serveur : erreur recv()" << endl;
				close(sockserverfd);
				exit(EXIT_FAILURE);
				}

			else if ( nbrec == 0 ) {
				cerr << "Serveur : erreur connection du client fermée()" << endl;
				close(sockserverfd);
				exit(EXIT_FAILURE);
			}

			else { //traitement des données reçues
				cout << "Données recues : " << endl;
				message = string(buffer, strlen(buffer));

				cout << "longueur " << message.length() << "/" << nbrec << " / *" << message << "*" << endl;
			}
			}
	} //fin de traitement de la réception de message

for (unsigned int i=0; i < nfds ; i++) {
if (fds[i].fd >= 0)
	close (fds[i].fd);
}

close (sockserverfd);

return(EXIT_SUCCESS);
}
