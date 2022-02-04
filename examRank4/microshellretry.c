#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

typedef struct s_tokens{
	char **cmd;
	int fd[2];
	int cmd_size;
	struct s_tokens *next;
}	t_tokens;

int ft_strlen(char *str)
{
	int i = 0;
	if(!str)
		return i;
	while(str[i])
	{
		i++;
	}
	return i;
}

void exit_fatal()
{
	write(STDERR, "error: fatal\n", ft_strlen("error: fatal\n"));
	exit(EXIT_FAILURE);
}

char *ft_strdub(char *str)
{
	char *temp;
	int len = ft_strlen(str);
	temp = (char *)malloc((len + 1) * sizeof(char));
	if(!temp)
		exit_fatal();
	temp[len] = 0;
	for(int j = 0; j < len; j++)
		temp[j] = str[j];
	// printf("%s\n", temp);
	return temp;
}

void init_token(t_tokens *new)
{
	new->next = NULL;
	new->fd[0] = 0;
	new->fd[1] = 1;
}

void print_tokens(t_tokens *tokens)
{
	printf("check\n");
	while(tokens)
	{
		printf("cammand: \n");
		for(int i = 0; i < tokens->cmd_size; i++)
			printf("%s\n", tokens->cmd[i]);
		tokens = tokens->next;
	}
}

int get_cmd(t_tokens *new, char **argv)
{
	int size = 0;
	while(argv[size] && strcmp(argv[size], "|") && strcmp(argv[size], ";"))
		size++;
	new->cmd_size = size;
	new->cmd = (char **)malloc((size+1) * sizeof(char *));
	if(!new->cmd)
		exit_fatal();
	new->cmd[size] = NULL;
	for(int i = 0; i < size; i++)
		new->cmd[i] = ft_strdub(argv[i]);
	printf("size is %d\n", size);
	return size;
}

void ft_lstAddBack(t_tokens **tokens, t_tokens *new)
{
	if(!(*tokens))
		*tokens = new;
	t_tokens *last;
	last = *tokens;
	while(last->next)
		last = last->next;
	last->next = new;
}

int parse_argu(t_tokens **tokens, char **argv)
{
	int i = 0;
	t_tokens *new;
	while(argv[i] != NULL && strcmp(argv[i], ";") != 0)
	{
		if(strcmp(argv[i], "|") == 0)
		{
			i++;
			continue;
		}
		new = (t_tokens *)malloc(sizeof(t_tokens));
		if(!new)
			exit_fatal();
		init_token(new);
		i += get_cmd(new, &argv[i]);
		printf("i is %d argv is %s\n", i, argv[i]);
		ft_lstAddBack(tokens, new);
		// print_tokens(*tokens);
	}
	printf("return i is %d \n", i);

	return i;
}

int main(int argc, char **argv, char **env)
{
	int i = 1;
	t_tokens *tokens;
	tokens = NULL;
	if(argc > 1)
	{
		// while(argv[i])
		while(i < argc)
		{
			if(!strcmp(argv[i], ";"))
			{
				i++;
				tokens = NULL;
				continue;
			}
			i += parse_argu(&tokens, &argv[i]);
			printf("after parsing i is %d \n", i);

			print_tokens(tokens);
		}
	}
	return 0;
}