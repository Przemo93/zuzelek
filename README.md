

███████╗██╗   ██╗███████╗███████╗██╗     
╚══███╔╝██║   ██║╚══███╔╝██╔════╝██║     
  ███╔╝ ██║   ██║  ███╔╝ █████╗  ██║     
 ███╔╝  ██║   ██║ ███╔╝  ██╔══╝  ██║     
███████╗╚██████╔╝███████╗███████╗███████╗
╚══════╝ ╚═════╝ ╚══════╝╚══════╝╚══════╝

by Jakub Rózga & Przemysław Rydzyk


1. Opis zadania.
	
	15. Gra sieciowa Żużel dla 2 osób.
	Naszym zadaniem było stworzenie sieciowej gry dla dwóch graczy - Żużel. 
	Zadaniem gracza natomiast jest ukończenie 4 okrążeń po torze przy użyciu jedynie lewej strzałki.

2. Przygotowanie do gry.

	Przed pierwszym uruchomieniem należy zainstalować bibliotekę allegro w wersji 4 (liballegro4-dev).
	Następnie należy skompilować plik główny gry, używając do tego polecenia make.

3. Gra.
	
	Gracz, który uruchamia grę jako pierwszy powinien odpalić ją poleceniem "./zuzel 1" - stanie się wtedy serwerem.
	Drugi z graczy odpala grę poleceniem "./zuzel 2 [IP]" - w miejscu IP należy podać IP serwera.
	Po podłączeniu klienta do serwera gra się rozpoczyna - serwer rozpoczyna odliczanie, po którym następuje start.
	Od tej chwili zachodzi między nimi także wymiana informacji o aktualnej pozycji. 
	Jeżeli któryś z graczy wyjedzie poza tor lub przekroczy linię mety, wysyła odpowiedni sygnał i przestaje wysyłać swoją pozycję.
	Po odebraniu takiego sygnału drugi z graczy także przestaje wysyłać swoją pozycję.
	W zależności od zaistniałej sytuacji wyświetlana jest informacja o zwycięstwie lub porażce, i program jest zamykany.
