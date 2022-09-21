//#include "cub3d.h"
#include "MLX42/include/MLX42/MLX42.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <string.h>
# include <sys/time.h>

typedef struct s_node
{
	uint32_t pos[2];
	int nb;
	struct s_node *next;
}t_node;

typedef struct s_list
{
	mlx_image_t *img_h;
	mlx_image_t *img_b;
	int y_h;
	int x;
	int y_b;
	struct s_list *next;
}t_list;

typedef struct s_vars
{
	mlx_t *mlx;
	mlx_texture_t *image;
	t_list *tuyaux;
	int h;
	int w;
	int cursor_x;
	int cursor_y;
	t_node *sprite;
	uint32_t size[2];
	mlx_image_t *img;
	struct timeval time;
	mlx_image_t *bird;
	mlx_texture_t *tuyau_b;
	mlx_texture_t *tuyau_h;
	int			speed;
	int			start_s;
	int			start_ms;
	int			last_x_mouse;
	int			score;
	int			pause;
	int			pause_delay;
	char		*score_display;
	mlx_image_t *score_ptr;
	mlx_texture_t *texture_pause;
	mlx_image_t *pause_screen;
}	t_vars;

t_list *new_list(mlx_image_t *img_h, mlx_image_t *img_b, int y_b, int y_h)
{
	t_list *new;
	new = malloc(sizeof(t_list));
	new->img_h = img_h;
	new->img_b = img_b;
	new->x = 2560;
	new->y_b = y_b;
	new->y_h = y_h;
	new->next = NULL;
	return (new);
}

int	ms(t_vars *v)
{
	int	res;
	gettimeofday(&v->time, NULL);
	res = (((v->time.tv_sec - v->start_s) * 1000000) + v->time.tv_usec - v->start_ms) / 1000;
	return (res);

}

void collision(t_vars *v)
{
	t_list *start;

	start = v->tuyaux;
	while (start)
	{
		if (start->x > 100 && start->x < 450)
		{
			if (v->bird->instances[0].y > start->y_b || v->bird->instances[0].y < start->y_h + 1200)
				exit(0);
		}
		start = start->next;
	}
}
char radix_10(int nb, int div)
{
	while (div--)
		nb /= 10;
	return (nb % 10 + 48);
}

void set_score_display(t_vars *v)
{
	int len;
	int score;
	int i;

	i = 0;
	len = 0;
	score = v->score;
	if (!score)
		len = 1;

	while (score)
	{
		score /= 10;
		len++;
	}
	if (v->score_display)
		free(v->score_display);
	v->score_display = malloc(sizeof(char) * (8 + len));
	memcpy(v->score_display, "Score: ", 7);
	score = v->score;
	if (!score && ++i)
		v->score_display[7] = '0';
	while (score)
	{
		len--;
		v->score_display[7 + i++] = radix_10(v->score, len);
		score /= 10;
	}
	v->score_display[7 + i] = 0;
	printf("%s\n", v->score_display);
}

void add_end(t_list **list, t_list *new)
{
	t_list *start;
	start = *list;
	while (start && start->next)
		start = start->next;
	start->next = new;
}

void	display_obstacle(t_vars *v)
{
	static int mode;
	static int time;

	t_list *start;
	start = v->tuyaux;
	while (start)
	{
		mlx_delete_image(v->mlx, start->img_b);
		mlx_delete_image(v->mlx, start->img_h);
		start->x -= v->speed;
		start->img_h = mlx_texture_to_image(v->mlx, v->tuyau_h);
		start->img_b = mlx_texture_to_image(v->mlx, v->tuyau_b);
		mlx_image_to_window(v->mlx, start->img_b, start->x, start->y_b);
		mlx_image_to_window(v->mlx, start->img_h, start->x, start->y_h);
		start->img_h->instances[0].x -= v->speed;
		start->img_b->instances[0].x -= v->speed;
		start->img_h->instances[0].z = 3;
		start->img_b->instances[0].z = 3;
		if (!start->next && start->x <= v->w - 1000)
		{
			if (!(mode % 2))
				add_end(&v->tuyaux, new_list(mlx_texture_to_image(v->mlx, v->tuyau_h), mlx_texture_to_image(v->mlx, v->tuyau_b), 1000 - (ms(v) % 150), -500 - (ms(v) % 150)));
			else
				add_end(&v->tuyaux, new_list(mlx_texture_to_image(v->mlx, v->tuyau_h), mlx_texture_to_image(v->mlx, v->tuyau_b), 500 - (ms(v) % 150) , -1000 - (ms(v) % 150)));
		mode++;
		}
		start = start->next;
	}
	if (v->tuyaux->x <= -400)
	{
		v->score++;
		mlx_delete_image(v->mlx, v->score_ptr);
		set_score_display(v);
		v->score_ptr = mlx_put_string(v->mlx, v->score_display, 10, 10);
		start = v->tuyaux;
		mlx_delete_image(v->mlx, v->tuyaux->img_h);
		mlx_delete_image(v->mlx, v->tuyaux->img_b);
		v->tuyaux = v->tuyaux->next;
		free(start);
	}
	if (ms(v) > time + 2500) {
		time = ms(v);
		v->speed++;
	}
}

void	hook(void *param)
{
	t_vars 	*v;
	v = param;
	mlx_get_mouse_pos(v->mlx, &v->cursor_x, &v->cursor_y);
	static int time;
	if (mlx_is_key_down(v->mlx, MLX_KEY_ESCAPE))
		mlx_close_window(v->mlx);
	if (mlx_is_key_down(v->mlx, MLX_KEY_SPACE) && ms(v) > v->pause_delay)
	{
		v->pause_delay = ms(v) + 400;
		if (!v->pause)
		{
			v->pause = 1;
			v->pause_screen = mlx_texture_to_image(v->mlx, v->texture_pause);
			mlx_image_to_window(v->mlx, v->pause_screen, 0, 0);
			v->pause_screen->instances[0].z = 20;
		}
		else
		{
			v->pause = 0;
			mlx_delete_image(v->mlx, v->pause_screen);
		}
	}
	if (!v->pause)
	{
		display_obstacle(v);
		if (ms(v) > time + 150 || !time)
		{
			time = ms(v);
			if (v->bird)
				mlx_delete_image(v->mlx, v->bird);
			v->bird = mlx_texture_area_to_image(v->mlx, v->image, v->sprite->pos, v->size);
			mlx_image_to_window(v->mlx, v->bird, v->cursor_y, v->cursor_x);
			v->bird->instances[0].z = 1;
			v->sprite = v->sprite->next;
		}


		v->bird->instances[0].y = v->cursor_y;
		if (v->cursor_y <= 0)
			v->bird->instances[0].y = 0;
		else if (v->cursor_y >= v->h - v->bird->width)
			v->bird->instances[0].y = v->h - v->bird->width;
		v->bird->instances[0].x = 50;
		collision(v);
		usleep(10);
		v->last_x_mouse = v->cursor_x;
	}
}
t_node *new_node(int x, int y)
{
	t_node *node;

	node = malloc(sizeof(t_node));
	node->pos[0] = x;
	node->pos[1] = y;
	node->next = NULL;
	return (node);
}
void add_back(t_node **start, t_node *node)
{
	t_node *s;

	s = *start;
	while (s->next)
		s = s->next;
	s->next = node;
}

void set_position(t_vars *v)
{
	t_node *start;
	t_node *last;
	start = new_node(0, 0);
	add_back(&start, new_node(498,0));
	add_back(&start, new_node(997,0));
	add_back(&start, new_node(1494,0));
	add_back(&start, new_node(0,340));
	add_back(&start, new_node(498,340));
	add_back(&start, new_node(997,340));
	add_back(&start, new_node(1494,340));
	last = start;
	while (last->next)
		last = last->next;
	last->next = start;
	v->sprite = start;
}
int32_t	main(int ac, char **av)
{
	t_vars v;

	v.pause_delay = 0;
	v.size[0] = 498;
	v.size[1] = 330;
	v.speed = 15;
	gettimeofday(&v.time, NULL);
	v.start_s = v.time.tv_sec;
	v.start_ms = v.time.tv_usec;
	v.pause = 0;
	set_position(&v);
	v.mlx = mlx_init(1, 1, "cub3d", true);
	if (!v.mlx)
		return (0);
	mlx_get_monitor_size(0, &v.w, &v.h);
	mlx_terminate(v.mlx);
	v.mlx = mlx_init(v.w, v.h, "test", true);
	v.image = mlx_load_png("birds.png");
	v.bird = mlx_texture_area_to_image(v.mlx, v.image, v.sprite->pos, v.size);
	v.tuyau_b = mlx_load_png("tuyau_bas.png");
	v.tuyau_h = mlx_load_png("tuyau_haut.png");
	v.tuyaux = new_list(mlx_texture_to_image(v.mlx, v.tuyau_h), mlx_texture_to_image(v.mlx, v.tuyau_b), 500, -1000);
	v.img = mlx_texture_to_image(v.mlx, mlx_load_png("sky.png"));
	mlx_image_to_window(v.mlx, v.img, 0, 0);
	v.score_ptr = mlx_put_string(v.mlx, "Score: 0", 10, 10);
	mlx_set_instance_depth(&v.score_ptr->instances[0], 15);
	v.img->instances[0].z = 0;
	v.texture_pause = mlx_load_png("pause.png");
	mlx_set_cursor_mode(v.mlx, MLX_MOUSE_HIDDEN);
	mlx_get_mouse_pos(v.mlx, &v.cursor_x, &v.cursor_y);
	v.last_x_mouse = v.cursor_x;
	mlx_loop_hook(v.mlx, &hook, &v);
	mlx_loop(v.mlx);
	mlx_terminate(v.mlx);
	return (EXIT_SUCCESS);
}