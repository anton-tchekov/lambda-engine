#include "condition.h"
#include <assert.h>

typedef struct CONDITION
{
	int (*Check)(void);
	int Cache;
} Condition;

static u32 _num_cond;
static Condition _cond[512];

u32 condition_action(int (*condition)(void))
{
	_cond[_num_cond].Check = condition;
	_cond[_num_cond].Cache = -1;
	++_num_cond;
	return _num_cond;
}

void condition_reset(void)
{
	u32 i;
	for(i = 0; i < _num_cond; ++i)
	{
		_cond[i].Cache = -1;
	}
}

int condition_get(u32 n)
{
	assert(n != 0);
	--n;
	assert(n < _num_cond);
	if(_cond[n].Cache == -1)
	{
		_cond[n].Cache = _cond[n].Check();
		assert(_cond[n].Cache >= 0);
	}

	return _cond[n].Cache;
}

