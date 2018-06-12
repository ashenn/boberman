#include "../libs/libmy_extended.h"
#include "client.h"

void quit(char* buffer, int sd) {
	my_putstr("Execute Exit() function\n");
	
	my_putstr("Arguments : ");
	my_putstr(buffer);
	
	my_putstr("\nUserID : ");
	my_put_nbr(sd);
}

void joinChannel(char* buffer, int sd) {
	my_putstr("Execute JoinChannel() function\n");
	
	my_putstr("Arguments : ");
	my_putstr(buffer);

	my_putstr("\nUserID : ");
	my_put_nbr(sd);
}

void fetchClients(char* buffer, int sd) {
	my_putstr("hello");
	char *packet = malloc(1);
	packet[0] = 0x5;
	if ((send(sd, packet, 1, 0)) == -1) {
		my_putstr("Failure Fetching clients\n");
		close(sd);
	}
}