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

	clients = initListMgr();
	channels = initListMgr();

	server_t server_info = socket_init(inet_addr("127.0.0.1"), my_getnbr(opts->next->value));

	network_handling(server_info.fd, server_info.addr);

	close (server_info.fd);

	destroy_options(&opts);
	destroy_list(&args);
	destroy_logger(&logger);

	return 0;
}


