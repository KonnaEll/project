#ifndef LSH_FUNCS

struct Hash_Node
{
    int item;
    int ID;
    struct Hash_Node* next;
};

float h_function(int**, int, int);

#endif