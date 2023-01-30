template <class K, class V>
K AVLTree<K, V>::max() {
    Node* tmp = root;
    while (tmp->right) {
        tmp = tmp->right;
    }
    return tmp->key;
}

template <class K, class V>
K AVLTree<K, V>::min() {
    Node* tmp = root;
    while (tmp->left) {
        tmp = tmp->left;
    }
    return tmp->key;
}
template <class K, class V>
V AVLTree<K, V>::find(const K& key) const
{
    return find(root, key);
}

template <class K, class V>
V AVLTree<K, V>::find(Node* subtree, const K& key) const
{
    if (subtree == NULL)
        return V();
    else if (key == subtree->key)
        return subtree->value;
    else {
        if (key < subtree->key)
            return find(subtree->left, key);
        else
            return find(subtree->right, key);
    }
}

template <class K, class V>
void AVLTree<K, V>::rotateLeft(Node*& t)
{
    Node * temp = t->right;
    t->right = temp->left;

    temp->left = t;
    t->height = 1 + std::max(heightOrNeg1(t->left), heightOrNeg1(t->right));
    //now do the same again
    t = temp;
    t->height = 1 + std::max(heightOrNeg1(t->left), heightOrNeg1(t->right));
}

template <class K, class V>
void AVLTree<K, V>::rotateLeftRight(Node*& t)
{
    rotateLeft(t->left);
    rotateRight(t);
}

template <class K, class V>
void AVLTree<K, V>::rotateRight(Node*& t)
{
    Node* temp = t->left;
    t->left = temp->right;

    temp->right = t;
    t->height = 1 + std::max(heightOrNeg1(t->left), heightOrNeg1(t->right));
    //now do the same again
    t = temp;
    t->height = 1 + std::max(heightOrNeg1(t->left), heightOrNeg1(t->right));
}


template <class K, class V>
void AVLTree<K, V>::rotateRightLeft(Node*& t)
{
    rotateRight(t->right);
    rotateLeft(t);
}


//rebalance() should, given a subtree, rotate the subtree so that it is balanced. 
//You should assume that the subtree’s left and right children are both already balanced trees. 
//The node’s height should always be updated, even if no rotations are required.
template <class K, class V>
void AVLTree<K, V>::rebalance(Node*& subtree)
{
   
    if (subtree == NULL) {
        return;
    }

    int balance = heightOrNeg1(subtree->right) - heightOrNeg1(subtree->left); 
    if (balance == 2) {
        if ((heightOrNeg1(subtree->right->right) - heightOrNeg1(subtree->right->left)) > 0) {  
            rotateLeft(subtree);
        }
        else {  
            rotateRightLeft(subtree);
        }
    }

    else if (balance == -2) {
        if ((heightOrNeg1(subtree->left->right) - heightOrNeg1(subtree->left->left)) < 0) {  
            rotateRight(subtree);
        }
        else {  
            rotateLeftRight(subtree);
        }
    }

    
}

template <class K, class V>
void AVLTree<K, V>::insert(const K & key, const V & value)
{
    insert(root, key, value);
}

template <class K, class V>
void AVLTree<K, V>::insert(Node*& subtree, const K& key, const V& value)
{
    
    if (subtree == NULL) {
        subtree = new Node(key, value);
    }
    else if (key == subtree->key) {
        subtree->value = value;
        return;
    }
    else if (key < subtree->key) {
        insert(subtree->left, key, value);
        rebalance(subtree);
    }
    else if (key > subtree->key) {
        insert(subtree->right, key, value);
        rebalance(subtree);
    }
    subtree->height = 1 + std::max(heightOrNeg1(subtree->left), heightOrNeg1(subtree->right));
}

template <class K, class V>
void AVLTree<K, V>::remove(const K& key)
{
    remove(root, key);
}

template <class K, class V>
void AVLTree<K, V>::remove(Node*& subtree, const K& key)
{
    if (subtree == NULL)
        return;

    if (key < subtree->key) {
        remove(subtree->left, key);
    } 
    else if (key > subtree->key) {
        remove(subtree->right, key);
    } 
    else {
        if (subtree->left == NULL && subtree->right == NULL) {
            /* no-child remove */
            
            delete subtree;
            subtree = NULL;
        } else if (subtree->left != NULL && subtree->right != NULL) {
            /* two-child remove */
            
            Node * placeholder = subtree->left;

            while (placeholder->right != NULL) {
                placeholder = placeholder->right;
            }
            subtree->key = placeholder->key;
            subtree->value = placeholder->value;
            remove(subtree->left, placeholder->key);
        } else {
            /* one-child remove */
            
            Node* temp;
            if(subtree->left != NULL){
              temp = subtree->left;
            }
            else{
              temp = subtree->right;
            }
            std::swap(*temp, *subtree);
            delete temp;
            temp = NULL;
        }
        
    }
    rebalance(subtree);
}
