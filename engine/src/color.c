#include "color.h"

const struct COLORS COLOR =
{
	{ 1.0, 0.0, 0.0, 1.0 }, /* RED         */
	{ 0.0, 1.0, 0.0, 1.0 }, /* GREEN       */
	{ 0.0, 0.0, 1.0, 1.0 }, /* BLUE        */
	{ 1.0, 1.0, 0.0, 1.0 }, /* YELLOW      */
	{ 0.0, 1.0, 1.0, 1.0 }, /* CYAN        */
	{ 1.0, 0.0, 1.0, 1.0 }, /* MAGENTA     */
	{ 1.0, 1.0, 1.0, 1.0 }, /* WHITE       */
	{ 0.0, 0.0, 0.0, 1.0 }, /* BLACK       */
	{ 0.0, 0.0, 0.0, 0.0 }  /* TRANSPRNT */
};

const struct COLORS8 COLOR8 =
{
	{ 255,   0,   0, 255 }, /* RED         */
	{   0, 255,   0, 255 }, /* GREEN       */
	{   0,   0, 255, 255 }, /* BLUE        */
	{ 255, 255,   0, 255 }, /* YELLOW      */
	{   0, 255, 255, 255 }, /* CYAN        */
	{ 255,   0, 255, 255 }, /* MAGENTA     */
	{ 255, 255, 255, 255 }, /* WHITE       */
	{   0,   0,   0, 255 }, /* BLACK       */
	{   0,   0,   0,   0 }  /* TRANSPRNT */
};

