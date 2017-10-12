#include <Localenv.h>


namespace core
{
	core::LocalEnv &getLocalEnv()
	{
		static LocalEnv *local_env = new LocalEnv();
		
		return *local_env;
	}
}
