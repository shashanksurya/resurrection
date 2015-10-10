#include <stdio.h>
#include <stdlib.h>
/*BINARY SEARCH TREE -deletion not working*/
typedef unsigned long int ul;
struct bst
{
	ul data;
	struct bst *left;
	struct bst *right;
	struct bst *parent;
};

void inorder(struct bst *l)
{
	if(l!=NULL)
	{
		inorder(l->left);
		printf("%lu ",l->data);
		inorder(l->right);
	}
}

void preorder(struct bst *l)
{
	if(l!=NULL)
	{
		printf("%lu ",l->data);
		inorder(l->left);
		inorder(l->right);
	}
}

void postorder(struct bst *l)
{
	if(l!=NULL)
	{
		inorder(l->left);
		inorder(l->right);
		printf("%lu ",l->data);
	}
}


struct bst* search(struct bst *l,ul key)
{
	if(l==NULL) return NULL;
	if(l->data == key) return l;
	else if(key < l->data) {return search(l->left,key);}
	else {return search(l->right,key);}
}

struct bst* insert(struct bst *l,ul key,struct bst *parent)
{
	struct bst *newnode;
	if(l == NULL)
	{
		newnode = (struct bst*)malloc(sizeof(struct bst));
		newnode->data = key;
		newnode->left = NULL;
		newnode->right = NULL;
		newnode->parent = parent;
	}
	else if(key < l->data)
		l->left = insert(l->left,key,l);
	else
		l->right = insert(l->right,key,l);
	return newnode;
}

struct bst* find_max(struct bst *l)
{
	if(l->right == NULL)
		return l;
	else
		return find_max(l->right);
}

struct bst* find_min(struct bst *l)
{
	if(l->left == NULL)
		return l;
	else
		return find_min(l->left);
}


struct bst* delkey(struct bst *node, int data)
{
        struct bst *temp;
        if(node==NULL)
        {
                printf("Element Not Found");
        }
        else if(data < node->data)
        {
                node->left = delkey(node->left, data);
        }
        else if(data > node->data)
        {
                node->right = delkey(node->right, data);
        }
        else
        {
                /* Now We can delete this node and replace with either minimum element 
                   in the right sub tree or maximum element in the left subtree */
                if(node->right && node->left)
                {
                        /* Here we will replace with minimum element in the right sub tree */
                        temp = find_min(node->right);
                        node -> data = temp->data; 
                        /* As we replaced it with some other node, we have to delete that node */
                        node -> right = delkey(node->right,temp->data);
                }
                else
                {
                        /* If there is only one or zero children then we can directly 
                           remove it from the tree and connect its parent to its child */
                        temp = node;
                        if(node->left == NULL)
                                node = node->right;
                        else if(node->right == NULL)
                                node = node->left;
                        free(temp); /* temp is longer required */ 
                }
        }
        return node;

}


int countNodes (struct bst* root)
{
    if (root == NULL)
     return 0;
    return countNodes (root->left) +
           countNodes (root->right) + 1;
}

void print_options()
{
	printf("------------BINARY SEARCH TREE DEMO---------------\n");
	printf("[1]. Insert a number\n");
	printf("[2]. Delete a number\n");
	printf("[3]. Search a number\n");
	printf("[4]. Find minimum\n");
	printf("[5]. Find maximum\n");
	printf("[6]. Preorder Traversal\n");
	printf("[7]. Inorder Traversal\n");
	printf("[8]. Postorder Traversal\n");
	printf("[9]. Find Parent\n");
	printf("[0]. Exit this program!\n");
	printf("Select your option: ");
}

int main(void)
{
	struct bst *root = NULL;
	struct bst *p = NULL;
root = insert(root,278,NULL);
insert(root,584,NULL);
insert(root,678,NULL);
insert(root,3532,NULL);
insert(root,5374,NULL);
insert(root,5514,NULL);
insert(root,5561,NULL);
insert(root,6106,NULL);
insert(root,6224,NULL);
insert(root,7422,NULL);
insert(root,7426,NULL);
insert(root,7701,NULL);
insert(root,7822,NULL);
insert(root,8240,NULL);
insert(root,9426,NULL);
insert(root,9594,NULL);
insert(root,10320,NULL);
insert(root,11808,NULL);
insert(root,12614,NULL);
insert(root,13595,NULL);
insert(root,14060,NULL);
insert(root,16083,NULL);
insert(root,16825,NULL);
insert(root,17102,NULL);
insert(root,18212,NULL);
insert(root,18803,NULL);
insert(root,19868,NULL);
insert(root,20472,NULL);
insert(root,20556,NULL);
insert(root,21353,NULL);
insert(root,21539,NULL);
insert(root,21836,NULL);
insert(root,22614,NULL);
insert(root,24944,NULL);
insert(root,25649,NULL);
insert(root,26727,NULL);
insert(root,27145,NULL);
insert(root,27540,NULL);
insert(root,28980,NULL);
insert(root,29415,NULL);
insert(root,29854,NULL);
insert(root,30140,NULL);
insert(root,30372,NULL);
insert(root,31084,NULL);
insert(root,33601,NULL);
insert(root,38055,NULL);
insert(root,38096,NULL);
insert(root,39346,NULL);
insert(root,39355,NULL);
insert(root,40196,NULL);
insert(root,41769,NULL);
insert(root,42577,NULL);
insert(root,44782,NULL);
insert(root,46760,NULL);
insert(root,47178,NULL);
insert(root,48849,NULL);
insert(root,49365,NULL);
insert(root,53317,NULL);
insert(root,54061,NULL);
insert(root,54600,NULL);
insert(root,54738,NULL);
insert(root,57178,NULL);
insert(root,57786,NULL);
insert(root,58903,NULL);
insert(root,62787,NULL);
insert(root,63791,NULL);
insert(root,64064,NULL);
insert(root,66062,NULL);
insert(root,66077,NULL);
insert(root,67370,NULL);
insert(root,67459,NULL);
insert(root,68770,NULL);
insert(root,69146,NULL);
insert(root,71992,NULL);
insert(root,72319,NULL);
insert(root,73698,NULL);
insert(root,76523,NULL);
insert(root,76989,NULL);
insert(root,77708,NULL);
insert(root,79073,NULL);
insert(root,80218,NULL);
insert(root,81292,NULL);
insert(root,83222,NULL);
insert(root,84756,NULL);
insert(root,85858,NULL);
insert(root,87540,NULL);
insert(root,92212,NULL);
insert(root,92543,NULL);
insert(root,92584,NULL);
insert(root,93015,NULL);
insert(root,93906,NULL);
insert(root,94191,NULL);
insert(root,94606,NULL);
insert(root,95518,NULL);
insert(root,96458,NULL);
insert(root,97821,NULL);
insert(root,98635,NULL);
insert(root,98829,NULL);
insert(root,99599,NULL);
insert(root,99812,NULL);
	inorder(root);
	printf("\n\n");
	p = find_max(root);
	printf("Max Element in Tree is %lu\n",p->data);
	p = find_min(root);
	printf("Min Element in Tree is %lu\n",p->data);
	printf("%d\n",countNodes(root));
	delkey(root,278);
	inorder(root);
	printf("\n");
	p = find_max(root);
	printf("Max Element in Tree is %lu\n",p->data);
	p = find_min(root);
	printf("Min Element in Tree is %lu\n",p->data);
	printf("%d\n",countNodes(root));
	return 0;
}