#include "detail/acceptor_impl.h"

#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
    int ret = 0;
    acceptor_impl_t acceptor(NULL);
    ret = acceptor.open("tcp://*:3366");

    cout <<"acceptor open:" << ret <<"\n";
	return 0;
}

