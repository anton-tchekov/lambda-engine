#include "recognizer.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>

static float distance(RecognizerPoint *p0, RecognizerPoint *p1);
static float cloud_distance(RecognizerPoint *p0, RecognizerPoint *p1, int start_idx);

static float distance(RecognizerPoint *p0, RecognizerPoint *p1)
{
	float dx, dy;
	dx = p1->Position[X] - p0->Position[X];
	dy = p1->Position[Y] - p0->Position[Y];
	return (float)sqrt(dx * dx + dy * dy);
}

static float cloud_distance(RecognizerPoint *p0, RecognizerPoint *p1, int start_idx)
{
	int i, j, idx;
	float sum, dist, min_dist;
	unsigned char matched[RECOGNIZER_GESTURE_POINTS];
	sum = 0.0;
	i = start_idx;
	memset(matched, 0, RECOGNIZER_GESTURE_POINTS);
	do
	{
		idx = -1;
		min_dist = FLT_MAX;
		for(j = 0; j < RECOGNIZER_GESTURE_POINTS; ++j)
		{
			if(!matched[j])
			{
				if((dist = distance(p0 + i, p1 + j)) < min_dist)
				{
					min_dist = dist;
					idx = j;
				}
			}
		}

		matched[idx] = 1;
		sum += (1.0f - (float)((i - start_idx + RECOGNIZER_GESTURE_POINTS) % RECOGNIZER_GESTURE_POINTS) /
				(1.0f * RECOGNIZER_GESTURE_POINTS)) * min_dist;

		i = (i + 1) % RECOGNIZER_GESTURE_POINTS;
	} while(i != start_idx);
	return sum;
}

void gesture_create(RecognizerGesture *pc, RecognizerPoint *points, int length)
{
	{
		/* Resample */
		int i, n;
		float interval, d0, d1, t;
		RecognizerPoint first;
		for(interval = 0.0f, i = 1; i < length; ++i)
		{
			if(points[i].ID == points[i - 1].ID)
			{
				interval += distance(points + i - 1, points + i);
			}
		}

		interval /= RECOGNIZER_GESTURE_POINTS - 1;
		pc->Points[0] = points[0];
		d0 = 0.0f;
		for(n = 1, i = 1; i < length; ++i)
		{
			if(points[i].ID == points[i - 1].ID)
			{
				d1 = distance(points + i - 1, points + i);
				if(d0 + d1 >= interval)
				{
					first = points[i - 1];
					while(d0 + d1 >= interval)
					{
						if(isnan(t = fminf(fmaxf((interval - d0) / d1, 0.0f), 1.0f)))
						{
							t = 0.5;
						}

						pc->Points[n].Position[X] = (1.0f - t) * first.Position[X] + t * points[i].Position[X];
						pc->Points[n].Position[Y] = (1.0f - t) * first.Position[Y] + t * points[i].Position[Y];
						pc->Points[n].ID = points[i].ID,
						d1 = d0 + d1 - interval;
						d0 = 0.0f;
						first = pc->Points[n];
						++n;
					}

					d0 = d1;
				}
				else
				{
					d0 += d1;
				}
			}
		}

		if(n == RECOGNIZER_GESTURE_POINTS - 1)
		{
			pc->Points[n] = points[length - 1];
		}
	}

	{
		/* Scale */
		int i;
		float min_x, max_x, min_y, max_y, size;
		min_x = FLT_MAX;
		max_x = -FLT_MAX;
		min_y = FLT_MAX;
		max_y = -FLT_MAX;
		for(i = 0; i < RECOGNIZER_GESTURE_POINTS; ++i)
		{
			min_x = fminf(min_x, pc->Points[i].Position[X]);
			min_y = fminf(min_y, pc->Points[i].Position[Y]);
			max_x = fmaxf(max_x, pc->Points[i].Position[X]);
			max_y = fmaxf(max_y, pc->Points[i].Position[Y]);
		}

		size = fmaxf(max_x - min_x, max_y - min_y);
		for(i = 0; i < RECOGNIZER_GESTURE_POINTS; ++i)
		{
			pc->Points[i].Position[X] = (pc->Points[i].Position[X] - min_x) / size;
			pc->Points[i].Position[Y] = (pc->Points[i].Position[Y] - min_y) / size;
		}
	}

	{
		/* Translate to Origin */
		int i;
		float x, y;
		x = 0.0f;
		y = 0.0f;
		for(i = 0; i < RECOGNIZER_GESTURE_POINTS; ++i)
		{
			x += pc->Points[i].Position[X];
			y += pc->Points[i].Position[Y];
		}

		x /= RECOGNIZER_GESTURE_POINTS;
		y /= RECOGNIZER_GESTURE_POINTS;
		for(i = 0; i < RECOGNIZER_GESTURE_POINTS; ++i)
		{
			pc->Points[i].Position[X] -= x;
			pc->Points[i].Position[Y] -= y;
		}
	}
}

int recognizer_classify(RecognizerGesture *gesture, RecognizerGesture *templates, int count)
{
	int i, j, step, result;
	float dist, min_mistance = FLT_MAX;
	result = -1;
	for(i = 0; i < count; ++i)
	{
		step = floor(pow(RECOGNIZER_GESTURE_POINTS, 1.0f - 0.5f));
		dist = FLT_MAX;
		for(j = 0; j < RECOGNIZER_GESTURE_POINTS; j += step)
		{
			dist = fminf(dist, fminf(
					cloud_distance(gesture->Points, templates[i].Points, j),
					cloud_distance(templates[i].Points, gesture->Points, j)));
		}

		if(dist < min_mistance)
		{
			min_mistance = dist;
			result = i;
		}
	}

	return result;
}

