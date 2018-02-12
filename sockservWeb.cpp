//R. Raffin IUT Aix-Marseille, dépt. Informatique, Arles
//Code Serveur pour les Sockets
//attend les clients (en boucle) sur le port 8000. pour la première connexion envoie une page d'accueil avec un lien. Si on a clique sur le lien, on affiche une autre page. Il faut vérifier si le lien (/liaison) est dans le GET HTTP. Permet d'expliquer le fonctionnement d'un serveur Web.
//pour bien faire (!), il faudrait lire le contenu des pages dans un fichier (.html?)
//Compilation : g++ -Wall sockservWeb.cpp -o serveur_web

#include <iostream>

#include <cstdlib>
#include <unistd.h>
#include <cstring>

//pour les sockets
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BACKLOG 2
#define MAXDATASIZE 100

using namespace std;

class serveur {
	private:
		int fd;
		struct sockaddr_in sockserver;
	public:
		serveur(int);
		~serveur(void);
		int attente(void);
		int emission(const int, const char *);
		int reception(const int, char *);
		void deconnexion(const int);
};

serveur::serveur(int _p=8000) {

	sockserver.sin_family = AF_INET;
	sockserver.sin_port = htons(_p);
	sockserver.sin_addr.s_addr = INADDR_ANY;
	bzero(&(sockserver.sin_zero),8);

	//Creation d'un socket
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {
		cerr << "ServeurWeb : erreur creation socket()" << endl;
		exit(-1);
	}

	//liaison (bind) avec le serveur
	if(bind(fd,(struct sockaddr*)&sockserver,sizeof(struct sockaddr)) == -1) {
		cerr << "ServeurWeb : erreur bind()" << endl;
		exit(-1);
	}

	//mise sur écoute
	if(listen(fd,BACKLOG) == -1){
		cerr << "ServeurWeb : erreur listen()" << endl;
		exit(-1);
	}

}

int serveur::reception(const int _fd, char * _buf) {
int numbytes;

	if ((numbytes=recv(_fd,_buf,MAXDATASIZE,0)) == -1) {
		cerr << "ServeurWeb : erreur recv()" << endl;
		exit(-1);
		}
	_buf[numbytes]='\0';

return numbytes;
}

int serveur::attente(void) {
socklen_t sin_size;
int  fd_temp;
struct sockaddr_in client;


sin_size=sizeof(struct sockaddr_in);

	if ((fd_temp = accept(fd,(struct sockaddr *)&client,&sin_size))==-1) {
		cerr << "ServeurWeb : erreur accept()" << endl;
		exit(-1);
	}
	cout << "ServeurWeb : connexion de " << inet_ntoa(client.sin_addr) << endl;
return fd_temp;
}

void serveur::deconnexion(const int _fd) {
	close(_fd);
}

int serveur::emission(const int _fd, const char* _s) {
int r;

	r = send(_fd, (void *) _s, (size_t) strlen(_s), 0);
	return r;
}


serveur::~serveur(void) {
	close(fd);
}

int main(void)
{
int fd_client;
char message[MAXDATASIZE];

serveur monserveur(8000);

	while (true) {
		//juste pour pré-remplir le message = ce n'est pas beau !
		strcpy(message,"rien");

		fd_client = monserveur.attente();
		monserveur.reception(fd_client, message);

		cout << "---------------------" << endl << "\tMessage du client : " << "\t" << message << endl << "---------------------" << endl;

		if (strstr(message,"/liaison")!=NULL) {//nelle page si lien clique

		monserveur.emission(fd_client,"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n<html>\n<head>\n");
		monserveur.emission(fd_client,"<title>2eme page</title>\n");
		monserveur.emission(fd_client,"</head>\n");
		monserveur.emission(fd_client,"<body>\n");

		monserveur.emission(fd_client,"Lien clique\n");
		monserveur.emission(fd_client,"</body>\n");
		monserveur.emission(fd_client,"</html>\n");

		}

		else {//page par defaut

		monserveur.emission(fd_client,"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\"><html><head>\n");
		monserveur.emission(fd_client,"<title>page simple</title>\n");
		monserveur.emission(fd_client,"</head>");
		monserveur.emission(fd_client,"<body>");

		monserveur.emission(fd_client,"Salut ca va ?\n");
		monserveur.emission(fd_client,"<br><a href=liaison>lien</a>\n");

		monserveur.emission(fd_client,"</body>");
		monserveur.emission(fd_client,"</html>");

		}

		monserveur.deconnexion(fd_client); //!! comme la seule façon d'arrêter le serveur est CTRL-C, il faudrait attraper ce signal (cf TP Signaux) pour pouvoir désallouer correctement le port. Sinon on risque de ne pas pouvoir s'en resservir (le temps que le système se rende compte que ce port ne sert plus a aucun processus)
	}

return EXIT_SUCCESS;
}
