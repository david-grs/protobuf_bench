#include "test.pb.h"

#include <fstream>
#include <string>

void dump()
{
    test t;

    t.set_str1(std::string(40, 'a'));
    t.set_str2(std::string(40, 'b'));
    t.set_str3(std::string(40, 'c'));
    t.set_str4(std::string(40, 'd'));
    t.set_str5(std::string(40, 'e'));

    t.set_d1(1.0);
    t.set_d2(1.0);
    t.set_d3(1.0);
    t.set_d4(1.0);
    t.set_d5(1.0);
    t.ByteSize();

    std::ofstream ofs("test.data");
    t.SerializePartialToOstream(&ofs);
}

int main()
{
    dump();
}

