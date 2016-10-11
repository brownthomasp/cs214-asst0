#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Keeping all the following struct/function definitions here for ease of readability instead of keeping them in a header file.

//Defines a node structure with pointers to left and right children for a red black tree implementation.
typedef struct RBtreeNode {
	char *word;
	char color; //r or b. Anything else is invalid.
	struct RBtreeNode *parent;
	struct RBtreeNode *left;
	struct RBtreeNode *right;
} node;

//Returns the word contained at a given node "n".
char* getWord(node *n) {
	if (n == NULL) {
		return NULL;
	}

	return n->word;
}

//Changes the word associated with a given node "n" to the string contained in *word.
void setWord(node *n, char *word) {
	if (n != NULL) {
		n->word = word;
	}
}

//Returns the color of the node "n".
char getColor(node *n) {
	if (n == NULL) {
		return '\0';
	}

	return n->color;
}

//Sets the color for a given node "n" to the specified color. Colors other than r (red) or b (black) are silently rejected.
void setColor(node *n, char color) {
	if (n != NULL && (color == 'r' || color == 'b'))  {
		n->color = color;
	}
}

//Retuns the parent node of the specified child.
node* getParent(node *child) {
	if (child == NULL) {
		return NULL;
	}

	return child->parent;
}

//Sets the parent node of the specified child to the specified parent.
void setParent(node *child, node *parent) {
	if (child != NULL) {
		child->parent = parent;
	}
}

//Retruns the left child of the specified parent node.
node* getLeftChild(node *parent) {
	if (parent == NULL) {
		return NULL;
	}
	
	return parent->left;
}

//Sets the left child of the specified parent node to the specified child node.
void setLeftChild(node *parent, node *child) {
	if (parent != NULL) {
		parent->left = child;
	}
}

//Returns the right child of the specified parent node.
node* getRightChild(node *parent) {
	if (parent == NULL) {
		return NULL;
	}

	return parent->right;
}

//Sets the right child of the specified parent node to the specified child node.
void setRightChild(node *parent, node *child) {
	if (parent != NULL) {
		parent->right = child;
	}
}

/*
 * The following two functions node *getUncle and node *getGrandparent rely on a struct member "parent", which holds a pointer to some node's parent node.
 * Both functions could have been accomplished using a simple O(n) search algorithm without requiring an extra n nodes * 8 bytes (size of a 64 bit pointer)  memory; however,
 * I reason that the memory footprint of this struct is already reasonably small and I am choosing to optimize for CPU time rather than memory.
 */

//Returns the grandparent node of the specified child node, where the grandparent node is the child's parent's parent node.
node* getGrandparent(node *child) {
	if (child != NULL) {
		if (child->parent != NULL) {
			return child->parent->parent;
		}
	}

	return NULL;
}

//Returns the uncle node of the specified child node, where the uncle node is the child's parent's sibling node. 
node* getUncle(node *child) {
	node *grandparent = getGrandparent(child);

	if (grandparent != NULL) {
		if (child->parent == grandparent->left) {
			return grandparent->right;
		}
		
		return grandparent->left;
	}

	return NULL;
	
}

//Mallocs memory for a new node and automatically colors it red.
node* makeNode(char *word, node *parent) {
	node *newNode = malloc(sizeof(node));
	setWord(newNode, word);
	setColor(newNode, 'r');
	setParent(newNode, parent);
	setLeftChild(newNode, NULL);
	setRightChild(newNode, NULL);
	
	return newNode;
}

//Frees memory associated with a given node "n".
void recycleNode(node *n) {
	if (n != NULL) {
		//free(getWord(n));
		free(n);
	}
}

//Frees all memory used by nodes associated with a tree whose root is "root".
void recycleTree(node *root) {
	if (root == NULL) {
		return;
	}

	//Tree is recycled bottom up.
	recycleTree(getLeftChild(root));
	recycleTree(getRightChild(root));
	recycleNode(root);
}

//Performs a red-black tree left rotation and returns the root of the tree after the rotation is completed.
node* leftRotate(node *root, node *n) {
	node *m = getRightChild(n);
	
	setRightChild(n, getLeftChild(m));

	if (getLeftChild(m) != NULL) {
		setParent(getLeftChild(m), n);
	}

	setParent(m, getParent(n));

	if (getParent(n) == NULL) {
		root = m;
	} else {
		if (n == getLeftChild(getParent(n))) {
			setLeftChild(getParent(n), m);	
		} else {
			setRightChild(getParent(n), m);
		}
	}

	setLeftChild(m, n);

	setParent(n, m);

	return root;
}

//Performs a red-black tree right rotation and returns the root of the tree after the rotation is completed.
node* rightRotate(node *root, node *n) {
	node *m = getLeftChild(n);

	setLeftChild(n, getRightChild(m));

	if (getRightChild(m) != NULL) {
		setParent(getRightChild(m), n);
	}	

	setParent(m, getParent(n));

	if (getParent(n) == NULL) {
		root = m;
	} else {
		if (n == getRightChild(getParent(n))) {
			setRightChild(getParent(n), m);
		} else {
			setLeftChild(getParent(n), m);
		}
	}

	setRightChild(m, n);

	setParent(n, m);

	return root;
}

//Inserts a new node into the tree, or creates a new root node if one does not exist.
node* insert(node *root, char *word) {
	node *ptr = root
            ,*parent = NULL
            ,*uncle = NULL
            ,*grandparent = NULL;

	int cmp = 0;	

	//Peform a standard binary search tree insertion.
	if (root == NULL) {
		root = makeNode(word, NULL);
	}

	//Ditto.
	while (ptr != NULL) { 
		parent = ptr;

		cmp = strcmp(word, getWord(ptr));

		if (cmp == 0) {
			return root;
		} else if (cmp < 0) {
			ptr = getLeftChild(ptr);
		} else {
			ptr = getRightChild(ptr);
		}
	}

	if (cmp < 0) {
		setLeftChild(parent, makeNode(word, parent));
		ptr = getLeftChild(parent);
	} else {
		setRightChild(parent, makeNode(word, parent));
		ptr = getRightChild(parent);
	}

	//Checks the red-black tree for validity and restructures it if this tree has violated any red-black tree proprerties.
	while (ptr != root && getColor(parent) == 'r') {
		uncle = getUncle(ptr);
		grandparent = getGrandparent(ptr);

		if (uncle == getRightChild(grandparent)) {
			if (getColor(uncle) == 'r') {
				setColor(parent, 'b');
				setColor(uncle, 'b');
				setColor(grandparent, 'r');	
				ptr = grandparent;
			} else {
				 if (ptr == getRightChild(parent)) {
					ptr = parent;
					root = leftRotate(root, ptr);
				}

				setColor(parent, 'b');
				setColor(grandparent, 'r');
				root = rightRotate(root, grandparent);
			}
		} else {
			if (getColor(uncle) == 'r') {
				setColor(parent, 'b');
				setColor(uncle, 'b');
				setColor(grandparent, 'r');
				ptr = grandparent;
			} else {
				if (ptr == getLeftChild(parent)) {
					ptr = parent;
					root = rightRotate(root, ptr);
				}

				setColor(parent, 'b');
				setColor(grandparent, 'r');
				root = leftRotate(root, grandparent);
			}
		}
	}

	setColor(root, 'b'); 	

	return root;
}

//Prints the contents of a tree with root node "root"  in sorted order.
void printTree(node *root) {
	if (root == NULL) {
		return;
	}

	printTree(getLeftChild(root));
	printf("%s\n", getWord(root));
	printTree(getRightChild(root));
}


int main(int argc, char **argv) {
	node *root = NULL;
	char *newWord = NULL;
	int inputLength = 0
           ,wordLength = 0
           ,i = 0;

	//Should be exactly 2 arguments. All other cases are errors which will be caught by this conditional.	
	if (argc != 2) {
		printf("Invalid number of arguments (%d) provided.\n", argc - 1);
		return -1;
	}
	
//	printf("---START DEBUG INFO---\n");

	//Get length of input argument/length of all valid substrings plus delimiters.
	inputLength = strlen(argv[1]);

	//Iterate over the input argument.
	while (i < inputLength) {
		while (isalpha(argv[1][i]) && i < inputLength) {
			wordLength++;
			i++;
		}

		if (wordLength != 0) { 
			newWord = malloc(wordLength + 1);	
			memcpy(newWord, &argv[1][i - wordLength], wordLength);
			root = insert(root, newWord);
			newWord = NULL;
		}

		wordLength = 0;
		i++;
	} 

//	printf("---END DEBUG INFO---\n");


	printTree(root);
	recycleTree(root);

	return 0;
}
