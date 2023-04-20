# Limit Order Book

## Overview
This project helped me answer my questions about how the price of a security moves at a low level. The Limit Order Book builds up and matches orders, simulating a real market for a single security. Participants can either make a Limit Order, which will sit in the book until it is filled, or a Market Order, which immediately takes the best price for the given action. 

## Orders and OrderQueue
The `Order` struct defined in "orderQ.h" represents a single order made by a market participant. Orders are placed in an `OrderQueue` at each price, ensuring that orders placed earlier are filled first. The Order struct contains a `price`,`quantity`, and `id`. The id field is for identification when removing orders from the queue. Each order also contains a `next` and `prev` pointer for the OrderQueue.

The `OrderQueue` class is a FIFO queue of Orders with O(1) lookup time. It is composed of a doubly linked list of Orders, each of which has an id which is added to a map of id to Order address for O(1) lookup. Each OrderQueue represents a price in the book, and the linked list of Orders represents multiple orders at the same price placed in succession. Orders can be added, removed, and have their quantities edited in O(1) time. An OrderQueue has two notable public class functions, `OrderQueue::GetVolume` and `OrderQueue::GetSize` which return the total quantity of the OrderQueue and the total number of orders in the OrderQueue respectively. 

## book class and AVL trees
The `book` class contains two self balancing trees of OrderQueues, sorted by price. The two trees represent the buy side and sell side of the book. This allows for O(log(n)) lookup, insert and delete times. 

When a market participant calls `book::limit_buy` or `book::limit_sell` with a given price and quantity, and Order pointer is returned with the id, price and quantity of the order placed. This Order will be added to the buy/sell side of the book and sit there until a Market Order fills it. 

When a market participant calls `book::market_buy` or `book::market_sell` with a given quantity, the book will fill an opposite action Order at the lowest sell/highest buy with the given quantity. If the Market Order is completely filled, these functions return 0. Otherwise, they return the amount leftover from the Market Order (meaning Orders in this book are **NOT** "*fill or kill*"). 

When a market participant calls `book::ask_price` or `book::bid_price`, they are given the lowest sell price or highest buy price respectively. This book does not use a *midprice*(the average of the bid and ask prices) or *microprice* (weighted average of bid and ask prices by volume). Instead, the bid and ask price are separate, and will return the best price for each side. These are also the prices at which Market Orders are placed. 

The `book::print_levels` function accepts an integer "levels" and will print an In-Order Traversal of the tree by price, printing out the best "levels" prices (if "levels" = 3, then the book prints the best 3 prices on each side). The ask and bid prices are separated by the lowest ask/highest bid. 

## Usage
To simulate orders in the book, I added a few orders to the book and printed out the levels in main.cpp (print levels will print out all levels if the argument passed is greater than the size of the book):
```c++
book b = book();
b.limit_buy(150, 100);
b.limit_buy(140, 100);
b.limit_buy(130, 100);
b.limit_sell(220, 50);
b.limit_sell(210, 50);
b.limit_sell(180, 50);
b.limit_buy(120, 500);

b.print_levels(10);
```
After running `make book` and then `./book` as defined in the Makefile, the terminal shows:
```bash
(base) kareembenaissa@Kareems-MacBook-Pro LOB % ./book   
| 220
| 210
| 180 -  -  -  ask_price
| 150 -  -  -  bid_price
| 140
| 130
| 120
```

I can now place a market buy order. The best buy price is currently 180, with a quantity of 50. 

```c++
std::cout << "Buying 25 at best market price\n";
b.market_buy(25)
b.print_levels(10);
```

```bash
| 220
| 210
| 180 -  -  -  ask_price
| 150 -  -  -  bid_price
| 140
| 130
| 120
```

As you can see, buying 25 at 180 didn't affect the book at all. But what if I buy 25 more at 180?

```c++
std::cout << "Buying 25 at best market price\n";
b.market_buy(25)
b.print_levels(10);
```

```bash
Buying 25 at best market price
| 220
| 210 -  -  -  ask_price
| 150 -  -  -  bid_price
| 140
| 130
| 120
```

As you can see, the ask price has now changed, because the lowest sell was bought out by multiple buy orders. This has effectively increased the price of the security, because the lowest available sell price was cleared by many buys, leaving only the higher sell prices left!

Finally, lets look at what happens when a limit buy is placed that matches a limit sell which is already in the book:

```c++
std::cout << "Buying 100 at best market price\n";
b.limit_buy(210, 100);
b.print_levels(10);
```

```bash
Buying 100 at best market price
| 220 -  -  -  ask_price
| 210 -  -  -  bid_price
| 150
| 140
| 130
| 120
```
Remember that the 210 sell price only had a quantity of 50. When a large limit buy is placed at the best sell price, the buy will wipe out the entire sell price, which will increase the sell price, as seen in the previous example. However, since this was a limit order, the leftover 50 quantity will now be added as the new buy highest buy price. 

## Answering Questions

The above section shows how the price of a security actually moves with *market microstructure*. It can be said that the actual/best price of a security is equal to the lowest ask - the smallest amount someone is willing to sell the security for. If a big buy order comes in, or if lots of people are buying a security all at once, they clear out all the quantities from the low sell orders. This means the only sell orders left are the ones which are very expensive, effectively raising the price of the security. *So, when a lot of people are buying a stock, the price goes up.* 

Conversely, if there is a huge sell order, or if a lot of people are selling a security at once, it will clear out all the highest buy orders. The only buy orders left are the cheap ones, which effectively lowers the price of the security. *So, when a lot of people are selling a stock, the price goes down.*

I knew that when people buy, price goes up, and when people sell, price goes down, but I never knew why. This project helped me understand the low level concepts behind prices and demand. It is important to note that the limit order books in use today for trading stocks are much more complex, and the prices are determined differently than just the lowest ask and highest bid. However, this general concept holds in markets that buy and sell. 

One more thing I learned about was *liquidity*. When the book is filled with many limit orders, that means the security has high liquidity, or that it is possible to make high quantiy market orders. If there are very few limit orders in the book, then it is less likely that a market order will be placed and fully filled at a good, fair price. Limit orders make liquidity, while market orders take liquidity - as you can see from the above section, when many market orders are placed at once, they clear out the limit orders. 


