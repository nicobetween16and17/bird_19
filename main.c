//#include "cub3d.h"
#include "MLX42/include/MLX42/MLX42.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <memory.h>

mlx_image_t	*g_img;

void	hook(void *param)
{
	mlx_t	*mlx;

	mlx = param;
	if (mlx_is_key_down(param, MLX_KEY_ESCAPE))
		mlx_close_window(param);
	if (mlx_is_key_down(param, MLX_KEY_UP))
		g_img->instances[0].y -= 1;
	if (mlx_is_key_down(param, MLX_KEY_DOWN))
		g_img->instances[0].y += 1;
	if (mlx_is_key_down(param, MLX_KEY_LEFT))
		g_img->instances[0].x -= 1;
	if (mlx_is_key_down(param, MLX_KEY_RIGHT))
		g_img->instances[0].x += 1;
}

int32_t	main(int ac, char **av)
{
	mlx_t	*mlx;
	int h;
	int w;

	h = 1;
	w = 1;
	printf("(%d x %d)\n", h, w);
	mlx = mlx_init(w, h, "cub3d", true);
	if (!mlx)
		return (0);
	mlx_get_monitor_size(0, &w, &h);
	mlx_terminate(mlx);
	mlx = mlx_init(w, h, "cub3d", true);
	printf("(%d x %d)\n", h, w);
	g_img = mlx_new_image(mlx, h / 5, h / 5);
	memset(g_img->pixels, 255, g_img->width * g_img->height * sizeof(int));
	mlx_image_to_window(mlx, g_img, 0, 0);
	mlx_loop_hook(mlx, &hook, mlx);
	mlx_loop(mlx);
	mlx_terminate(mlx);
	return (EXIT_SUCCESS);
}