NAME = cub3d

all: $(NAME)

$(NAME):
  gcc *.c Libft/libft.a MLX42/libmlx42.a -lglfw

clean:
  rm -f *.o

flclean:
  make clean
  rm -f $(NAME)