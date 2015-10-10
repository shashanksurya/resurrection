//Binary Search Tree
#include <stdio.h>
typedef unsigned long ul;

typedef struct{
	struct bst *left;
	struct bst *right;
	struct bst *parent;
	unsigned long int data;
}bst;


ul search(bst *l,ul key){
	if(l == NULL) return NULL;
	if(l->data == key) return l;
	if(key < l->data)
		return search(l->left,key);
	else
		return search(l->right,key);
}

void insert(bst **l,ul key,bst *parent)
{
	bst *node;
	if(*l == NULL)
	{
		node = (struct bst*)malloc(sizeof(struct bst));
		node->data = key;
		node->parent = parent;
		node->left = node->right = NULL;
		*l = node;
		return;
	}
	if(key < (*l)->left)
		insert(&(*l)->left),key,*l);
	else
		insert(&(*l)->right),key,*l);
}

int main(void)
{
	bst *ll;
	insert(&ll,2,ll);
	printf("%s\n","Binary Search Tree" );
	return 1;
}