#include <iostream>
#include <sstream>
#include <string>
#include "book.h"

using namespace std;


int main(int argc, char** argv)
{
    book b = book();
    b.limit_buy(150, 100);
    b.limit_buy(140, 100);
    b.limit_buy(130, 100);
    b.limit_sell(220, 50);
    b.limit_sell(210, 50);
    b.limit_sell(180, 50);
    b.print_levels(3);
    std::cout << "\n";


    std::cout << "Buying 25 at best market price - should be 180\n";
    double leftover = b.market_buy(25);
    b.print_levels(3);
    std::cout << "Leftover is " << leftover << "\n";
    std::cout << "\n";

    std::cout << "Buying 25 at best market price - should be 180\n";
    leftover = b.market_buy(25);
    b.print_levels(3);
    std::cout << "Leftover is " << leftover << "\n";
    std::cout << "\n";
    
    std::cout << "Buying 100 at best market price should make the new buy price equal to the old ask price\n";
    b.limit_buy(210, 100);
    b.print_levels(3);
    std::cout << "\n";

    return 0;
}
