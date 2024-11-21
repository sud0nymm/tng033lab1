#include "set.hpp"

/*
 * std::size_t is defined in the C++ standard library
 * std::size_t is an unsigned integer type that can store the maximum size of any possible object
 * sizes are non-negative integers -- i.e. unsigned integer type
 */

/* *********** class Node ************ */

// This class is private to class Set
// but all class Node members are public to class Set
class Set::Node {
public:
    // Constructor
    Node(int nodeVal = 0, Node* nextPtr = nullptr) : value{nodeVal}, next{nextPtr} {
        ++count_nodes;
    }

    // Destructor
    ~Node() {
        --count_nodes;
        assert(count_nodes >= 0);  // number of existing nodes can never be negative
    }
    int value;
    Node* next;

    // Total number of existing nodes -- used only to help to detect bugs in the code
    // Cannot be used in the implementation of any member functions
    static std::size_t count_nodes;

    friend std::ostream& operator<<(std::ostream& os, const Set& rhs);
};

/* ************************************ */

// Initialize the counter of the total number of existing nodes
std::size_t Set::Node::count_nodes = 0;

// Used only for debug purposes
// Return number of existing nodes
std::size_t Set::get_count_nodes() {
    return Set::Node::count_nodes;
}

/* *********** class Set member functions ************ */

// Default constructor
Set::Set() : head{new Node{}}, counter{0} {  // create the dummy node
    //
}

// Constructor for creating a singleton {x}
Set::Set(int x) : Set() {
    Node* node = new Node(x);
    head->next = node;
    counter++;
}

// Constructor: create a set with elements
// elements is not sorted and values in it may not be unique
Set::Set(const std::vector<int>& elements) : Set() {
        
    //Node* dummy = new Node(0); // creates a dummy node with value 0
    //head->next = dummy; // makes head point to dummy ndoe

    for (int i : elements) { // starts at 0th element, which we already created, so i is j + 1
        Node* ptr = head;

        while (ptr != nullptr) {
            if (ptr->next == nullptr) {
                Node* theNode = new Node(i);
                ptr->next = theNode; 
                counter++;
            }
            else if (ptr->next->value > i) {
                Node* theNode = new Node(i);
                theNode->next = ptr->next;
                ptr->next = theNode;
                counter++;
            }
            else if (ptr->next->value < i) {
                ptr = ptr->next;
            }
            else if (ptr->next->value == i) {
                break;
            }
        }

    }

}

// copy constructor
Set::Set(const Set& rhs) : Set() {
    Node* rhsptr = rhs.head->next;
    Node* ptr = head;

    while (rhsptr != nullptr) {
        Node* node = new Node(rhsptr->value);
        ptr->next = node;
        ptr = ptr->next;
        rhsptr = rhsptr->next;
        counter++;
    }
}

// Assignment operator: use copy-and-swap idiom
Set& Set::operator=(Set rhs) {
    std::swap(head, rhs.head);
    std::swap(counter, rhs.counter);
    return *this;
}

// Destructor: deallocate all nodes
Set::~Set() {
    Node* ptr = head;

    while (ptr != nullptr) {
        Node* temp = ptr->next;
        delete ptr;
        ptr = temp;
       // std::cout << "destrctor called\n"; // implement destruction later
    }
}

// Return number of elements in the set
std::size_t Set::cardinality() const {
    return counter;  // delete, if needed
}

// Test if set is empty
bool Set::empty() const {
    if (cardinality() == 0) {
        return true;
    }
    else return false;
}

// Test if x is an element of the set
bool Set::member(int x) const {
    Node* ptr = head->next;

    while (ptr != nullptr) {

        if (ptr->value == x) {

            return true;
        }

        ptr = ptr->next;
    }
    return false;  // delete, if needed 
}

// Return true, if *this is a subset of Set b
// Otherwise, false is returned
bool Set::is_subset(const Set& b) const {
    Node* ptr = head->next;

    while (ptr != nullptr) {
        if (!b.member(ptr->value)) {
            return false;
        }
        ptr = ptr->next;
    }

    return true;  // delete, if needed
}

// Return a new Set representing the union of Sets *this and b
// Repeated values are not allowed
// Implement an algorithm similar to the one in exercise 3/Set 1, but don't use vectors
Set Set::set_union(const Set& b) const {

    Node* ptr = head->next;
    Node* rhsptr = b.head->next;

    Set S{};
    Node* sptr = S.head;

    while (rhsptr != nullptr && ptr != nullptr) {

        if (rhsptr->value < ptr->value) {
            sptr->next = new Node(rhsptr->value);
            rhsptr = rhsptr->next;
        }
        else if (ptr->value < rhsptr->value) {
            sptr->next = new Node(ptr->value);
            ptr = ptr->next;
        }
        else if (ptr->value == rhsptr->value) {
            sptr->next = new Node(ptr->value);
            ptr = ptr->next;
            rhsptr = rhsptr->next;
        }
        sptr = sptr->next; 
        S.counter++;

    }

    while (ptr != nullptr) {
        sptr->next = new Node(ptr->value);
        ptr = ptr->next;
        sptr = sptr->next;
        S.counter++;
    }

    while (rhsptr != nullptr) {
        sptr->next = new Node(rhsptr->value);
        rhsptr = rhsptr->next;
        sptr = sptr->next;
        S.counter++;
    }
    
    return S;  // delete, if needed
} 

// Return a new Set representing the intersection of Sets *this and b
Set Set::set_intersection(const Set& b) const {

    Node* ptr = head->next;
    Node* rhsptr = b.head->next;

    Set S{};
    Node* sptr = S.head;

    while (rhsptr != nullptr && ptr != nullptr) {

        if (rhsptr->value > ptr->value) {
            ptr = ptr->next;
        }
        else if (ptr->value > rhsptr->value) {
            rhsptr = rhsptr->next;
        }
        else if (ptr->value == rhsptr->value) {
            sptr->next = new Node(ptr->value);
            ptr = ptr->next;
            rhsptr = rhsptr->next;
            S.counter++;
            sptr = sptr->next;
        }
    }

    return S;
}

// Return a new Set representing the difference between Set *this and Set b
Set Set::set_difference(const Set& b) const {

    Node* ptr = head->next;

    Set S{};
    Node* sptr = S.head;

    while (ptr != nullptr) {
        
        if (!b.member(ptr->value)) {
            sptr->next = new Node(ptr->value);
            sptr = sptr->next;
            S.counter++;
        }

        ptr = ptr->next;   
    }

    return S;
}

std::ostream& operator<<(std::ostream& os, const Set& rhs) {
    if (rhs.empty()) {
        os << "Set is empty!";
    } else {
        Set::Node* ptr = rhs.head->next;
        os << "{ ";

        while (ptr != nullptr) {
            os << ptr->value << " ";
            ptr = ptr->next;
        }
        os << "}";
    }
    return os;
}

/********** Private member functions ************/
