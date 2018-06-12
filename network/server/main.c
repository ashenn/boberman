#include "../libs/libmy_extended.h"
#include "./server.h"
#include "server.h"

List commands = NULL;
List clients = NULL;
List channels = NULL;
Options opts = NULL;
Logger logger = NULL;

int main (int argc, char *argv[])
{
	opts = init_parser("null");
	List args = get_args(argc, argv);
	logger = config_logger("./journal.log", 0, 1, 1);

	add_option(&opts, "-p", 1, "8888", my_putstr); //port
	add_option(&opts, "-c", 1, "25", my_putstr);  //max clients
	parser(&opts, args);

    init_commands ();

	// ---------
	client_t *bot = malloc(sizeof(client_t));
	bot->fd = -1;
	bot->id = -1;
	bot->name = NULL;

	clients = create_list(bot);

	channel_t *default_channel = malloc(sizeof(channel_t));
	default_channel->id = 0x00;
	default_channel->clients = create_list(bot);
	default_channel->name = "General";

	channels = create_list(default_channel);
	// ---------

	server_t server_info = socket_init(inet_addr("127.0.0.1"), my_getnbr(opts->next->value));

	network_handling(server_info.fd, server_info.addr);

	close (server_info.fd);

	destroy_options(&opts);
	destroy_list(&args);
	destroy_logger(&logger);

	return 0;
}


