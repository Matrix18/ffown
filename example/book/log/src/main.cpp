#include <iostream>
using namespace std;

#include "detail/task_queue_impl.h"
#include "thread.h"
#include "log.h"

using namespace ff;

int main(int argc, char* argv[])
{
	str_format_t fmt("@@@@@%2ld#####%09d^^%s^^^%08sYYY");
	unsigned long long a = (-1);
	fmt.append(&a);
	fmt.append(222.4554);
	fmt.append("-WWWWW-");
	fmt.append("-TTTT-");
	printf("result:%s\n", fmt.gen_result().c_str());

	//logdebug(("XX", "TTTT:%09s", "abc"));

	LOG.start("-log_class FF,XX -log_print_screen true -log_level 6");
	LOGDEBUG(("XX", "FFFFF"));
	LOGTRACE(("XX", "FFFFF"));
	LOGINFO(("XX", "FFFFF"));
	LOGWARN(("XX", "FFFFF"));
	LOGERROR(("XX", "FFFFF"));
	LOGFATAL(("XX", "FFFFF"));
	LOG.mod_class("TT", true);

	sleep(1);
	LOGFATAL(("TT", "FFFFF"));

	LOGFATAL(("FF", "DSDFFFFF", string("SFWEGGGGGGGGG")));

	LOG.stop();
    return 0;
}
