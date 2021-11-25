#ifndef __ENGINE_H__
#define __ENGINE_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Headers */
#include <math.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#include <unistd.h>
#include <pthread.h>

#ifdef _WIN32

#include <winsock2.h>
#include <ws2ipdef.h>
#include <ws2tcpip.h>

#else

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#endif

/* Types */
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

/* Defines */
#define NUM_TEXTURE_LAYERS            6
#define BILLBOARD_TEXTURE_SIZE     2048
#define COLOR_CORNER_SIZE            32

#define CORNER_WHITE_X                 (BILLBOARD_TEXTURE_SIZE - COLOR_CORNER_SIZE)
#define CORNER_WHITE_Y                 (BILLBOARD_TEXTURE_SIZE - COLOR_CORNER_SIZE)
#define CORNER_WHITE_W                 (COLOR_CORNER_SIZE)
#define CORNER_WHITE_H                 (COLOR_CORNER_SIZE)

#define CORNER_BLACK_X                 (BILLBOARD_TEXTURE_SIZE - 2 * COLOR_CORNER_SIZE)
#define CORNER_BLACK_Y                 (BILLBOARD_TEXTURE_SIZE - COLOR_CORNER_SIZE)
#define CORNER_BLACK_W                 (COLOR_CORNER_SIZE)
#define CORNER_BLACK_H                 (COLOR_CORNER_SIZE)

#define CORNER_TRANSPARENT_X           (BILLBOARD_TEXTURE_SIZE - 3 * COLOR_CORNER_SIZE)
#define CORNER_TRANSPARENT_Y           (BILLBOARD_TEXTURE_SIZE - 3 * COLOR_CORNER_SIZE)
#define CORNER_TRANSPARENT_W           (COLOR_CORNER_SIZE)
#define CORNER_TRANSPARENT_H           (COLOR_CORNER_SIZE)

#define MOUSE_BUTTON_1         0
#define MOUSE_BUTTON_2         1
#define MOUSE_BUTTON_3         2
#define MOUSE_BUTTON_4         3
#define MOUSE_BUTTON_5         4
#define MOUSE_BUTTON_6         5
#define MOUSE_BUTTON_7         6
#define MOUSE_BUTTON_8         7
#define MOUSE_BUTTON_LAST      MOUSE_BUTTON_8
#define MOUSE_BUTTON_LEFT      MOUSE_BUTTON_1
#define MOUSE_BUTTON_RIGHT     MOUSE_BUTTON_2
#define MOUSE_BUTTON_MIDDLE    MOUSE_BUTTON_3

#define KEY_PRESS                   1
#define KEY_RELEASE                 0

/* Keys */
#define KEY_UNKNOWN                -1
#define KEY_SPACE                  32
#define KEY_APOSTROPHE             39 /* ' */
#define KEY_COMMA                  44 /* , */
#define KEY_MINUS                  45 /* - */
#define KEY_PERIOD                 46 /* . */
#define KEY_SLASH                  47 /* / */
#define KEY_0                      48
#define KEY_1                      49
#define KEY_2                      50
#define KEY_3                      51
#define KEY_4                      52
#define KEY_5                      53
#define KEY_6                      54
#define KEY_7                      55
#define KEY_8                      56
#define KEY_9                      57
#define KEY_SEMICOLON              59 /* ; */
#define KEY_EQUAL                  61 /* = */
#define KEY_A                      65
#define KEY_B                      66
#define KEY_C                      67
#define KEY_D                      68
#define KEY_E                      69
#define KEY_F                      70
#define KEY_G                      71
#define KEY_H                      72
#define KEY_I                      73
#define KEY_J                      74
#define KEY_K                      75
#define KEY_L                      76
#define KEY_M                      77
#define KEY_N                      78
#define KEY_O                      79
#define KEY_P                      80
#define KEY_Q                      81
#define KEY_R                      82
#define KEY_S                      83
#define KEY_T                      84
#define KEY_U                      85
#define KEY_V                      86
#define KEY_W                      87
#define KEY_X                      88
#define KEY_Y                      89
#define KEY_Z                      90
#define KEY_LEFT_BRACKET           91 /* [ */
#define KEY_BACKSLASH              92 /* \ */
#define KEY_RIGHT_BRACKET          93 /* ] */
#define KEY_GRAVE_ACCENT           96 /* ` */
#define KEY_WORLD_1               161 /* non-US #1 */
#define KEY_WORLD_2               162 /* non-US #2 */
#define KEY_ESCAPE                256
#define KEY_ENTER                 257
#define KEY_TAB                   258
#define KEY_BACKSPACE             259
#define KEY_INSERT                260
#define KEY_DELETE                261
#define KEY_RIGHT                 262
#define KEY_LEFT                  263
#define KEY_DOWN                  264
#define KEY_UP                    265
#define KEY_PAGE_UP               266
#define KEY_PAGE_DOWN             267
#define KEY_HOME                  268
#define KEY_END                   269
#define KEY_CAPS_LOCK             280
#define KEY_SCROLL_LOCK           281
#define KEY_NUM_LOCK              282
#define KEY_PRINT_SCREEN          283
#define KEY_PAUSE                 284
#define KEY_F1                    290
#define KEY_F2                    291
#define KEY_F3                    292
#define KEY_F4                    293
#define KEY_F5                    294
#define KEY_F6                    295
#define KEY_F7                    296
#define KEY_F8                    297
#define KEY_F9                    298
#define KEY_F10                   299
#define KEY_F11                   300
#define KEY_F12                   301
#define KEY_F13                   302
#define KEY_F14                   303
#define KEY_F15                   304
#define KEY_F16                   305
#define KEY_F17                   306
#define KEY_F18                   307
#define KEY_F19                   308
#define KEY_F20                   309
#define KEY_F21                   310
#define KEY_F22                   311
#define KEY_F23                   312
#define KEY_F24                   313
#define KEY_F25                   314
#define KEY_KP_0                  320
#define KEY_KP_1                  321
#define KEY_KP_2                  322
#define KEY_KP_3                  323
#define KEY_KP_4                  324
#define KEY_KP_5                  325
#define KEY_KP_6                  326
#define KEY_KP_7                  327
#define KEY_KP_8                  328
#define KEY_KP_9                  329
#define KEY_KP_DECIMAL            330
#define KEY_KP_DIVIDE             331
#define KEY_KP_MULTIPLY           332
#define KEY_KP_SUBTRACT           333
#define KEY_KP_ADD                334
#define KEY_KP_ENTER              335
#define KEY_KP_EQUAL              336
#define KEY_LEFT_SHIFT            340
#define KEY_LEFT_CONTROL          341
#define KEY_LEFT_ALT              342
#define KEY_LEFT_SUPER            343
#define KEY_RIGHT_SHIFT           344
#define KEY_RIGHT_CONTROL         345
#define KEY_RIGHT_ALT             346
#define KEY_RIGHT_SUPER           347
#define KEY_MENU                  348
#define KEY_LAST                     KEY_MENU

/* Engine */
#define ENGINE_ENABLE_AUDIO         1
#define ENGINE_ENABLE_NETWORK       2

int engine_init(int flags, const char *title, int min_w, int min_h);
void engine_main_loop(void);
void engine_destroy(void);

void engine_exit(void);

u32 engine_fps(void);
double engine_time(void);
double engine_delta(void);
int engine_width(void);
int engine_height(void);

/* Settings */
typedef struct SETTING_INT
{
	const char *Key;
	int Default, Value;
} SettingInt;

typedef struct SETTING_BOOL
{
	const char *Key;
	int Default, Value;
} SettingBool;

typedef struct SETTING_FLOAT
{
	const char *Key;
	float Default, Value;
} SettingFloat;

typedef struct SETTING_STRING
{
	const char *Key, *Default, *Value;
} SettingString;

typedef struct SETTINGS
{
	u32 NumFloats, NumInts, NumBools, NumStrings;
	SettingFloat *Floats;
	SettingInt *Ints;
	SettingBool *Bools;
	SettingString *Strings;
} Settings;

int settings_read(const char *restrict filename, Settings *restrict settings);
int settings_write(const char *restrict filename, Settings *restrict settings);

/* Vector */
#define EPSILON 0.00000001f
#define PI      3.14159265358979323846

#define X       0
#define Y       1
#define Z       2
#define W       3

#define RAD(x)  ((x) * (PI / 180.0))
#define DEG(x)  ((x) * (180.0 / PI))

#define RADF(x) (float)((x) * ((float)PI / 180.0f))
#define DEGF(x) (float)((x) * (180.0f / (float)PI))

typedef float vec2[2];
typedef float vec3[3];
typedef float vec4[4];
typedef vec4 mat4[4];

i32 abs(i32 a);

/* vector */
void vec2_add(vec2 r, const vec2 a, const vec2 b);
void vec3_add(vec3 r, const vec3 a, const vec3 b);
void vec4_add(vec4 r, const vec4 a, const vec4 b);

void vec2_sub(vec2 r, const vec2 a, const vec2 b);
void vec3_sub(vec3 r, const vec3 a, const vec3 b);
void vec4_sub(vec4 r, const vec4 a, const vec4 b);

void vec2_scale(vec2 r, const vec2 v, float s);
void vec3_scale(vec3 r, const vec3 v, float s);
void vec4_scale(vec4 r, const vec4 v, float s);

void vec2_dup(vec2 r, const vec2 v);
void vec3_dup(vec3 r, const vec3 v);
void vec4_dup(vec4 r, const vec4 v);

float vec2_dot(const vec2 a, const vec2 b);
float vec3_dot(const vec3 a, const vec3 b);
float vec4_dot(const vec4 a, const vec4 b);

float vec2_len(const vec2 v);
float vec3_len(const vec3 v);
float vec4_len(const vec4 v);

void vec2_norm(vec2 r, const vec2 v);
void vec3_norm(vec3 r, const vec3 v);
void vec4_norm(vec4 r, const vec4 v);

void tri_norm(vec3 n, const vec3 p0, const vec3 p1, const vec3 p2);

float vec2_dist(const vec2 p0, const vec2 p1);
float vec3_dist(const vec3 p0, const vec3 p1);

/* r, a and b must never point to the same vector */
void vec3_cross(vec3 r, const vec3 a, const vec3 b);
void vec4_cross(vec4 r, const vec4 a, const vec4 b);

void vec3_reflect(vec3 r, const vec3 v, const vec3 n);
void vec4_reflect(vec4 r, const vec4 v, const vec4 n);

/* matrix */
void mat4_identity(mat4 m);
void mat4_dup(mat4 m, const mat4 n);
void mat4_row(vec4 r, const mat4 m, int i);
void mat4_col(vec4 r, const mat4 m, int i);

void mat4_transpose(mat4 m, const mat4 n);

void mat4_add(mat4 m, const mat4 a, const mat4 b);
void mat4_sub(mat4 m, const mat4 a, const mat4 b);
void mat4_scale(mat4 m, const mat4 a, float x, float y, float z);
void mat4_mul(mat4 m, const mat4 a, const mat4 b);

void mat4_translate(mat4 m, float x, float y, float z);
void mat4_translate_in_place(mat4 m, float x, float y, float z);

void mat4_rotate_x(mat4 q, const mat4 m, float angle);
void mat4_rotate_y(mat4 q, const mat4 m, float angle);
void mat4_rotate_z(mat4 q, const mat4 m, float angle);

void mat4_invert(mat4 t, const mat4 m);
void mat4_orthonormalize(mat4 r, const mat4 m);
void mat4_frustum(mat4 m, float l, float r, float b, float t, float n, float f);

void mat4_ortho(mat4 m, float l, float r, float b, float t, float n, float f);
void mat4_perspective(mat4 m, float y_fov, float aspect, float n, float f);
void mat4_look_at(mat4 m, const vec3 eye, const vec3 center, const vec3 up);

void mat4_mul_vec3(vec3 r, const mat4 m, const vec3 v);
void mat4_mul_vec4(vec4 r, const mat4 m, const vec4 v);

void mat4_print(const mat4 m);
void mat4_translation(const mat4 m);

/* Camera */
typedef struct CAMERA
{
	float NearPlane, FarPlane;
	mat4 Perspective, PerspectiveSkybox, PerspectiveEye[2];
	vec3 Position, Rotation;
	vec2 Front, Right, FOV;
} Camera;

void camera_init(Camera *camera, float fov, float near, float far);
void camera_update(Camera *camera);
void camera_on_mouse(Camera *camera, double offset_x, double offset_y);
void camera_perspective(Camera *camera, float scale);
void camera_fov_x(Camera *camera, float fov_x, float scale);
void camera_fov_y(Camera *camera, float fov_y, float scale);

/* Color */
typedef u8 ColorRGBA8[4];

struct COLORS
{
	vec4 RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, WHITE, BLACK, TRANSPRNT;
};

struct COLORS8
{
	ColorRGBA8 RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, WHITE, BLACK, TRANSPRNT;
};

extern const struct COLORS COLOR;
extern const struct COLORS8 COLOR8;

/* Screenshot */
int screenshot(void (*event_complete)(const char *));

/* Folder */
int folder_exists(const char *dirname);
int folder_create(const char *dirname);
int folder_create_if_not_exists(const char *dirname);

/* Log */
#ifdef DEBUG

void log_trace(const char *format, ...);
void log_debug(const char *format, ...);

#else

#define log_trace(...)
#define log_debug(...)

#endif

void log_info(const char *format, ...);
void log_warn(const char *format, ...);
void log_error(const char *format, ...);
void log_fatal(const char *format, ...);

/* Lighting */
#define NUM_POINT_LIGHTS    4

typedef struct DIR_LIGHT
{
	vec3 Position, Color;
} DirLight;

typedef struct POINT_LIGHTS
{
	vec3 Position[NUM_POINT_LIGHTS],
		Color[NUM_POINT_LIGHTS];

	float Constant[NUM_POINT_LIGHTS],
		Linear[NUM_POINT_LIGHTS],
		Quadratic[NUM_POINT_LIGHTS];
} PointLights;

typedef struct LIGHTING
{
	DirLight DirLight;
	PointLights PointLights;
} Lighting;

void lighting_init(Lighting *lighting);

void light_dir_position(Lighting *lighting, float x, float y, float z);
void light_dir_position_v3(Lighting *lighting, const vec3 position);
void light_dir_color(Lighting *lighting, float r, float g, float b);
void light_dir_color_v3(Lighting *lighting, const vec3 color);

void light_position_v3(Lighting *lighting, int id, const vec3 position);
void light_color_v3(Lighting *lighting, int id, const vec3 color);
void light_position(Lighting *lighting, int id, float x, float y, float z);
void light_color(Lighting *lighting, int id, float r, float g, float b);
void light_spread(Lighting *lighting, int id, float c, float l, float q);

/* UTF-8 */
const char *utf8(const char *restrict s, i32 *restrict out);

/* Conversion */
int parse_int(const char *s, int *out);
int parse_float(const char *s, float *out);
int parse_bool(const char *s, int *out);

/* BMP */
int bmp_write(const char *filename, const u8 *data, i32 width, i32 height);

/* AABB */
typedef struct AABB
{
	float VelocityY;
	vec3 Min, Max;
} AABB;

typedef struct WORLD
{
	float Gravity;
	AABB *Boxes;
	u32 NumBoxes;
} World;

void aabb_normalize(AABB *a);
void aabb_print(AABB *a);
void aabb_enclose(AABB *out, AABB *in, int count);
void aabb_transform(AABB *out, AABB *in, mat4 model);
int aabb_intersect(AABB *a, AABB *b);
void aabb_instance(AABB *out, AABB *in, mat4 model);

/* Physics */
typedef struct PLAYER
{
	u32 Flags;
	Camera Camera;
	vec3 Position;
	float VelocityY, VelocityXZ, AccelerationJump,
			HeightAboveEye, HeightBelowEye, Size,
			WalkAnimation;
} Player;

#define ACTION_FORWARD          0x01U
#define ACTION_BACKWARD         0x02U
#define ACTION_RIGHT            0x04U
#define ACTION_LEFT             0x08U
#define ACTION_UP               0x10U
#define ACTION_DOWN             0x20U
#define ACTION_JUMP             0x40U

#define PLAYER_FLAG_GAMEMODE    0x01U
#define PLAYER_FLAG_GROUNDED    0x02U

void player_get_aabb(Player *player, AABB *aabb);
float player_distance(Player *player, vec3 pos);
void player_init(Player *player, float vel_move, float accel_jump, float hbe, float hae, float size);
void player_update(Player *player, u32 keys, float delta_time);
float aabb_size_x(AABB *a);
float aabb_size_y(AABB *a);
float aabb_size_z(AABB *a);

/* Audio */
typedef u32 AudioBuffer;
typedef u32 AudioSource;

int audio_enabled(void);
void audio_buffer_destroy(AudioBuffer buffer);
void audio_listener_update(Camera *camera);
void audio_volume(float volume);

AudioSource audio_source_new(void);
void audio_source_play(AudioSource source);
int audio_source_playing(AudioSource source);
void audio_source_reset(AudioSource source);
void audio_source_pause(AudioSource source);
void audio_source_destroy(AudioSource source);
void audio_source_relative(AudioSource source, int relative);
void audio_source_position_v3(AudioSource source, vec3 position);
void audio_source_position(AudioSource source, float x, float y, float z);
void audio_source_looping(AudioSource source, int looping);
void audio_source_buffer(AudioSource source, AudioBuffer buffer);
void audio_source_volume(AudioSource source, float volume);
void audio_source_pitch(AudioSource source, float pitch);

/* Cubemap */
typedef u32 Cubemap;

/* Font */
typedef struct FONT_CHAR
{
	i32 Character, Advance, Location[2], Size[2], Bearing[2];
} FontChar;

typedef struct FONT_SIZE
{
	FontChar *Characters;
	i32 Size;
	u32 NumCharacters;
} FontSize;

typedef struct FONT
{
	FontSize *Sizes;
	u32 NumSizes;
} Font;

typedef struct FONT_COLLECTION
{
	i32 TextureSize;
	u32 NumFonts;
	Font *Fonts;
} FontCollection;

int renderer_font_init(void);
int renderer_font_use(void);

void renderer_font_projection(mat4 projection);
void renderer_font_window(void);

void font_collection_use(FontCollection *fc);
void font_collection_destroy(FontCollection *fc);

i32 font_render_string(i32 x, i32 y, const char *restrict s, u32 len);
i32 font_width_string(const char *restrict s, u32 len);
i32 font_height_string(const char *restrict s, u32 len);

/* Mesh */
#define NUM_TEXTURE_LAYERS          6

typedef struct MATERIAL
{
	u32 VertexOffset, VertexCount;
	u8 ColorDiffuse[4], ColorSpecular[4], ColorEmissive[4];
	u32 SizeDiffuse, SizeSpecular, SizeEmissive, SizeNormal;
	u32 LayerDiffuse, LayerSpecular, LayerEmissive, LayerNormal;
	float Shininess;
} Material;

typedef struct MESH_COLLECTION
{
	u32 NumMeshes, TotalVertices, TotalMaterials, *MaterialOffsets, *MaterialCounts;
	AABB *AABBs;
	Material *Materials;
	u32 VBO, VAO, Textures[NUM_TEXTURE_LAYERS];
} MeshCollection;

/* Texture */
typedef struct TEXTURE
{
	i32 Size, Count;
	u32 TextureID;
} Texture;

/* Shadow */
typedef struct SHADOW_EDGE
{
	u32 V0, V1, F0, F1;
} ShadowEdge;

typedef struct SHADOW_COLLECTION
{
	u8 *Base;
	u32 NumShadows, TotalVertices, TotalIndices, TotalEdges;
	u32 *VertexOffsets, *VertexCounts, *IndexOffsets, *IndexCounts, *EdgeOffsets, *EdgeCounts;
	vec3 *Vertices;
	vec3 *Transformed;
	u32 *Indices;
	ShadowEdge *Edges;
} ShadowCollection;

/* Compressed loader functions */
int audio_load(const char *filename, AudioBuffer *audio);

int meshes_load(const char *filename, MeshCollection *meshes);
void meshes_destroy(MeshCollection *meshes);

int shadows_load(const char *filename, ShadowCollection *shadow);
void shadows_destroy(ShadowCollection *shadow);

int texture_load(const char *filename, Texture *texture, u32 flags);
void texture_destroy(Texture *texture);

int cubemap_load(const char *filename, Cubemap *cubemap);
void cubemap_destroy(Cubemap *cubemap);

int fonts_load(const char *filename, FontCollection *fonts);
void fonts_destroy(FontCollection *fonts);

/* 2D Renderer */
typedef i16 Rect2D[4];
typedef u8 ColorRGBA8[4];

#define STR_LEN         0xFFFF

#define TEXTURE_LINEAR  (0 << 0)
#define TEXTURE_NEAREST (1 << 0)

#define TEXTURE_REPEAT  (0 << 1)
#define TEXTURE_CLAMP   (1 << 1)

void renderer2d_bind_texture(Texture *texture);
void renderer2d_bind_font_collection(FontCollection *fc);
void renderer2d_fill_square(int x, int y, int w, int h);

void rect2d_dup(Rect2D dst, Rect2D src);
void r2d_color8(const ColorRGBA8 color);
void r2d_color(u8 r, u8 g, u8 b, u8 a);
void r2d_font(u32 font_id, u32 size_id);
void r2d_lineheight(u32 h);
int r2d_width(const char *s, u32 len);
void r2d_maxwidth(u32 w);
void r2d_string(int x, int y, const char *s, u32 len);
void r2d_rect(int x0, int y0, int w0, int h0);
void r2d_rect_r(Rect2D r);
void r2d_quad(
		int x0, int y0,
		int x1, int y1,
		int x2, int y2,
		int x3, int y3);
void r2d_sprite(int x0, int y0, int w0, int h0,
		int x1, int y1, int w1, int h1, int layer);
void r2d_sprite_r(const Rect2D r0, const Rect2D r1, int layer);

/* Billboard / Screen */
#define SOURCE_DIFFUSE   0
#define SOURCE_EMISSIVE  1
#define SOURCE_SELECTION 2

typedef struct SCREEN
{
	Rect2D SourceDiffuse, SourceEmissive, SourceSelection;
	vec3 Position;
	mat4 ModelMatrix;
	float Width, Height;
	int SelectionID;
	u32 ConditionID;
} Screen;

void screen_preinit(void);
void screen_rect(Screen *screen, int area, int x, int y, int w, int h);
void screen_rect_r(Screen *screen, int area, Rect2D r);
void screen_sprite(Screen *screen, int area, int x0, int y0, int w0, int h0,
		int x1, int y1, int w1, int h1, int layer);

void screen_sprite_r(Screen *screen, int area, Rect2D r0, Rect2D r1, int layer);
void screen_string(Screen *screen, int area, int x, int y, const char *s, u32 len);
void screen_render(Screen *screen);

/* Pixel Transfer */
void screen_upload(Screen *screen, int area, int x, int y, int w, int h, u8 *pixels);
void screen_download(Screen *screen, int area, int x, int y, int w, int h, u8 *pixels);

/* Billboard */
void billboard_add(Rect2D src0, Rect2D src1, vec3 center, mat4 model, float w, float h, int sel_id, Rect2D sel);
void billboard_render(mat4 perspective, float shadow);
void billboard_selection_render(mat4 perspective);

/* User implemented */
void billboard_update(void);

/* Scene */
#define SCENE_FLAG_INVISIBLE 0x01
#define SCENE_FLAG_DISABLE   0x02
#define SCENE_FLAG_DIRTY     0x04
#define SCENE_FLAG_SHADOW    0x08
#define SCENE_FLAG_NO_SEL    0x10

typedef struct DYNAMIC_NODE
{
	mat4 ModelMatrix, PositionMatrix;
	struct DYNAMIC_NODE *Prev, *Next, *Children, *Parent;
	u32 SelectionID, MeshID, ShadowID, RenderCond, Flags;
} DynamicNode;

typedef struct SCENE
{
	Lighting *Lighting;
	DynamicNode Root;
} Scene;

void scene_init(Scene *scene, Lighting *lighting);

void node_init(DynamicNode *node, u32 mesh_id, u32 shadow_id, u32 selection_id, u32 condition_id, u32 flags);
void node_invalidate(DynamicNode *node);
void node_add(DynamicNode *node, DynamicNode *child);
void node_remove(DynamicNode *node);

/* RESET */
void node_matrix(DynamicNode *node);
void node_reset(DynamicNode *node, float x, float y, float z);
void node_reset_v3(DynamicNode *node, vec3 p);

/* TRANSLATE */
void node_translate_x(DynamicNode *node, float x);
void node_translate_y(DynamicNode *node, float y);
void node_translate_z(DynamicNode *node, float z);
void node_translate_xy(DynamicNode *node, float x, float y);
void node_translate_xz(DynamicNode *node, float x, float z);
void node_translate_yz(DynamicNode *node, float y, float z);
void node_translate(DynamicNode *node, float x, float y, float z);
void node_translate_v3(DynamicNode *node, vec3 p);

/* ROTATE */
void node_rotate_x(DynamicNode *node, float r);
void node_rotate_y(DynamicNode *node, float r);
void node_rotate_z(DynamicNode *node, float r);
void node_rotate_1(DynamicNode *node, int a, float ra);
void node_rotate_2(DynamicNode *node, int a, float ra, int b, float rb);
void node_rotate_3(DynamicNode *node, int a, float ra, int b, float rb, int c, float rc);

/* SCALE */
void node_scale_x(DynamicNode *node, float x);
void node_scale_y(DynamicNode *node, float y);
void node_scale_z(DynamicNode *node, float z);
void node_scale_xz(DynamicNode *node, float x, float z);
void node_scale_xy(DynamicNode *node, float x, float y);
void node_scale_yz(DynamicNode *node, float y, float z);
void node_scale(DynamicNode *node, float x, float y, float z);
void node_scale_v3(DynamicNode *node, vec3 p);
void node_scale_uniform(DynamicNode *node, float x);

/* MUL */
void node_mul(DynamicNode *node, mat4 m);

/* CENTER */
void node_center(MeshCollection *mc, DynamicNode *node);

/* Graphics */
enum AA_METHOD
{
	AA_NOAA,
	AA_MSAA
};

int key_pressed(int key);

int graphics_init(const char *title, int min_w, int min_h);
void graphics_destroy(void);

void graphics_screenshot(void);

void graphics_vr_mode(int vr);
int graphics_get_vr_mode(void);

void graphics_wireframe(int i);
int graphics_get_wireframe(void);

void graphics_fullscreen(void);
void graphics_windowed(void);
void graphics_mode(u32 n);
int graphics_get_mode(void);

void graphics_event_mode_enable(void);
void graphics_event_mode_disable(void);

void graphics_vsync_enable(void);
void graphics_vsync_disable(void);
void graphics_vsync(u32 n);
int graphics_get_vsync(void);

void graphics_mouse_lock(void);
void graphics_mouse_unlock(void);

void graphics_aa_method(int method);
int graphics_get_aa_method(void);

void graphics_get_selpos_v3(vec3 pos);

void graphics_get_sel(u32 *x, u32 *y, u32 *z);
void graphics_get_selpos(float *x, float *y, float *z);

u32 graphics_vbo_binds(void);
u32 graphics_vao_binds(void);
u32 graphics_texture_binds(void);
u32 graphics_fbo_binds(void);
u32 graphics_draw_calls(void);
u32 graphics_vertices(void);

/* 3D Renderer */
void renderer3d_dsl(float dark, float shadow, float light);
void renderer3d_bind_mesh_collection(MeshCollection *mc);
void renderer3d_bind_shadow_collection(ShadowCollection *sc);
void renderer3d_bind_scene(Scene *scene);
void renderer3d_bind_camera(Camera *camera);
void renderer3d_bind_world(World *world);

/* Skybox Renderer */
void skybox_bind_cubemap(Cubemap cubemap);

/* Readfile */
char *readfile(const char *restrict filename, u32 *restrict length);

/* Recognizer */
#define RECOGNIZER_GESTURE_POINTS 32

typedef struct POINT
{
	vec2 Position;
	int ID;
} RecognizerPoint;

typedef struct POINT_CLOUD
{
	RecognizerPoint Points[RECOGNIZER_GESTURE_POINTS];
} RecognizerGesture;

void gesture_create(RecognizerGesture *pc, RecognizerPoint *points, int length);
int recognizer_classify(RecognizerGesture *gesture, RecognizerGesture *templates, int count);

/* TCP Client */
typedef struct TCP_CLIENT
{
	volatile int run;
	pthread_t thr;
	int sockfd, pos, max;
	char *buf;
	struct sockaddr_in servaddr;
	void (*fnrecv)(const char *);
	void (*fnstatus)(int);
} TCP_Client;

int tcp_client_init(TCP_Client *cli, int max, void (*status)(int), void (*recv)(const char *));
int tcp_client_connect(TCP_Client *cli, const char *ip, int port);
int tcp_client_send(TCP_Client *cli, const char *msg, int len);
void tcp_client_close(TCP_Client *cli);
void tcp_client_destroy(TCP_Client *cli);

/* TCP Server */
typedef struct TCP_SERVER
{
	volatile int run;
	pthread_t thr;
	struct sockaddr_in address;
	int max_clients, *sockets, msocket, max, *pos;
	char *buf;
	void (*fnrecv)(const char *);
	void (*fnconn)(int, int);
} TCP_Server;

int tcp_server_init(TCP_Server *srv, int max_clients, int buf_size,
		void (*conn)(int, int), void (*recv)(const char *));
int tcp_server_open(TCP_Server *srv, int port);
int tcp_server_send(TCP_Server *srv, int client_id, const char *msg, int len);
void tcp_server_sendall(TCP_Server *srv, const char *msg, int len);
void tcp_server_close(TCP_Server *srv);
void tcp_server_destroy(TCP_Server *srv);

/* Timer */
void time_hms(i32 *h, i32 *m, i32 *s);
void time_ymd_hms(i32 *yy, i32 *mm, i32 *dd, i32 *h, i32 *m, i32 *s);

/* Util */
#define arrlen(x) (sizeof(x) / sizeof(*x))

#ifdef __cplusplus
}
#endif

#endif

