#include <iostream>
using namespace std;

#include "detail/task_queue_impl.h"
#include "thread.h"
#include "log.h"

using namespace ff;

int main(int argc, char* argv[])
{
	str_format_t fmt("@@@@@%2ld#####%4d^^^^^%sYYY");
	unsigned long long a = (-1);
	fmt.append(&a);
	fmt.append(222.4554);
	fmt.append("-WWWWW-");
	fmt.append("-TTTT-");
	printf("result:%s\n", fmt.gen_result().c_str());
    return 0;
}
