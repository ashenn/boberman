/*
** libmy_extended.h for libmy_extended in /home/lalos_b/Documents/TIC-DVC3/lalos_b
** 
** Made by LALOS Bastien
** Login   <lalos_b@etna-alternance.net>
** 
** Started on  Sun Jan 15 18:49:34 2017 LALOS Bastien
** Last update Fri Feb 17 15:50:35 2017 LALOS Bastien
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

struct node
{
  void *value;
  int index;
  int length;
  struct node* next;
  struct node* prev;
};
typedef struct	node Node;
typedef struct	node* List;

struct option
{
  int	index;
  int	length;
  int	has_value;
  void *default_value;
  void *name;
  void *value;
  void (*ptr_function)(char*);
  struct option* next;
  struct option* prev;
};
typedef struct	option Option;
typedef struct	option* Options;

struct logger
{
  int max_level;
  int file;
  int w_terminal;
  int w_file;
  void	(*debug)(struct logger* logger, char* msg);
  void	(*info)(struct logger* logger, char* msg);
  void	(*warning)(struct logger* logger, char* msg);
  void	(*error)(struct logger* logger, char* msg);
};
//typedef struct logger Log;
typedef struct logger* Logger;

/* fonctions liblog */
extern Logger	config_logger(char* path, int max_level, int w_terminal, int w_file);
extern void	debug(Logger logger, char* msg);
extern void	info(Logger logger, char* msg);
extern void	warning(Logger logger, char* msg);
extern void	error(Logger logger, char* msg);
extern void	show_time();
extern void	my_putstr_color(char* msg, char* color);
extern void	write_in_terminal(char* level, char* color, char* msg);
extern void	write_in_file(int f, char* msg, char* level);
extern void	clear_file(char* path);
extern void	destroy_logger(Logger *logger);

/* fonctions libparse */
extern Options	init_parser(void *name);
extern int	check_args(int nb_args, int nb);
extern List	get_args(int nb_args, char** args);
extern int	add_option(Options *list, void *name, int has_value, char* default_value, void *function);
extern int	del_option(Options *list);
extern int	destroy_options(Options *list);
extern int	parser(Options *opt, List args);

/* fonctions liblist */
extern List	create_list(void* value);
extern int	destroy_list(List *list);
extern int	add_node(List *list, void* value);
extern int	del_first_node(List *list);
extern int	del_last_node(List *list);
extern int	del_node(List *list, int index);
extern Node	search_node_by_index(List list, int index);
extern Node	search_node_by_value(List list, void* value);

/* fonctions libmy */
extern void	my_aff_tab(int tab[], int len);
extern int	my_getnbr(char *str);
extern int	my_isneg(int n);
extern void	my_put_nbr(int nbr);
extern void	my_putchar(char c);
extern void	my_putnbr_base(int nbr, char *base);
extern void	my_putstr(char *str);
extern char	**my_str_to_wordtab(char *str);
extern char	*my_strcat(char *str1, char *str2);
extern int	my_strcmp(char *s1, char *s2);
extern char	*my_strcpy(char *dest, char *src);
extern char	*my_strdup(char *str);
extern int	my_strlen(char *str);
extern char	*my_strncat(char *str1, char *str2, int n);
extern int	my_strncmp(char *s1, char *s2);
extern char	*my_strncpy(char *dest, char *src, int n);
extern char	*my_strstr(char *str, char *to_find);
extern char	my_swap(int *a, int*b);
