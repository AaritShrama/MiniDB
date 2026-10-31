#ifndef MINI_AVL_INDEX_H
#define MINI_AVL_INDEX_H 
#include <cstdint> 
#include <vector> 
using namespace std; 
using AVLRecordId = uint64_t ; 
class AVLIndex{
    private:
    struct Node{
        int key ; 
        vector<AVLRecordId> recordIds ; 
        Node* left ; 
        Node* right ; 
        int height ; 
        explicit Node(int value , AVLRecordId recordId)
         : key(value),recordIds{recordId}, left(nullptr),right(nullptr), height(1) {}
        };
    int height(Node * node){
        if(node==nullptr){
            return 0 ; 
        }
        return node-> height ; 
    }
    int balanceFactor(Node* node){
        if(node==nullptr){
            return 0 ; 
        }
        return height(node->left) - height(node->right) ; 
    }
    void updateHeight(Node* node){
        if(node==nullptr){
            return  ;
        }
        int leftHeght = height(node->left) ; 
        int rightHeight= height(node->right) ; 
        node->height  =1+(leftHeght>rightHeight?leftHeght:rightHeight) ;
    }
    Node * rotateRight(Node* y){
        Node* x= y->left ; 
        Node* subtree = x->right ; 
        x->right= y;  
        y->left=subtree ; 
        updateHeight(y) ; 
        updateHeight(x) ;
        return x ;  
    }
    Node* rotateLeft(Node* x){
        Node *y = x->right ; 
        Node* subtree = y->left ;
        y->left = x ; 
        x->right = subtree ; 
        updateHeight(x) ; 
        updateHeight(y);
        return y  ;
    }
    Node* balance(Node* node){
        if(node==nullptr){
            return nullptr ;
        }
        updateHeight(node) ; 
        int bf = balanceFactor(node) ;
        // ll or lr 
        if(bf>1){
           if(balanceFactor(node->left)<0 ){
            node->left  =rotateLeft(node->left) ;
           }
           return rotateRight(node) ;  
        }
        if(bf<-1){
            if(balanceFactor(node->right)>0){
                node->right = rotateRight(node->right) ;
            }
            return rotateLeft(node) ;
        }
        return node ; 
    }
    Node* insertNode(Node* node , int key , AVLRecordId recordId){
        if(node==nullptr){
            return new Node(key,recordId) ; 
        }
        if(key<node->key){
            node->left =insertNode(node->left,key,recordId); 
        }
        else if(key>node->key){
            node->right=insertNode(node->right,key ,recordId) ;
        }
        else{
            node->recordIds.push_back(recordId) ; 
            return node ; 
        }
        return balance(node) ; 
    }
    Node* eraseNode(Node* node,int key){
            if(node==nullptr){
                return nullptr ; 
            }
            if(key<node->key){
                node->left = eraseNode(node->left,key) ;
            }
            else if(key>node->key){
                node->right=eraseNode(node->right,key) ; 
            }
            else{
                if(node->left==nullptr){
                    Node* rightChild=node->right ;
                    delete node ; 
                    return rightChild ; 
                }
                if(node->right==nullptr){
                    Node* leftchild =node->left ;
                    delete node ;
                    return leftchild ;
                }
                Node* successor = findMinimum(node->right) ;
                node->key = successor->key ;
                node->recordIds=successor->recordIds ; 
                node->right=eraseNode(node->right , successor->key) ; 
            }
            return balance(node) ; 
    }
    Node* findNode(Node* node,int key) const{
        if(node==nullptr || node->key==key){
            return node ; 
        }
        if(key<node->key){
            return findNode(node->left,key)  ;
        }
        return findNode(node->right,key)  ; 
    }
    void inorderTraversal(Node* node,vector<int>&keys) const{
        if(node==nullptr){
            return ;
        }
        inorderTraversal(node->left,keys) ; 
        keys.push_back(node->key);
        inorderTraversal(node->right,keys);
    }
    Node* findMinimum(Node* node){
        while(node != nullptr && node->left !=nullptr){
            node=node->left ; 
        }
        return node ; 
    }
  
    Node *root = nullptr ; 

    void clear(Node* node) {
        if (node == nullptr) {
            return;
        }
        clear(node->left);
        clear(node->right);
        delete node;
    }

    public:
    AVLIndex()=default  ;
    ~AVLIndex(){
        clear(root);
    }
    AVLIndex(const AVLIndex&) = delete;
    AVLIndex& operator=(const AVLIndex&) = delete;
    void insert(int key, AVLRecordId recordId){
        root=insertNode(root,key,recordId) ;
    }
    vector<AVLRecordId> find(int key) const{
        Node* node =findNode(root,key) ;
        if(node==nullptr){
            return{} ;
        }
        return node->recordIds ; 
    }
    vector<int> getSortedKeys() const {
        vector<int> keys ; 
        inorderTraversal(root,keys) ;
        return keys ;     }

    void eraseKey(int key) {
        root = eraseNode(root,key) ; 
    }
    bool erase(int key , AVLRecordId recordId){
        Node* node=findNode(root,key);
        if(node==nullptr){
            return false ;
        }
        for(auto it = node->recordIds.begin() ; it != node->recordIds.end(); ++it){
            if(*it==recordId){
                node->recordIds.erase(it); 
                if(node->recordIds.empty()){
                    root=eraseNode(root,key) ; 

                }
                return true ; 
            }
           }
           return false ; 
} ;
};
#endif 