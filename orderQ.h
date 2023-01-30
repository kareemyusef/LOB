#include <unordered_map>

struct Order {
    double price_;
    int quantity_;
    int id_;
    Order* next_ = nullptr;
    Order* prev_ = nullptr;

    Order(double price, int quantity, int id): price_(price), quantity_(quantity), id_(id) {}

    Order(const Order& other) {
        price_ = other.price_;
        quantity_  = other.quantity_;
        id_ = other.id_;
        next_ = other.next_;
        prev_ = other.prev_;
    }
};
/**
 * @brief The Order Queue class is a queue with O(1) average find, delete and modify time
 *        We do this with a doubly linked list and an unordered map.
 *        Constraints: if you modify an order quantity, we modify. If you modfy price, we delete and insert
 * 
 */
class OrderQueue {
    public:
        //construct empty Queue
        OrderQueue(): volume_(0), head_(nullptr), tail_(nullptr),  size_(0) {}

        //rule of three: copy constructor
        OrderQueue(const OrderQueue& other); 
        //rule of three: destructor
        ~OrderQueue() { clear(head_); }
        //rule of three: assignment =operator
        const OrderQueue& operator=(const OrderQueue& rhs);

        Order* top() { return head_; }
        Order* back() {return tail_; }
        

        void push(Order* order);

        void remove(int id);

        void modify(int id, int new_quantity);

        int getVolume() { return volume_; }

        int getSize() { return size_; }

        void print();

        void printPtr();


    private:
        
        
        
        /**
         * map from id to order to lookup orders for delete/modify in O(1)
         * volume for analysis
         * head, tail and size for classic doubly linked list
         * 
         */
        std::unordered_map<int, Order*> order_lookup;
        int volume_;
        Order* head_;
        Order* tail_;
        int size_;

        Order* copy(Order* head);
        void clear(Order* head);
};

void OrderQueue::print() {
    Order* curr = head_;
    while (curr) {
        std::cout << curr->price_ << " " << curr->quantity_ << " " << curr->id_  << " " << std::endl;
        curr = curr->next_;
    }
}
void OrderQueue::printPtr() {
    Order* curr = head_;
    while (curr) {
        std::cout << curr << std::endl;
        curr = curr->next_;
    }
}

OrderQueue::OrderQueue(const OrderQueue& other) {
    if (!other.head_) {
        OrderQueue();
        return;
    }
    head_ = new Order(*other.head_);
    Order* me = head_;
    Order* curr = other.head_->next_;
    while (curr != nullptr) {
        Order* newOrder = new Order(*curr);
        me->next_ = newOrder;
        me = me->next_;
        if (curr == other.tail_) {
            tail_ = me;
        }
        curr = curr->next_;
    }
    volume_ = other.volume_;
    size_ = other.size_;
    
}

const OrderQueue& OrderQueue::operator=(const OrderQueue& rhs) {
    if (!rhs.head_) {
        OrderQueue();
        return *this;
    }
    clear(head_);
    head_ = new Order(*rhs.head_);
    Order* me = head_;
    Order* curr = rhs.head_->next_;
    while (curr != nullptr) {
        Order* newOrder = new Order(*curr);
        me->next_ = newOrder;
        me = me->next_;
        if (curr == rhs.tail_) {
            tail_ = me;
        }
        curr = curr->next_;
    }
    volume_ = rhs.volume_;
    size_ = rhs.size_;
    return *this;
}

/**
 * @brief Orders are created with null next and prev. This is purposeful, so that we can set them in insert.
 * 
 * @param order  
 */
void OrderQueue::push(Order* order) {
    //      order
    //    /      |
    //head       tail
    if (!head_) {
        head_ = order;
        tail_ = order;
    }
    //  order1 <-prev  order2
    //   |             |
    // head   ->next  tail
    else if (size_ == 1) {
        tail_ = order;
        head_->next_ = order;
        tail_->prev_ = head_;
    }
    //  order1 <-prev  order2  <-prev  order3
    //   |                               |
    // head   ->next           ->next   tail
    else {
        tail_->next_ = order;
        order->prev_ = tail_;
        tail_ = order;
    }
    size_++;
    volume_ += order->quantity_;

    //insert vs operator[]: 
    //operator[] will replace key with new value if key already exists
    //insert will not replace key if it already exists and instead throws an error.

    //using insert here because the key should never already exist in the map.
    std::pair<int, Order*> pear(order->id_, order);
    order_lookup.insert(pear);

}

void OrderQueue::modify(int id, int new_quantity) {
    if (order_lookup.find(id) == order_lookup.end()) {
        throw std::invalid_argument("Order id not found");
        return;
    }
    volume_ -= order_lookup[id]->quantity_;
    order_lookup[id]->quantity_ = new_quantity;
    volume_ += order_lookup[id]->quantity_;
    return;
}
/**
 * @brief if size = 1, then we will delete the OrderQueue along with removing the price from the limit tree.
 * 
 * @param id 
 */
void OrderQueue::remove(int id) {
    if (order_lookup.find(id) == order_lookup.end()) {
        throw std::invalid_argument("Order id not found");
        return;
    }
    int remove_quantity = order_lookup[id]->quantity_;
    
    if (order_lookup[id] == head_) {
        //size = 2
        if (head_->next_ == tail_) {
            delete tail_->prev_;
            head_ = tail_;
        }
        else {
            Order* tmp = head_->next_;
            delete head_;
            head_ = tmp;
        }
    }
    else if (order_lookup[id] == tail_) {
        //size = 2
        if (tail_->prev_ == head_) {
            delete head_->next_;
            tail_ = head_;
        }
        else {
            Order* tmp = tail_->prev_;
            delete tail_;
            tail_ = tmp;
        }
    }
    else {
        //set prev next to next
        order_lookup[id]->prev_->next_ = order_lookup[id]->next_;
        //set next prev to prev
        order_lookup[id]->next_->prev_ = order_lookup[id]->prev_;
        delete order_lookup[id];
    }
    
    //happens in all cases
    //at this point order is deleted so dont try to access it
    volume_ -= remove_quantity;
    order_lookup.erase(id);
    return;
}

//recursive destrcutor. Should work.
void OrderQueue::clear(Order* head) {
    if (!head) {
        return;
    }
    clear(head->next_);
    delete head;
}
//recursive copy. should also work -> didnt work
// Order* OrderQueue::copy(Order* head) {
//     // if (!head) {
//     //     return nullptr;
//     // }
//     // Order* newOrder = new Order(head->price_, head->quantity_, head->id_);
//     // std::cout << head->id_ << std::endl;
//     // newOrder->prev_ = copy(head->prev_);
//     // newOrder->next_ = copy(head->next_);
//     // return newOrder;
//     Order* curr = head;
//     while (curr != ) {
//         Order* newOrder = new Order(curr->price_, curr->quantity_, curr->id_);
//     }
// }
 