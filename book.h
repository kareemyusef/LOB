#include <iostream>
#include <sstream>
#include <string>
#include <unordered_set>
#include "avltree.h"
#include "orderQ.h"


class book {
    
    
    AVLTree<double, OrderQueue* > buy_side;
    AVLTree<double, OrderQueue* > sell_side;

    
    std::unordered_set<double> buy_prices;
    std::unordered_set<double> sell_prices;

    int id_counter = 0;


    int fill_buy(double price, int quantity);
    int fill_sell(double price, int quantity);
    

    
    public:
        Order* limit_buy(double price, int quantity);
        Order* limit_sell(double price, int quantity);
        int market_buy(int quantity);
        int market_sell(int quantity);

        double ask_price() { return sell_side.min(); }
        double bid_price() { return  buy_side.max(); }
        void print_levels(int levels);
        

        
};

void book::print_levels(int levels) {
    std::vector<double> buy_vec = buy_side.getInorderTraversal();
    std::vector<double> sell_vec = sell_side.getInorderTraversal();
    for (uint i = 0; i < sell_vec.size(); i++) {
        std::cout << "| " << sell_vec[sell_vec.size() - i - 1];
        if (i == sell_vec.size() - 1) {
            std::cout << " -  -  -  ask_price\n";
        }
        else {
            std::cout << "\n";
        }
    }
    for (uint i = 0; i < buy_vec.size(); i++) {
        std::cout << "| " << buy_vec[buy_vec.size() - i - 1];
        if (i == 0) {
            std::cout << " -  -  -  bid_price\n";
        }
        else {
            std::cout << "\n";
        }
    }
}

//returns order if quantity was more than the current price volume
//return NULL if quantity was filled by current price volume
Order* book::limit_buy(double price, int quantity) {
    //if price exists in sell side, then fill_sell, and limit_buy the leftover
    
    if (sell_prices.find(price) != sell_prices.end()) {
        int leftover = fill_sell(price, quantity);
        if (leftover != 0) {
            return limit_buy(price, quantity);
        }
        return NULL;
    }
    
    //since price is not currenctly being sold, we have to make a new buy order
    Order* o = new Order(price, quantity, id_counter);
    id_counter++; 
    //if price exists in buy, then add it to buy's queue
    if (buy_prices.find(price) != buy_prices.end()) {
        OrderQueue *target = buy_side.find(price);
        target->push(o);
        return o;
    }
    //price doesn't exist in buy, make a new queue and at it to the buy_side tree and add price to prices
    OrderQueue *q = new OrderQueue();
    q->push(o);
    buy_side.insert(price, q);
    buy_prices.insert(price);
    return o;
}

//returns order if quantity was more than the current price volume
//return NULL if quantity was filled by current price volume
Order* book::limit_sell(double price, int quantity) {
    //if price exists in buy side, then fill_buy, and limit_sell the leftover
    if (buy_prices.find(price) != buy_prices.end()) {
        int leftover = fill_buy(price, quantity);
        if (leftover != 0) {
            return limit_sell(price, quantity);
        }
        return NULL;
    }
    //since price is not currenctly being bought, we have to make a new sell order
    Order* o = new Order(price, quantity, id_counter);
    id_counter++; 
    //if price exists in sell, then add it to sell's queue
    if (sell_prices.find(price) != sell_prices.end()) {
        OrderQueue *target = sell_side.find(price);
        target->push(o);
        return o;
    }
    //price doesn't exist in sell, make a new queue and at it to the sell_side tree and add price to prices
    OrderQueue* q = new OrderQueue();
    q->push(o);
    sell_side.insert(price, q);
    sell_prices.insert(price);
    return o;
}


int book::fill_buy(double price, int quantity) {
    OrderQueue *q = buy_side.find(price);
    if (quantity == q->getVolume()) {
        buy_side.remove(price);
        buy_prices.erase(price);
        return 0;
    }
    if (quantity > q->getVolume()) {
        double leftover = quantity - q->getVolume();
        buy_side.remove(price);
        buy_prices.erase(price);
        return leftover;
    }
    //if quantity is less than volume then fill whole buy order with this queue
    Order* tmp = q->top();
    while (quantity >= tmp->quantity_) {
        quantity -= tmp->quantity_;
        q->remove(tmp->id_); //pop
        tmp = q->top();
    }
    //leftover or 0
    if (quantity != 0) {
        q->modify(tmp->id_, tmp->quantity_ - quantity);
    }
    return 0;

}

int book::fill_sell(double price, int quantity) {
    OrderQueue* q = sell_side.find(price);
    if (quantity == q->getVolume()) {
        std::cout << "here\n";
        sell_side.remove(price);
        sell_prices.erase(price);
        return 0;
    }
    if (quantity > q->getVolume()) {
        double leftover = quantity - q->getVolume();
        sell_side.remove(price);
        sell_prices.erase(price);
        return leftover;
    }
    //if quantity is less than volume then fill whole sell order with this queue
    Order* tmp = q->top();
    while (quantity >= tmp->quantity_) {
        quantity -= tmp->quantity_;
        q->remove(tmp->id_); //pop
        tmp = q->top();
    }
    //leftover or 0
    if (quantity != 0) {
        q->modify(tmp->id_, tmp->quantity_ - quantity);
    }
    return 0;
}
//for now return -1 if too big
int book::market_buy(int quantity) {
    double best_buy = sell_side.min();
    int leftover = fill_sell(best_buy, quantity);
    return leftover;
}

int book::market_sell(int quantity) {
    double best_sell = buy_side.max();
    int leftover = fill_sell(best_sell, quantity);
    return leftover;
}


