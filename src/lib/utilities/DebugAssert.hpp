#ifndef NDEBUG
#include <iostream>
#include <exception>
#define ASSERT(condition, message) \
        do { \
            if (!(condition)) { \
                std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                            << " line " << __LINE__ << ": " << message << std::endl; \
                std::terminate(); \
            } \
        } while (false)

#define ASSERT_MESSAGE(message) assert(false && message)

#else
#define ASSERT(condition, message) do { } while (false)
#endif