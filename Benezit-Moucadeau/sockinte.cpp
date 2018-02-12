
#include "cli_req.hpp"
#include "serv_req.hpp"
// pour les thread
#include <thread>

void serv(serveur * monserveur,  int * fd_client, client * monclient){
//Boucle permettant la reception est l'envoi de plusieurs message
  char* buff = new char[MAXDATASIZE];
	while(strcmp(buff, "quit")){
		monserveur->reception(*fd_client, buff);
		cout << *fd_client << " : " << buff << "\n";
        monclient->emission(buff);
	}
		monserveur->deconnexion(*fd_client);
}

void cli(client * monclient, serveur * monserveur,  int * fd_client){
//Boucle permettant la reception est l'envoi de plusieurs message
    char* buff = new char[MAXDATASIZE];
    while(strcmp(buff, "quit")){
	    monclient -> reception(buff);
//Condition pour pouvoir verifier la reception du serveur inte et de conserver quit pour le bon fonctionnnment de l'appli
        if(strcmp(buff,"quit")){
	        monserveur -> emission(*fd_client, strcat(buff," Lu-inte"));
        }else{
            monserveur -> emission(*fd_client, buff);
        }

    }
}

int main(int argc, char *argv[])
{
  //creation du client
  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " <Addresse IP serveur> [<port>]" << endl;
    exit(-1);
  }
  client * monclient;
	if (argc == 3) {
	  unsigned int port = strtol(argv[2], (char **) NULL, 10);
	  monclient = new client (argv[1], port);
	}else{
	    monclient = new client (argv[1], 3350);
	}
  monclient -> connexion();

  //Creation du serveur
  serveur monserveur (2002);
  int fd_client;

    fd_client = monserveur.attente();
//mise en place des threads permettant d'utiliser le client et serveur en meme temps
	thread One (serv, &monserveur, &fd_client, monclient);
	thread Two (cli, monclient, &monserveur, &fd_client);
	One.join();
    Two.join();
	return EXIT_SUCCESS;
}
