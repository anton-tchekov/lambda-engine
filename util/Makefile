all: compression mesh_convert texture_convert font_convert audio_convert cubemap_convert aabb_to_obj shadow_convert

aabb_to_obj:
	gcc aabb_to_obj.c -I../../engine/src ../../engine/src/readfile.c ../../engine/src/vector.c -lm -o aabb_to_obj -Wall -Wextra

compression:
	gcc compression.c -I../../engine/src -I../../engine/src/lib -g -lm -lz -o compression -Wall -Wextra

shadow_convert: shadow_convert.cpp
	g++ shadow_convert.cpp -I../../engine/src ../../engine/src/vector.c -g -fpermissive -Wall -Wextra -o shadow_convert -lassimp -I../src/

mesh_convert:
	gcc mesh_convert.c -g \
	-I../../engine/src/ \
	-I../../engine/src/ \
	../../engine/src/readfile.c \
	../../engine/src/bmp.c \
	../../engine/src/log.c \
	../../engine/src/thread.c \
	../../engine/src/vector.c \
	-lm -o mesh_convert -Wall -Wextra -pthread -lglfw -O2

texture_convert:
	gcc texture_convert.c -o texture_convert -Wall -Wextra -lm

font_convert:
	gcc font_convert.c -Ilib -lfreetype -g -o font_convert -Wall -Wextra

audio_convert:
	gcc audio_convert.c -I../../engine/src/ -o audio_convert -Wall -Wextra

cubemap_convert:
	gcc cubemap_convert.c -o cubemap_convert -Wall -Wextra -lm

clean:
	rm -f aabb_to_obj
	rm -f compression
	rm -f shadow_convert
	rm -f mesh_convert
	rm -f texture_convert
	rm -f font_convert
	rm -f audio_convert
	rm -f cubemap_convert
