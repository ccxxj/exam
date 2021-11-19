#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

typedef struct draw{
	int width;
	int height;
	char *print;
}	t_draw;

typedef struct circle{
	char type;
	float x;
	float y;
	float r;
	char color;
}	t_circle;

int ft_strlen(char *str)
{
	int i = 0;
	while (str[i])
		i++;
	return i;
}

int get_info(FILE *file, t_draw *draw)
{
	char *temp;
	char background;

	if (fscanf(file, "%d %d %c\n", &draw->width, &draw->height, &background) == 3)
	{
		temp = (char *)malloc(draw->width * draw->height);
		if (!temp)
			return 1;
		draw->print = temp;
		int i = 0;
		while(i < draw->width * draw->height)
		{
			draw->print[i] = background;
			i++;
		}
		return 0;
	}
	return 1;
}

int check_draw_input(t_draw *draw)
{
	if (draw->width > 300 || draw->width < 1 || draw->height > 300 || draw->height < 1)
		return 1;
	return 0;
}

int check_circle_input(t_circle *circle)
{
	if(circle->r > 0 && (circle->type == 'c' || circle->type == 'C'))
		return 0;
	return 1;
}

void print_all(t_draw *draw)
{
	int i = 0;
	while (i < draw->height)
	{
		write(1, draw->print + i*draw->width, draw->width);
		write(1, "\n", 1);
		i++;
	}
}

int check_element(float a, float b, t_circle *circle)
{
	float x = circle->x;
	float y = circle->y;
	float distance = sqrtf((a - x) * (a - x) + (b - y)*(b - y));
	if (distance <= circle->r)
	{
		if ((circle->r - distance) < 1.00000000)
			return 2;
		return 1;
	}
	return 0;
}

void transform(t_draw *draw, t_circle *circle)
{
	int i = 0;

	while (i < draw->width * draw->height)
	{
		int a = i % draw->width;
		int b = i / draw->width;
		int result = check_element((float)a, (float)b, circle);
		if (result == 2 || (result == 1 && circle->type == 'C'))
			draw->print[i] = circle->color;
		i++;
	}
}

int execute(FILE *file)
{
	t_draw draw;
	t_circle circle;

	if (!get_info(file, &draw) && !check_draw_input(&draw))
	{
		int result = fscanf(file, "%c %f %f %f %c\n", &circle.type, &circle.x, &circle.y, &circle.r, &circle.color);
		// printf(" data is %c %f %f %f %c\n", circle.type, circle.x, circle.y, circle.r, circle.color);
		while (result == 5 && !check_circle_input(&circle))
		{
			transform(&draw, &circle);
			result = fscanf(file, "%c %f %f %f %c\n", &circle.type, &circle.x, &circle.y, &circle.r, &circle.color);
		}
		if (result == -1)//EOF
			print_all(&draw);
		if(draw.print)
			free(draw.print);
		return 0;
	}
	if(draw.print)
		free(draw.print);
	return 1;
}

int main(int argc, char **argv)
{
	FILE *file;
	
	if (argc == 2)
	{
		file = fopen(argv[1], "r");
		if (file && !execute(file))
		{
			fclose(file);
			return 0;
		}
		int len1 = ft_strlen("Error: Operation file corrupted\n");
		write(1, "Error: Operation file corrupted\n", len1);
		fclose(file);
	}
	else
	{
		int len1 = ft_strlen("Error: argument\n");
		write(1, "Error: argument\n", len1);
	}
	return 1;
}