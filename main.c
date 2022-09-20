//#include "cub3d.h"
#include "MLX42/include/MLX42/MLX42.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <time.h>

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
	int timer;
	t_node *sprite;
	uint32_t size[2];
	mlx_image_t *img;
	time_t time;
	mlx_image_t *bird;
	mlx_texture_t *tuyau_b;
	mlx_texture_t *tuyau_h;

}t_vars;

t_list *new_list(mlx_image_t *img_h, mlx_image_t *img_b, int y_b, int y_h)
{
	t_list *new;
	printf("hey\n");
	new = malloc(sizeof(t_list));
	new->img_h = img_h;
	new->img_b = img_b;
	new->x = 2460;
	new->y_b = y_b;
	new->y_h = y_h;
	new->next = NULL;
	printf("crash\n");
	return (new);
}

void add_end(t_list **list, t_list *new)
{
	t_list *start;
	printf("hey\n");
	start = *list;
	while (start && start->next)
		start = start->next;
	start->next = new;
	printf("crash\n");
}
void	display_obstacle(t_vars *v)
{
	t_list *start;
	start = v->tuyaux;
	printf("obstacle are displaying...\n");
	while (start) {
		printf("%d %d %d\n", start->x, start->y_b, start->y_h);
		mlx_delete_image(v->mlx, start->img_b);
		mlx_delete_image(v->mlx, start->img_h);
		start->x -= 10;
		start->img_h = mlx_texture_to_image(v->mlx, v->tuyau_h);
		start->img_b = mlx_texture_to_image(v->mlx, v->tuyau_b);
		mlx_image_to_window(v->mlx, start->img_b, start->x, start->y_b);
		mlx_image_to_window(v->mlx, start->img_h, start->x, start->y_h);
		start = start->next;
		if (!start->next && start->x <= v->h - 1000)
			add_end(&v->tuyaux, new_list(mlx_texture_to_image(v->mlx, v->tuyau_h), mlx_texture_to_image(v->mlx, v->tuyau_b), 500, -500));
	}
	if (v->tuyaux->x <= 0)
		v->tuyaux = v->tuyaux->next;
}

void	hook(void *param)
{
	t_vars 	*v;
	v = param;
	mlx_get_mouse_pos(v->mlx, &v->cursor_x, &v->cursor_y);
	static int time;
	if (!(v->timer) % 2)
		display_obstacle(v);
	if (!(v->timer % 5) && printf("change_image... %d\n", time++))
	{
		if (v->bird)
			mlx_delete_image(v->mlx, v->bird);
		v->bird = mlx_texture_area_to_image(v->mlx, v->image, v->sprite->pos, v->size);
		mlx_image_to_window(v->mlx, v->bird, v->cursor_y, v->cursor_x);
		v->sprite = v->sprite->next;
	}
	if (mlx_is_key_down(v->mlx, MLX_KEY_ESCAPE))
		mlx_close_window(v->mlx);
	v->bird->instances[0].y = v->cursor_y;
	if (v->cursor_y <= 0)
		v->bird->instances[0].y = 0;
	else if (v->cursor_y >= v->h - v->bird->width)
		v->bird->instances[0].y = v->h - v->bird->width;
	v->bird->instances[0].x = 50;
	usleep(10000);
	v->timer++;

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

	v.timer = 0;
	v.size[0] = 498;
	v.size[1] = 330;
	set_position(&v);
	printf("launch...\n");
	v.mlx = mlx_init(1, 1, "cub3d", true);
	if (!v.mlx)
		return (0);
	printf("open...\n");
	mlx_get_monitor_size(0, &v.w, &v.h);
	mlx_terminate(v.mlx);
	printf("got size...\n");
	v.mlx = mlx_init(v.w, v.h, "test", true);
	printf("resize\n");
	printf("(%d x %d)\n", v.h, v.w);
	v.image = mlx_load_png("birds.png");
	v.bird = mlx_texture_area_to_image(v.mlx, v.image, v.sprite->pos, v.size);
	v.tuyau_b = mlx_load_png("tuyau_bas.png");
	v.tuyau_h = mlx_load_png("birds.png");
	v.tuyaux = new_list(mlx_texture_to_image(v.mlx, v.tuyau_h), mlx_texture_to_image(v.mlx, v.tuyau_b), 500, -500);
	printf("crash here\n");
	v.img = mlx_new_image(v.mlx, 2560, 1440);
	memset(v.img->pixels, 255, v.img->width * v.img->height * sizeof(int));
	mlx_image_to_window(v.mlx, v.img, 0, 0);
	v.img->instances[0].z = 1;
	mlx_loop_hook(v.mlx, &hook, &v);
	mlx_loop(v.mlx);
	mlx_terminate(v.mlx);
	system("leaks a.out");
	return (EXIT_SUCCESS);
}