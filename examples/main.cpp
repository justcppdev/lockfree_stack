#include <lockfree_stack.hpp>
#include <random>
#include <thread>
#include <iostream>
#include <chrono>

int main()
{
    lockfree_stack_t<int> stack;
    

    std::thread customer{ [ &stack ]() mutable {
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<int> dist(1, 10);

        for( ;; ) {
            int value = dist(mt);
            auto result = stack.pop();
            if ( result ) {
                std::cout << "stack -> " << *result << std::endl;
            }
            std::this_thread::sleep_for( std::chrono::seconds( value ) );
        }
    } };

    std::thread provider{ [ &stack ]() mutable {
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<int> dist(1, 10);

        for( ;; ) {
            int value = dist(mt);
            stack.push( value );
            std::cout << "stack <- " << value << std::endl;
            std::this_thread::sleep_for( std::chrono::seconds( value ) );
        }
    } };

    customer.join();
    provider.join();

    return EXIT_SUCCESS;
}
