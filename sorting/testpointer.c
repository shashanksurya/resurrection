#include <stdio.h>
#include <stdlib.h>
int main(void)
{
	int *x = (int *)malloc(sizeof(int));
	int *y;
	*x = 5;
	y=x;
	//free(x);
	printf("%d\n",*y);

}

void delkey(struct bst *l,ul key)
{
	printf("Deleting %lu\n", key);
	struct bst *p = search(l,key);
	struct bst *temp;
	ul x;
	if(!p)
	{
		printf("No such key!\n");
	}
	else if(p->left==NULL && p->right==NULL)
	{//NO CHILDREN
		
		if(p->parent->data < p->data)
			p->parent->right = NULL;
		else
			p->parent->left = NULL;
		p->parent=NULL;
		free(p);
	}
	else if(p->left!=NULL && p->right!=NULL)
	{//TWO CHILDREN
		
		temp = find_min(p->right);
		printf("Min Found %lu\n", temp->data);
		x = temp->data;
		delkey(l,x);
		p->data = x;
	}
	else
	{//ONE CHILD
		if(!p->left)
		{
			if(p->parent->data < p->data)
				p->parent->right = p->right;
			else
				p->parent->left = p->right;		
			//Assign new parent
			p->right->parent = p->parent;
		}
		else
		{
			if(p->parent->data < p->data)
				p->parent->right = p->left;
			else
				p->parent->left = p->left;
			//Assign new parent				
			p->left->parent = p->parent;
		}
		p->left = NULL;
		p->right= NULL;
		p->parent=NULL;
		free(p);
	}
}

while(1)
	{
		print_options();
		switch(getchar())
		{
			case '1':
			printf("Enter the number you wish to enter\n");
			scanf("%lu",&somenum);
			printf("Given No %lu\n",somenum);
			if(root)
			{
				if(search(root,somenum))
				{
					printf("Key already present, duplicates not allowed!\n");
					break;
				}
			p = insert(root,somenum,NULL);
			printf("Inserted %lu\n",p->data);
			}
			else
				root = insert(root,somenum,NULL);
			break;

			case '2':
			printf("Enter the number you wish to delete\n");
			scanf("%lu",&somenum);
			delkey(root,somenum);
			break;

			case '3':
			printf("Enter the number you wish to search\n");
			scanf("%lu",&somenum);
			p = search(root,somenum);
			if(p)
				printf("Key Found\n");
			else
				printf("Key Not Found\n");
			break;

			case '4':
			p = find_min(root);
			printf("Min. Element Found: %lu\n",p->data);
			break;

			case '5':
			p = find_max(root);
			printf("Max. Element Found: %lu\n",p->data);
			break;

			case '6':
			printf("Preorder Traversal\n");
			preorder(root);
			printf("\n");
			break;

			case '7':
			printf("Inorder Traversal\n");
			inorder(root);
			printf("\n");
			break;

			case '8':
			printf("Postorder Traversal\n");
			postorder(root);
			printf("\n");
			break;

			case '9':
			printf("Enter the number for which you want to find parent\n");
			scanf("%lu",&somenum);
			p = search(root,somenum);
			if(p)
			{
				if(p->parent == NULL)
					printf("The element you tried to search is root\n");
				else
					printf("The element's parent is %lu\n",p->parent->data);
			}
			else
				printf("The key is non-existent, so is its parent!\n");
			break;
			
			case '0':
			printf("I do not have feelings but I wish to express you Thank You and Have a good day\n");
			//free(root);
			//free(p);
			exit(0);
			break;
			
			default:
			printf("Incorrect Option\n");
			break;

		}getchar(); //to fix temporarily the key press of enter after choosing option
	}