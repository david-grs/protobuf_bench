#include "test.pb.h"

#include "mtrace/mtrace.h"
#include "mtrace/malloc_counter.h"

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

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

std::string from_disk(const std::string& filename = "test.data")
{
    enum { BufferSize = 1024 };
    char buff[BufferSize];

    std::string str;
    std::ifstream ifs(filename);
    while (std::streamsize bytes = ifs.readsome(buff, BufferSize))
        str.append(buff, bytes);

    return str;
}


int main()
{
    //dump();
    const std::string serialized = from_disk();

    {
        mtrace<malloc_counter> mt;

        malloc_counter& counter = mt.get<0>();
        std::cout << "malloc_calls=" << counter.malloc_calls() << " bytes_allocated=" << (counter.malloc_bytes() / std::size_t(1 << 20)) << "M" << std::endl;

    }

}

