#include "big_number.h"

int main() {
    big_num b0 = 97;
    big_num b1 = 138;
    big_num b2 = 253;
    std::cout << b0 << std::endl;
    std::cout << b1 << std::endl;
    std::cout << b2 << std::endl;
    std::cout << b0 + b1 << std::endl;
    std::cout << b0 * b1 << std::endl;
    std::cout << b0 * b1 * b2 << std::endl;
    std::cout << b0 * b1 * b2 + 19 << std::endl;

    return 0;
}

