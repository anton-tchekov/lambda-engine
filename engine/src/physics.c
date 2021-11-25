#include "physics.h"
#include "internal.h"

#define MAX_COLLISION_STEP 0.05f

void player_get_aabb(Player *player, AABB *aabb)
{
	aabb->Min[X] = player->Position[X] - player->Size;
	aabb->Max[X] = player->Position[X] + player->Size;

	aabb->Min[Y] = player->Position[Y] - player->HeightBelowEye;
	aabb->Max[Y] = player->Position[Y] + player->HeightAboveEye;

	aabb->Min[Z] = player->Position[Z] - player->Size;
	aabb->Max[Z] = player->Position[Z] + player->Size;
}

void player_init(Player *player, float vel_move, float accel_jump, float hbe, float hae, float size)
{
	camera_init(&player->Camera, (float)RAD(90.0f), 0.1f, 500.0f);
	camera_update(&player->Camera);
	player->Flags = 0;
	player->VelocityY = 0.0;
	player->VelocityXZ = vel_move;
	player->AccelerationJump = accel_jump;
	player->HeightBelowEye = hbe;
	player->HeightAboveEye = hae;
	player->Size = size;
}

float player_distance(Player *player, vec3 pos)
{
	vec3 distance;
	vec3_sub(distance, player->Position, pos);
	return vec3_len(distance);
}

void player_update(Player *player, u32 keys, float delta_time)
{
	int j;
	AABB player_aabb;
	float velocity_xz;
	u32 i;
	vec3 new_pos, prev_pos;
	World *world = GameEngine.Graphics.Renderer3D.World;

	/* Dynamic Timescaling */
	float maxvx = fabsf(delta_time * player->VelocityXZ);
	float maxvy = fabsf((player->VelocityY - world->Gravity * delta_time) * delta_time);

	float minrepx = ceilf(maxvx * (1.0f / MAX_COLLISION_STEP));
	float minrepy = ceilf(maxvy * (1.0f / MAX_COLLISION_STEP));
	int minrep = (int)fmax(minrepx, minrepy);

	float step_time = delta_time / (float)minrep;

	vec3_dup(prev_pos, player->Position);
	vec3_dup(new_pos, player->Position);

	for(j = 0; j < minrep; ++j)
	{
		velocity_xz = step_time * player->VelocityXZ;

		/* y axis movement */
		if(player->Flags & PLAYER_FLAG_GAMEMODE)
		{
			if(keys & KEY_UP)
			{
				new_pos[Y] += velocity_xz;
			}

			if(keys & KEY_DOWN)
			{
				new_pos[Y] -= velocity_xz;
			}
		}
		else
		{
			if((keys & KEY_JUMP) && (player->Flags & PLAYER_FLAG_GROUNDED))
			{
				player->VelocityY += player->AccelerationJump;
				player->Flags &= ~PLAYER_FLAG_GROUNDED;
			}

			player->VelocityY -= world->Gravity * step_time;
			new_pos[Y] += player->VelocityY * step_time;
		}

		player_aabb.Min[X] = new_pos[X] - player->Size;
		player_aabb.Max[X] = new_pos[X] + player->Size;

		player_aabb.Min[Y] = new_pos[Y] - player->HeightBelowEye;
		player_aabb.Max[Y] = new_pos[Y] + player->HeightAboveEye;

		player_aabb.Min[Z] = new_pos[Z] - player->Size;
		player_aabb.Max[Z] = new_pos[Z] + player->Size;

		if(!(player->Flags & PLAYER_FLAG_GAMEMODE))
		{
			for(i = 0; i < world->NumBoxes; ++i)
			{
				if(aabb_intersect(&player_aabb, world->Boxes + i))
				{
					new_pos[Y] = prev_pos[Y];
					if(player->VelocityY < 0.0f)
					{
						player->Flags |= PLAYER_FLAG_GROUNDED;
					}

					player->VelocityY = 0.0f;
					break;
				}
			}
		}

		/* x axis movement */
		if(keys & KEY_FORWARD)
		{
			new_pos[X] += player->Camera.Front[0] * velocity_xz;
		}

		if(keys & KEY_BACKWARD)
		{
			new_pos[X] -= player->Camera.Front[0] * velocity_xz;
		}

		if(keys & KEY_RIGHT)
		{
			new_pos[X] += player->Camera.Right[0] * velocity_xz;
		}

		if(keys & KEY_LEFT)
		{
			new_pos[X] -= player->Camera.Right[0] * velocity_xz;
		}

		player_aabb.Min[X] = new_pos[X] - player->Size;
		player_aabb.Max[X] = new_pos[X] + player->Size;

		player_aabb.Min[Y] = new_pos[Y] - player->HeightBelowEye;
		player_aabb.Max[Y] = new_pos[Y] + player->HeightAboveEye;

		player_aabb.Min[Z] = new_pos[Z] - player->Size;
		player_aabb.Max[Z] = new_pos[Z] + player->Size;

		if(!(player->Flags & PLAYER_FLAG_GAMEMODE))
		{
			for(i = 0; i < world->NumBoxes; ++i)
			{
				if(aabb_intersect(&player_aabb, world->Boxes + i))
				{
					new_pos[X] = prev_pos[X];
					if(world->Boxes[i].VelocityY && player->Flags & PLAYER_FLAG_GROUNDED)
					{
						player->Flags &= ~PLAYER_FLAG_GROUNDED;
						player->VelocityY = world->Boxes[i].VelocityY;
					}
					break;
				}
			}
		}

		/* z axis movement */
		if(keys & KEY_FORWARD)
		{
			new_pos[Z] += player->Camera.Front[1] * velocity_xz;
		}

		if(keys & KEY_BACKWARD)
		{
			new_pos[Z] -= player->Camera.Front[1] * velocity_xz;
		}

		if(keys & KEY_RIGHT)
		{
			new_pos[Z] += player->Camera.Right[1] * velocity_xz;
		}

		if(keys & KEY_LEFT)
		{
			new_pos[Z] -= player->Camera.Right[1] * velocity_xz;
		}

		player_aabb.Min[X] = new_pos[X] - player->Size;
		player_aabb.Max[X] = new_pos[X] + player->Size;

		player_aabb.Min[Y] = new_pos[Y] - player->HeightBelowEye;
		player_aabb.Max[Y] = new_pos[Y] + player->HeightAboveEye;

		player_aabb.Min[Z] = new_pos[Z] - player->Size;
		player_aabb.Max[Z] = new_pos[Z] + player->Size;

		if(!(player->Flags & PLAYER_FLAG_GAMEMODE))
		{
			for(i = 0; i < world->NumBoxes; ++i)
			{
				if(aabb_intersect(&player_aabb, world->Boxes + i))
				{
					new_pos[Z] = prev_pos[Z];
					if(world->Boxes[i].VelocityY && player->Flags & PLAYER_FLAG_GROUNDED)
					{
						player->Flags &= ~PLAYER_FLAG_GROUNDED;
						player->VelocityY = world->Boxes[i].VelocityY;
					}
					break;
				}
			}
		}

		vec3_dup(prev_pos, new_pos);
	}

	vec3_dup(player->Position, new_pos);
	vec3_dup(player->Camera.Position, player->Position);
}

