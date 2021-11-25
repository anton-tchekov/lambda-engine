#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include "types.h"
#include "vector.h"
#include "aabb.h"
#include "camera.h"

typedef struct PLAYER
{
	u32 Flags;
	Camera Camera;
	vec3 Position;
	float VelocityY, VelocityXZ, AccelerationJump,
			HeightAboveEye, HeightBelowEye, Size,
			WalkAnimation;
} Player;

#define KEY_FORWARD          0x01U
#define KEY_BACKWARD         0x02U
#define KEY_RIGHT            0x04U
#define KEY_LEFT             0x08U
#define KEY_UP               0x10U
#define KEY_DOWN             0x20U
#define KEY_JUMP             0x40U

#define PLAYER_FLAG_GAMEMODE 0x01U
#define PLAYER_FLAG_GROUNDED 0x02U

void player_get_aabb(Player *player, AABB *aabb);
float player_distance(Player *player, vec3 pos);
void player_init(Player *player, float vel_move, float accel_jump, float hbe, float hae, float size);
void player_update(Player *player, u32 keys, float delta_time);
float aabb_size_x(AABB *a);
float aabb_size_y(AABB *a);
float aabb_size_z(AABB *a);

#endif

