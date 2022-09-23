NAME = bird

all: $(NAME)

brew:
	curl -fsSL https://rawgit.com/kube/42homebrew/master/install.sh | zsh

lib:
	brew update
	brew install glfw
	git clone https://github.com/codam-coding-college/MLX42.git
	git clone git@github.com:nicobetween16and17/Libft.git
	cd MLX42 && make re && cd ../Libft && make re && cd ..

$(NAME):
	gcc main.c Libft/libft.a MLX42/libmlx42.a -I include -lglfw -L "/Users/$(USER)/.brew/opt/glfw/lib/" -o $(NAME)

clean:
	rm -f *.o

fclean:
	make clean
	rm -f $(NAME)