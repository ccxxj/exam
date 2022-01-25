#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

typedef struct s_token{
	char **cmd;
	int fd[2];
	int size;
	struct s_token *next;
} t_token;


int ft_strlen(char *str)
{
	int i = 0;
	if(!str)
		return 0;
	while(str[i])
		i++;
	return i;
}

void exit_fatal()
{
	write(STDERR, "error: fatal\n", ft_strlen("error: fatal\n"));
	exit(EXIT_FAILURE);
}

void error_msg(char *str)
{
	write(STDERR, str, ft_strlen(str));
	exit(EXIT_FAILURE);
}

void error_msgs(char *str1, char *str2)
{
	write(STDERR, str1, ft_strlen(str1));
	write(STDERR, str2, ft_strlen(str2));
	write(STDERR, "\n", 1);
	exit(EXIT_FAILURE);
}

char *ft_strdup(char *str)
{
	char *temp;
	if(!str)
		return NULL;
	int len = ft_strlen(str);
	temp = (char *)malloc((len + 1) * sizeof(char));
	if(!temp)
		exit_fatal();
	temp[len] = '\0';
	int i = 0;
	while(str[i])
	{
		temp[i] = str[i];
		i++;
	}
	return temp;
}

void init_token(t_token *new)
{
	new->next = NULL;
	new->fd[0] = 0;
	new->fd[1] = 1;
}

void list_addback(t_token **token, t_token *new)
{
	if(!(*token))
		*token = new;
	else{
		while((*token)->next)
		{
			(*token) = (*token)->next;
		}
		(*token)->next = new;
	}
}

int get_cmd(t_token *new, char **argv)
{
	int i = 0;
	while(argv[i] && strcmp(argv[i], ";") && strcmp(argv[i], "|"))
		i++;
	new->size = i;
	new->cmd = (char **)malloc((i + 1)*sizeof(char*));
	new->cmd[i] = NULL;
	if(!new->cmd)
		exit_fatal();
	int j = 0;
	while(j < i)
	{
		new->cmd[j] = ft_strdup(argv[j]);
		j++;
	}
	return j;
}

int parse_argu(t_token **token, char **argv)
{
	int i = 0;
	t_token *new;
	while(argv[i] && strcmp(argv[i], ";"))
	{
		if(!strcmp(argv[i], "|"))
		{
			i++;
			continue;
		}
		new = (t_token *)malloc(sizeof(t_token));
		if(!new)
			exit_fatal();
		init_token(new);
		i += get_cmd(new, &(argv[i]));
		list_addback(token, new);

	}
	return i;
}

void print_token(t_token *token)
{
	while(token)
	{
		int i = 0;
		while(i < token->size)
		{
			printf("%s\n", token->cmd[i]);
			i++;
		}
		printf("finished 1 token\n");
		token = token->next;
	}
}

void free_all(t_token *token)
{
	t_token *temp;
	while(token)
	{
		int i = 0;
		while(i < token->size)
		{
			free(token->cmd[i]);
			i++;
		}
		free(token->cmd);
		temp = token;
		token = token->next;
		free(temp);
		temp = NULL;
	}
}

void exec_pipes(t_token *token, char **env)
{
	int status;
	if(token->next)
	{
		int fd[2];
		if(pipe(fd) == -1)
			exit_fatal();
		token->fd[1] = fd[1];
		token->next->fd[0] = fd[0];
	}
	int pid = fork();
	if(pid < 0)
		exit_fatal();
	if(pid == 0)
	{
		if(token->fd[0]!= STDIN)
		{
			if(dup2(token->fd[0], STDIN) < 0)
				exit_fatal();
		}
		if(token->fd[1]!= STDOUT)
		{
			if(dup2(token->fd[1], STDOUT) < 0)
				exit_fatal();
		}
		if(execve(token->cmd[0], token->cmd, env) < 0)
			error_msgs("error: cannot execute ", token->cmd[0]);
	}
	if(pid > 0)
	{
		waitpid(pid, &status, 0);
		if(token->fd[0]!= STDIN)
			close(token->fd[0]);
		if(token->fd[1]!= STDOUT)
			close(token->fd[1]);
	}
}

void exec_cmds(t_token *tokens, char **env)
{
	while(tokens)
	{
		if(!strcmp(tokens->cmd[0], "cd"))
		{
			if(tokens->size != 2)
				error_msg("error: bad arguments\n");
			if(chdir(tokens->cmd[1]) < 0)
				error_msgs("error: cd: cannot change directory to ", tokens->cmd[1]);
		}
		else
			exec_pipes(tokens, env);
		tokens = tokens->next;
	}
}

int main(int argc, char **argv, char **env)
{
	t_token *tokens;
	if(argc > 1)
	{
		int i = 1;
		while(argv[i])
		{
			if(!strcmp(argv[i], ";"))
			{
				i++;
				tokens = NULL;
				continue;
			}
			i += parse_argu(&tokens, &argv[i]);
			exec_cmds(tokens,env);
			// print_token(tokens);
			// printf("finsihed 1 row\n");
			free_all(tokens);//TO add later
		}
	}
	return 0;
}