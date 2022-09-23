NAME = bird

install:
	curl -fsSL https://rawgit.com/kube/42homebrew/master/install.sh | zsh
	brew update
	brew install glfw
	git clone git clone https://github.com/codam-coding-college/MLX42.git

lib:
	cd MLX42 && make re && cd ../Libft && make re && cd ..

all: $(NAME)

$(NAME):
	gcc main.c Libft/libft.a MLX42/libmlx42.a -I include -lglfw -L "/Users/$(USER)/.brew/opt/glfw/lib/" && ./a.out

clean:
	rm -f *.o

flclean:
	make clean
	rm -f $(NAME)