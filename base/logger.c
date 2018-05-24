#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>

#include "basic.h"
#include "logger.h"

void log2file(char* lvl, char* msg){
	Log* logger = getLogger();

	time_t current_time;
	struct tm * time_info;		
	char timeString[20];

	time(&current_time);
	time_info = localtime(&current_time);

	strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", time_info);

	write(logger->f, "[", 1);
	write(logger->f, timeString, 19);
	write(logger->f, "] ", 2);
	write(logger->f, lvl, strlen(lvl));
	write(logger->f, ":\n", 2);
	write(logger->f, msg, strlen(msg));
	write(logger->f, "\n\n\n", 3);
}

void logg(short lvl, char* msg, va_list* args){
	Log* logger = getLogger();
	char* l = logger->lvls[lvl];

	char message[10000];
	vsprintf(message, msg, *args);

	if (lvl >= logger->lvl){
		fprintf(stdout, "%s[%s] ", logger->lvlColors[lvl], l);
		printf("%s", message);
		fprintf(stdout, "%s\n\n\n", KNRM);

		if (logger->f > 0)	{
			log2file(l, message);
		}
	}

}


short setLogFile(Log* logger, char* file){
	logger->dbg("change log file to: ");
	logger->dbg(file);
	logger->file = file;

	logger->f = open(logger->file, O_CREAT|O_WRONLY|O_APPEND, S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP);
	if (logger->f > 0)
	{
		return 1;
	}

	int filen = strlen(logger->file);

	char m[] = "Fail to set file";
	
	int mlen = strlen(m);

	char msg[filen+mlen];
	concat(m, logger->file, msg);

	logger->err(msg);
	return 0;
}

void* logDebug(char* msg, ...){
	va_list args;
	va_start(args, msg);
	logg(DEBUG, msg, &args);
	va_end(args);
}

void* logInfo(char* msg, ...){
	va_list args;
	va_start(args, msg);
	logg(INFO, msg, &args);
	va_end(args);
}

void* logWarning(char* msg, ...){
	va_list args;
	va_start(args, msg);
	logg(WARNING, msg, &args);
	va_end(args);
}

void* logError(char* msg, ...){
	va_list args;
	va_start(args, msg);
	logg(ERROR, msg, &args);
	va_end(args);
}

void setLoggerFuncs(Log* logger){
	logger->dbg = logDebug;
	logger->inf = logInfo;
	logger->war = logWarning;
	logger->err = logError;
	logger->close = closeLogger;
}

void setLoggerLvls(Log* logger){
	logger->lvls = malloc(sizeof(char*) * 5);

	logger->lvls[0] = "DEBUG\0";
	logger->lvls[1] = "INFO\0";
	logger->lvls[2] = "WARNING\0";
	logger->lvls[3] = "ERROR\0";
	logger->lvls[4] = '\0';
}

void setLoggerLvlColors(Log* logger){
	logger->lvlColors = malloc(sizeof(char*) * 5);
	
	logger->lvlColors[0] = KBLU;
	logger->lvlColors[1] = KNRM;
	logger->lvlColors[2] = KYEL;
	logger->lvlColors[3] = KRED;
	logger->lvlColors[4] = '\0';
}

void* setLogLvl(int lvl){
	Log* logger = getLogger();
	if (lvl < 0 || lvl > 4)
	{
		logger->lvl = 2;
		logger->war("Bad Log lvl(%d) \nLvl is set to WARNING", lvl);
		return NULL;
	}

	logger->lvl = lvl;

	return NULL;
}

void parseLogArgs(){
	Log* logger = getLogger();

	static Arg arg1 = {
		.name = "-logLvl", 
		.function = setLogLvl, 
		.hasParam = 1, 
		.defParam = NULL, 
		.asInt = 1, 
		.type="num"
	};

	static Arg arg2 = {
		.name = "-logPath", 
		.function = (void*)setLogFile, 
		.hasParam = 1, 
		.defParam = NULL, 
		.asInt = 0, 
		.type="any"
	};

	arg2.defParam = (void*)logger;

	static  Arg* args[] = {
		&arg1,
		&arg2,
		NULL
	};

	logger->args = malloc(sizeof(ListManager));
	logger->args = defineArgs(args);
}

Log* initLogger(int argc, char *argv[]){
	static Log* logger = NULL;
	if (logger != NULL){
		return logger;
	}

	logger = malloc(sizeof(Log));
	if (logger == NULL){
		return NULL;
	}

	logger->f = 0;
	logger->lvl = -1;

	setLoggerLvls(logger);
	setLoggerLvlColors(logger);
	setLoggerFuncs(logger);

	logger->isInit = 1;

	parseLogArgs();
	parseArgs(logger->args, argc, argv);

	if (!logger->f)
	{
		setLogFile(logger, "./journal.log");
	}

	if (logger->lvl < 0)
	{
		logger->lvl = 2;
		logger->inf("No Log lvl: Lvl is set to WARNING");
	}

	return logger;
}

void closeLogFile(){
	Log*  logger = getLogger();
	
	close(logger->f);
	logger->f = 0;
}

void clearLoglvls(){
	int i;
	Log*  logger = getLogger();
	
	free(logger->lvls);
}

void clearLoglvlColors(){
	int i;
	Log*  logger = getLogger();

	free(logger->lvlColors);
}

void* closeLogger(){
	Log*  logger = getLogger();
	logger->inf("Closing Logger");
	closeLogFile();
	clearLoglvls();
	clearLoglvlColors();

	clearList(logger->args);
	free(logger->args);
	free(logger);
}

Log* getLogger(){
	return initLogger(0, NULL);
}