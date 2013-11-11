#include <string>
#include <iostream>

#include "creeping.h"
#include "debug_utils.h"

#define OPTION_ARG_NONE 0
#define OPTION_ARG_INT 1


struct OptionEntry {
	const char * 	name;
	const char *	short_name;
	int				with_arg;
	void *			variable;
	const char *	help;
};

std::string path_to_conf;
int o_timeout = -1;
bool o_slave = false;
bool o_once = false;

static struct OptionEntry entries[] = {
	{"--timeout", "-t", OPTION_ARG_INT, &o_timeout, "Quit after that time"},
	{"--slave", "-s", G_OPTION_ARG_NONE, &o_slave, "Read commands from stdin"},
	{"--once", "-o", G_OPTION_ARG_NONE, &o_once, "Don't loop run once"},
	{NULL}
};

bool check_path(char * path){
	struct stat st;
	if(stat(path,&st) == 0 && S_ISREG(st.st_mode) || !strcmp(path, "-"))
		return true;
    return false;
}

void parse_comand_line(int argc, char *argv[]){
	int i;
	for (i = 1; i < argc;i++)
	{
		struct OptionEntry * arg = entries;
		while (arg->name != NULL)
		{
			if (!strcmp(arg->name, argv[i]) ||  !strcmp(arg->short_name, argv[i]) )
			{
				if(arg->with_arg == OPTION_ARG_INT && i + 1 < argc)
					*(int *)arg->variable = atoi(argv[++i]);
				if(arg->with_arg == OPTION_ARG_NONE)
					*(bool *)arg->variable = true;
				break;
			}else if(check_path(argv[i])){
				path_to_conf.assign(argv[i]);
				break;
			}
			++arg;	
		}
	}
}

int
main(int argc, char *argv[])
{
	
	parse_comand_line(argc,argv);
	Creeping creeping;
	if(!path_to_conf.empty())
		creeping.open_conf(path_to_conf.c_str());
	if(o_once)
		creeping.play_once();

	nice(-10);
	
	
	creeping.start();
	
	if(o_timeout >= 0)
		sleep(o_timeout);
	else
		creeping.join();
	
	creeping.stop();

	
	DEBUG_PRINT_LINE;
	return 1;
}
