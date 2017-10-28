#include "test.pb.h"

#include "mtrace/mtrace.h"
#include "mtrace/malloc_counter.h"

#include <rapidjson/document.h>

#include <chrono>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

using namespace rapidjson;

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

template <typename Callable>
void run_benchmark(const std::string& desc, std::size_t iterations, Callable&& callable)
{
    mtrace<malloc_counter> mt;

    auto start = std::chrono::steady_clock::now();

    for (std::size_t i = 0; i < iterations; ++i)
        callable();

    auto end = std::chrono::steady_clock::now();

    double total_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    if (total_time < 1.0)
    {
        total_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        std::cout << desc << ": total_time=" << total_time << "us";
    }
    else
    {
        std::cout << desc << ": total_time=" << total_time << "ms";
    }

    double per_iteration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / double(iterations);
    std::cout << " per_iteration=" << per_iteration << "ns";

    malloc_counter& counter = mt.get<0>();
    std::cout << " --- malloc_calls=" << counter.malloc_calls() << " bytes_allocated=" << (counter.malloc_bytes() / std::size_t(1 << 10)) << "k" << std::endl;
}

int main()
{
    //dump();
    const std::string serialized = from_disk();

    run_benchmark("protobuf deserialization", 1000, [&]()
    {
        test t;
        t.ParseFromString(serialized);

        if (t.str1().size() != 40)
            throw std::runtime_error("failed");
    });

    const std::string json = R"json(
        {
            "str1": "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
            "str2": "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
            "str3": "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
            "str4": "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
            "str5": "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
            "d1": 1.0,
            "d2": 1.0,
            "d3": 1.0,
            "d4": 1.0,
            "d5": 1.0
        }
        )json";

    Document document;

    run_benchmark("rapidjson deserialization", 1000, [&]()
    {
        document.Parse(json.c_str());

        if (document["str1"].GetStringLength() != 40)
            throw std::runtime_error("failed");
    });

    return 0;
}

