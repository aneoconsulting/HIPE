//@HIPE_LICENSE@
#include <corefilter/tools/Localenv.h>


namespace corefilter
{
	corefilter::LocalEnv &getLocalEnv()
	{
		static LocalEnv *local_env = new LocalEnv();
		
		return *local_env;
	}
}
