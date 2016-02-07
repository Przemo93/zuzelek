#include <allegro.h>

#include <string>
#include <stdio.h>
#include <math.h>
#include <sstream>

#include <iostream>
#include <list>

#include <cstdlib>
#include <strings.h>
#include <cstdio>
#include <unistd.h>
#include <netdb.h>
#include <cstdlib>
#include <cstring>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define PI 3.14159265

const int BUFSIZE = 1000;

const int SCREEN_WIDTH = 1100;
const int SCREEN_HEIGHT = 675;
const int SCREEN_BPP = 32;

const int promien=3;
double kierunekPoruszania = 0;
double predkosc = 2;
double katSkretu = 1;

int sck_in;
int oponentSock;

struct polozenie{
	double x;
	double y;
};

std::list<polozenie> polozeniaGracza;
std::list<polozenie> polozeniaPrzeciwnika;

// 2 funkcje ruchu
int zmianaKata(int kat, int bazowaZmianaKata)
{
	kat -=bazowaZmianaKata;
	if(kat<0) kat=+360;
	return kat;
}

bool czyOkrazenieSkonczone(int x, int y, bool updown)
{
	bool ans;
	if(x>299 && x<321){
		if(y>24 && y<96 && updown == true) ans = true;
		else if(y>206 && y<275 && updown == false) ans = true;
	}
	else ans = false;
	return ans;
}


int server(){
    struct sockaddr_in sck_addr_in;
    memset(&sck_addr_in, 0, sizeof sck_addr_in);
    sck_addr_in.sin_family = AF_INET;
    sck_addr_in.sin_port = htons(6667);
    sck_addr_in.sin_addr.s_addr = htonl(INADDR_ANY); // "5.135.147.79"
    
    sck_in = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sck_in < 0){
        perror ("Can't create a socket");
        exit(EXIT_FAILURE);
    }
    
    int nFoo = 1;
    setsockopt(sck_in, SOL_SOCKET, SO_REUSEADDR, (char*)&nFoo, sizeof(nFoo));
    
    if(bind(sck_in, (struct sockaddr*)&sck_addr_in, sizeof(struct sockaddr)) < 0){
        perror("Can't bind a name to a socket");
        exit(EXIT_FAILURE);
    }
    
    if(listen(sck_in, 5) < 0){
        perror("Can't set queue size");
        exit(EXIT_FAILURE);
    }
    
    socklen_t slen = sizeof(sck_addr_in);
    
    printf("Czekam na klienta\n");
    int clientSocket = accept(sck_in, (struct sockaddr*)&sck_addr_in, &slen);
    
    if(clientSocket < 0){
        perror("Can't create a connetion socket");
        exit(EXIT_FAILURE);
    }
    oponentSock = clientSocket;
    
    return clientSocket;
}

int client(char* ip){
    
    char *server = ip;	/* adres IP pętli zwrotnej */
    char const *protocol = "tcp";
    short service_port = 6667;	/* port usługi daytime */
    
    struct sockaddr_in sck_addr;
    int sck;
    
    printf ("Usługa %d na %s z serwera %s :\n", service_port, protocol, server);
    
    memset (&sck_addr, 0, sizeof sck_addr);
    sck_addr.sin_family = AF_INET;
    inet_aton (server, &sck_addr.sin_addr);
    sck_addr.sin_port = htons (service_port);
    
    if ((sck = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror ("Nie można utworzyć gniazdka");
        exit (EXIT_FAILURE);
    }
    
    if (connect (sck, (struct sockaddr*) &sck_addr, sizeof sck_addr) < 0) {
        perror ("Brak połączenia");
        exit (EXIT_FAILURE);
    }
    
    return sck;
}


int main( int argc, char** argv ){
	
	//przygotowanie polaczenia
	int clientSocket;
	int sck; 
    if(argv[1][0] == '1'){
		clientSocket = server();
		printf("Polaczono !\n");
    }
	else{
		sck = client(argv[2]);
		printf("Polaczono z serwerem ! \n");
	}
	
	//inicjowanie funkcji zwiazanych z grafika
	BITMAP* buffer = NULL;
    allegro_init();
	install_keyboard();
	set_color_depth(16);
    set_gfx_mode( GFX_AUTODETECT_WINDOWED, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0 );
	clear_to_color(screen, makecol( 0, 0, 0 ) );
	buffer = create_bitmap(SCREEN_WIDTH, SCREEN_HEIGHT);
    
	BITMAP *okrazenie1 = NULL, *okrazenie2 = NULL, *okrazenie3 = NULL, *okrazenie4 = NULL;	
	BITMAP *odliczanie3 = NULL, *odliczanie2 = NULL, *odliczanie1 = NULL, *odliczanieStart = NULL, *wygrana = NULL, *przegrana = NULL;
	
	polozenie player, player_temp;
	
	int licznikOkrazen=0;
	double x,y;
	bool zakonczenie = false;
	bool kolizja = false;
	bool lap_updown= false;
	bool zwyciestwo = true;
	
    //zaladowanie bitmap
	odliczanie3 = load_bmp("odliczanie3.bmp", NULL);
	odliczanie2 = load_bmp("odliczanie2.bmp", NULL);
	odliczanie1= load_bmp("odliczanie1.bmp", NULL);
	odliczanieStart = load_bmp("odliczanieStart.bmp", NULL);
	okrazenie1 = load_bmp("okrazenie1.bmp", NULL);
	okrazenie2 = load_bmp("okrazenie2.bmp", NULL);
	okrazenie3 = load_bmp("okrazenie3.bmp", NULL);
	okrazenie4 = load_bmp("okrazenie4.bmp", NULL);
	przegrana = load_bmp("przegrana.bmp", NULL);
	wygrana = load_bmp("Wygrana.bmp", NULL);
        
	//ustalenie pozycji poczatkowej
    if(argv[1][0] == '1'){
        player.x = 535;
        player.y = 480;
	}
    else{
        player.x = 535;
        player.y = 510;
    }
	int z = 4;
	
	//inicjalizacja wyscigu
	while(z>0){
		if(z == 4) draw_sprite(buffer, odliczanie3, 0, 0);
		if(z == 3) draw_sprite(buffer, odliczanie2, 0, 0);
		if(z == 2) draw_sprite(buffer, odliczanie1, 0, 0);
		if(z == 1){
			draw_sprite(buffer, odliczanieStart, 0, 0);
			circlefill(buffer, (int)player.x,(int)player.y, promien, makecol(128,0,0)); 
			if(player.y == 480) circlefill(buffer, 535, 510, promien, makecol(0,128,0));
			else circlefill(buffer, 535, 480, promien, makecol(0, 128, 0));
		}
		draw_sprite(screen,buffer,0,0);
		sleep(1);
		z = z - 1;
	}	
    
	while(zakonczenie == false && licznikOkrazen != 9){
		// zakonczenie wyscigu jesli wcisniety zostal ESC
		if(key[ KEY_ESC ]){	
		    printf("Gracz zamknal gre poprzez ESC. \n");
		    zakonczenie = true;
        }
	
		// wyswietlenie odpowiedniego tla planszy
        if(licznikOkrazen < 3) draw_sprite(buffer, okrazenie1, 0, 0);     
        else if(licznikOkrazen < 5) draw_sprite(buffer, okrazenie2, 0, 0);
        else if(licznikOkrazen < 7) draw_sprite(buffer, okrazenie3, 0, 0);      
        else if(licznikOkrazen < 9) draw_sprite(buffer, okrazenie4, 0, 0);
		
		// jesli nie ma kolizji...
		if(kolizja == false){
			
			//zmieniamy predkosc i kat w zaleznosci od tego, czy wcisniety jest klawisz
			if( key[ KEY_LEFT ] ){
	    		predkosc-=0.05;
	    		if(predkosc<1.5) predkosc=1.5;
	    		katSkretu+=0.05;
	    		if(katSkretu>1.5) katSkretu=1.5;
            	kierunekPoruszania = zmianaKata(kierunekPoruszania,katSkretu);
	  		}
	  		else{
	    		predkosc+=0.05;
	    		if(predkosc>2) predkosc=2;
	    		katSkretu-=0.05;
	    		if(katSkretu<1) katSkretu=1; 
	  		}
			
	  		//i wyznaczamy nowa pozycje
	  		x = predkosc*cos(kierunekPoruszania*PI/180);
	  		player.x+=x;
	  		y = predkosc*sin(kierunekPoruszania*PI/180);
	  		player.y+=y;
			
			//sprawdzamy, czy okrazenie zostalo ukonczone przez gracza
	  		if(czyOkrazenieSkonczone((int)player.x,(int)player.y,lap_updown)==true){
	      			licznikOkrazen++;
	      			lap_updown = (!lap_updown);
	  		}
			
			//sprawdzamy, czy gracz nie znalazl sie poza torem
			if(getpixel(buffer, (int)player.x, (int)player.y) == makecol(0,0,0)) kolizja = true;
			circlefill(buffer, (int)player.x, (int)player.y, promien, makecol(128,0,0));
			polozeniaGracza.push_front(player); 
			while(polozeniaGracza.size()>50) polozeniaGracza.pop_back();
		}
		
		//rysowanie sladu za graczem
		for(unsigned int i = 0; i<polozeniaGracza.size();i++){
	  		player_temp = polozeniaGracza.front();
	  		polozeniaGracza.pop_front();
	  		circlefill(buffer, (int)player_temp.x, (int)player_temp.y, promien, makecol(128,0,0)); 
	  		polozeniaGracza.push_back(player_temp);	  
		}
		
		
		
	 	//klient - komunikacja: przygotowanie i przeslanie informacji o kolizji/koncu wyscigu
        if(argv[1][0] == '2'){
            if(kolizja == true){
				player.x = 1;
				player.y = 1;
			}
			else if(licznikOkrazen == 9){
				player.x = 2;
				player.y = 2;
			}
		  
			char x[sizeof(player.x)];
			char y[sizeof(player.y)];
				
			sprintf(x, "%lf", player.x);
			sprintf(y, "%lf", player.y);
				
			write(sck,x,8);
			write(sck,y,8);
			oponentSock = sck;

			char rcvx[sizeof(double)];
			char rcvy[sizeof(double)];

			polozenie rcv;

			read(sck, &rcvx, 8);
			rcv.x = atof(rcvx);
			read(sck, &rcvy, 8);
			rcv.y = atof(rcvy);
			
			if(rcv.x == 1 && rcv.y == 1){	
				zakonczenie = true;
				zwyciestwo = true;
			}
			else if (rcv.x == 2 && rcv.y == 2) kolizja = true; 
			else polozeniaPrzeciwnika.push_front(rcv);
		}
		
		
		
		// to samo dla serwera
		else{
			char rcvx[sizeof(double)];
            char rcvy[sizeof(double)];
            
            polozenie rcv;
            
            read(clientSocket, &rcvx, 8);
            rcv.x = atof(rcvx);
            read(clientSocket, &rcvy, 8);
            rcv.y = atof(rcvy);
            		
            if(rcv.x == 1 && rcv.y == 1) {
				zakonczenie = true;
				zwyciestwo = true;
			}
			else if (rcv.x == 2 && rcv.y == 2) kolizja = true; 
            else polozeniaPrzeciwnika.push_front(rcv);
			
	    	if(kolizja == true){
			    player.x = 1;
			    player.y = 1;
			}
			else if(licznikOkrazen == 9){
			    player.x = 2;
			    player.y = 2;
			}
	    		
			char x[sizeof(player.x)];
            char y[sizeof(player.y)];
			 
            sprintf(x ,"%lf",player.x);
            sprintf(y ,"%lf",player.y);

            write(clientSocket,x,8);
            write(clientSocket,y,8);
       
 		}
	
		// zredukowanie sladu przeciwnika
	 	while(polozeniaPrzeciwnika.size()>50){
			polozeniaPrzeciwnika.pop_back();
		}
	
		// rysowanie sladu przeciwnika
	 	for(unsigned int i = 0; i<polozeniaPrzeciwnika.size();i++){
	  		player_temp = polozeniaPrzeciwnika.front();
	  		polozeniaPrzeciwnika.pop_front();
	  		circlefill(buffer, (int)player_temp.x, (int)player_temp.y, promien, makecol(0,128,0)); 
	  		polozeniaPrzeciwnika.push_back(player_temp);	  
		}
	
		draw_sprite(screen, buffer, 0, 0);

		if(kolizja == true){
			zakonczenie = true;
			zwyciestwo = false;
		}		
	}
	
	//gdy wygrana wyswietlamy wygrana	
	if(zwyciestwo == true){
		printf("wygrana\n");
		draw_sprite(screen, wygrana, 0, 0);
		sleep(2);
	}
	
	//gdy przegrana wyswietlamy przegrana
	else{
		draw_sprite(screen, przegrana, 0, 0);
		sleep(2);
	}
	
	//zamykanie gniazd
	if(argv[1][0] == '1'){
        close(clientSocket);
        close(sck_in);
    }
	
	//czyszczenie listy polozen graczy
	while (!polozeniaGracza.empty()){
	    polozeniaGracza.pop_back();
	}
	while (!polozeniaPrzeciwnika.empty()){
	    polozeniaPrzeciwnika.pop_back();
	}

	//czyszczenie pamieci zajetej przez bitmapy
	destroy_bitmap(odliczanieStart);
	destroy_bitmap(odliczanie3);
	destroy_bitmap(odliczanie2);
	destroy_bitmap(odliczanie1);
	destroy_bitmap(okrazenie1);
	destroy_bitmap(okrazenie2);
	destroy_bitmap(okrazenie3);
	destroy_bitmap(okrazenie4);
	destroy_bitmap(wygrana);
	destroy_bitmap(przegrana);
	destroy_bitmap(buffer);
	allegro_exit();

	return 0;
}
END_OF_MAIN();

    
