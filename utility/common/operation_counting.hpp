#ifndef COMMON_OPERATION_COUNTING_HPP
#define COMMON_OPERATION_COUNTING_HPP

#include <iostream>
#include <map>
#include <initializer_list>

namespace common
{
    struct operation_counter : public std::map<const char *, long long>
    {
        virtual void print()
        {
            for (auto [key, value] : *this)
                std::cout << key << ": " << value << std::endl;
        }

        virtual void print(std::initializer_list<const char *> keys)
        {
            for (const char *key : keys)
                std::cout << key << ": " << (*this)[key] << std::endl;
        }

        void println()
        {
            print();
            std::cout << std::endl;
        }

        void println(std::initializer_list<const char *> keys)
        {
            print(keys);
            std::cout << std::endl;
        }
    };
}

#endif
