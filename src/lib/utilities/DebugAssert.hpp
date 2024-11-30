#ifndef NDEBUG
#include <iostream>
#include <exception>
#define ASSERT(condition, message) \
        do { \
            if (!(condition)) { \
                std::cerr << "ASSERT: Assertion `" #condition "` failed in " << __FILE__ \
                            << " line " << __LINE__ << ": " << message << std::endl; \
                std::terminate(); \
            } \
        } while (false)

#define ASSERT_MESSAGE(message) ASSERT(false, message)

#define ENGINE_LOG(message) std::clog << "[log] " << message << std::endl;

#else
#define ASSERT(condition, message) do { } while (false)
#endif