# Source Folders
FOLDER+=-Isrc/
FOLDER+=-Isrc/datastructure
FOLDER+=-Isrc/network
FOLDER+=-Isrc/io
FOLDER+=-Isrc/lib

# Engine

# OpenGL
SOURCE+=lib/glad.c

# Network
HEADER+=network/network.h
SOURCE+=network/network.c

HEADER+=network/client.h
SOURCE+=network/client.c

HEADER+=network/server.h
SOURCE+=network/server.c

# Datastructure
HEADER+=datastructure/crc.h
SOURCE+=datastructure/crc.c

HEADER+=datastructure/hashmap.h
SOURCE+=datastructure/hashmap.c

HEADER+=defines.h

HEADER+=conversion.h
SOURCE+=conversion.c

HEADER+=folder.h
SOURCE+=folder.c

HEADER+=opengl.h
SOURCE+=opengl.c

HEADER+=openal.h

HEADER+=readfile.h
SOURCE+=readfile.c

HEADER+=shaders.h
SOURCE+=shaders.c

HEADER+=strings.h
SOURCE+=strings.c

HEADER+=terminal.h

HEADER+=thread.h
SOURCE+=thread.c

HEADER+=timer.h
SOURCE+=timer.c

HEADER+=types.h

HEADER+=uniform.h
SOURCE+=uniform.c

HEADER+=utf8.h
SOURCE+=utf8.c

HEADER+=vector.h
SOURCE+=vector.c

HEADER+=wireframe.h
SOURCE+=wireframe.c

HEADER+=debug.h
SOURCE+=debug.c

HEADER+=log.h
SOURCE+=log.c

HEADER+=recognizer.h
SOURCE+=recognizer.c

HEADER+=aabb.h
SOURCE+=aabb.c

HEADER+=font.h
SOURCE+=font.c

HEADER+=renderer3d.h
SOURCE+=renderer3d.c

HEADER+=selection.h
SOURCE+=selection.c

HEADER+=scene.h
SOURCE+=scene.c

HEADER+=datastructure.h
SOURCE+=datastructure.c

HEADER+=shader.h
SOURCE+=shader.c

HEADER+=bmp.h
SOURCE+=bmp.c

HEADER+=screenshot.h
SOURCE+=screenshot.c

HEADER+=config.h
SOURCE+=config.c

HEADER+=settings.h
SOURCE+=settings.c

HEADER+=internal.h
SOURCE+=internal.c

HEADER+=lighting.h
SOURCE+=lighting.c

HEADER+=cubemap.h
SOURCE+=cubemap.c

HEADER+=texture.h
SOURCE+=texture.c

HEADER+=support.h
SOURCE+=support.c

HEADER+=camera.h
SOURCE+=camera.c

HEADER+=color.h
SOURCE+=color.c

HEADER+=skybox.h
SOURCE+=skybox.c

HEADER+=condition.h
SOURCE+=condition.c

HEADER+=renderer2d.h
SOURCE+=renderer2d.c

HEADER+=shadow.h
SOURCE+=shadow.c

HEADER+=audio.h
SOURCE+=audio.c

HEADER+=physics.h
SOURCE+=physics.c

HEADER+=engine.h
SOURCE+=engine.c

HEADER+=graphics.h
SOURCE+=graphics.c

HEADER+=billboard.h
SOURCE+=billboard.c

HEADER+=mesh.h
SOURCE+=mesh.c

HEADER+=csm.h
SOURCE+=csm.c

HEADER+=blur.h
SOURCE+=blur.c

# I/O
HEADER+=io/aaf.h
SOURCE+=io/aaf.c

HEADER+=io/acf.h
SOURCE+=io/acf.c

HEADER+=io/aif.h
SOURCE+=io/aif.c

HEADER+=io/amf.h
SOURCE+=io/amf.c

HEADER+=io/aff.h
SOURCE+=io/aff.c

HEADER+=io/asf.h
SOURCE+=io/asf.c

HEADER+=io/dat.h
SOURCE+=io/dat.c

# Shaders

# Lighting
SHADER+=glsl/lighting_vs.glsl
SHADER+=glsl/lighting_fs.glsl

# Billboard
SHADER+=glsl/billboard_vs.glsl
SHADER+=glsl/billboard_fs.glsl

# Selection
SHADER+=glsl/billboard_sel_vs.glsl
SHADER+=glsl/billboard_sel_fs.glsl

SHADER+=glsl/selection_vs.glsl
SHADER+=glsl/selection_fs.glsl

SHADER+=glsl/selection_position_vs.glsl
SHADER+=glsl/selection_position_fs.glsl

# Shadow
SHADER+=glsl/shadow_vs.glsl
SHADER+=glsl/shadow_fs.glsl

# Shadow Map
SHADER+=glsl/shadow_map_vs.glsl
SHADER+=glsl/shadow_map_fs.glsl

SHADER+=glsl/shadow_bw_vs.glsl
SHADER+=glsl/shadow_bw_fs.glsl

# Blur
SHADER+=glsl/blur_vs.glsl
SHADER+=glsl/blur_fs.glsl

# Skybox
SHADER+=glsl/skybox_vs.glsl
SHADER+=glsl/skybox_fs.glsl

# Wireframe
SHADER+=glsl/wireframe_vs.glsl
SHADER+=glsl/wireframe_fs.glsl

# 2D Renderer
SHADER+=glsl/renderer2d_vs.glsl
SHADER+=glsl/renderer2d_fs.glsl

