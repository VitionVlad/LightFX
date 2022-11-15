Name = LightFX

File = main.cpp

all:
	@echo "compiling..."
	@g++ $(File) -o $(Name) -lglfw -lGL -lGLEW -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi -m64 -g

clean:
	@echo "deleting..."
	@rm -rf $(Name)