#ifndef DeadCode_H
#define DeadCode_H

#define DeadCode() \
{ \
	static int DeadVar{ 0 }; \
    static int _DeadVar_{ 0 }; \
 \
\
	if (!DeadVar && _DeadVar_) \
	{ \
		ConMsg(""); \
	} \
 \
}

#endif