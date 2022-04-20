#define SHOW_STEP 0
#define BALANCING 1

#include <stdlib.h> // malloc, rand
#include <stdio.h>
#include <time.h> // time
#include <string.h> //strcmp, strdup

#define max(x, y)	(((x) > (y)) ? (x) : (y))

////////////////////////////////////////////////////////////////////////////////
// AVL_TREE type definition
typedef struct node
{
	char		*data;
	struct node	*left;
	struct node	*right;
	int			height;
} NODE;

typedef struct
{
	NODE	*root;
	int		count;  // number of nodes
} AVL_TREE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a AVL_TREE head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
AVL_TREE *AVL_Create( void);

/* Deletes all data in tree and recycles memory
*/
void AVL_Destroy( AVL_TREE *pTree);
static void _destroy( NODE *root);

/* Inserts new data into the tree
	return	1 success
			0 overflow
*/
int AVL_Insert( AVL_TREE *pTree, char *data);

/* internal function
	This function uses recursion to insert the new data into a leaf node
	return	pointer to new root
*/
static NODE *_insert( NODE *root, NODE *newPtr);

static NODE *_makeNode( char *data);

/* Retrieve tree for the node containing the requested key
	return	address of data of the node containing the key
			NULL not found
*/
char *AVL_Retrieve( AVL_TREE *pTree, char *key);

/* internal function
    Retrieve node containing the requested key
    return    address of the node containing the key
            NULL not found
*/
static NODE *_retrieve( NODE *root, char *key);

/* Prints tree using inorder traversal
*/
void AVL_Traverse( AVL_TREE *pTree);
static void _traverse( NODE *root);

/* Prints tree using inorder right-to-left traversal
*/
void printTree( AVL_TREE *pTree);
/* internal traversal function
*/
static void _infix_print( NODE *root, int level);

/* internal function
	return	height of the (sub)tree from the node (root)
*/
static int getHeight( NODE *root);

/* internal function
	Exchanges pointers to rotate the tree to the right
	updates heights of the nodes
	return	new root
*/
static NODE *rotateRight( NODE *root);

/* internal function
	Exchanges pointers to rotate the tree to the left
	updates heights of the nodes
	return	new root
*/
static NODE *rotateLeft( NODE *root);

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	AVL_TREE *tree;
	char str[1024];
	
	if (argc != 2)
	{
		fprintf( stderr, "Usage: %s FILE\n", argv[0]);
		return 0;
	}
	
	// creates a null tree
	tree = AVL_Create();
	
	if (!tree)
	{
		fprintf( stderr, "Cannot create tree!\n");
		return 100;
	}

	FILE *fp = fopen( argv[1], "rt");
	if (fp == NULL)
	{
		fprintf( stderr, "Cannot open file! [%s]\n", argv[1]);
		return 200;
	}

	while(fscanf( fp, "%s", str) != EOF)
	{

#if SHOW_STEP
		fprintf( stdout, "Insert %s>\n", str);
#endif		
		// insert function call
		AVL_Insert( tree, str);

#if SHOW_STEP
		fprintf( stdout, "Tree representation:\n");
		printTree( tree);
#endif
	}
	
	fclose( fp);
	
#if SHOW_STEP
	fprintf( stdout, "\n");

	// inorder traversal
	fprintf( stdout, "Inorder traversal: ");
	AVL_Traverse( tree);
	fprintf( stdout, "\n");

	// print tree with right-to-left infix traversal
	fprintf( stdout, "Tree representation:\n");
	printTree(tree);
#endif
    fprintf( stdout, "Height of tree: %d\n", tree->root->height);
    fprintf( stdout, "# of nodes: %d\n", tree->count);
	
	// retrieval
	char *key;
	fprintf( stdout, "Query: ");
	while( fscanf( stdin, "%s", str) != EOF)
	{
        key = AVL_Retrieve( tree, str);
		
		if (key) fprintf( stdout, "%s found!\n", key);
		else fprintf( stdout, "%s NOT found!\n", str);
		
		fprintf( stdout, "Query: ");
	}
	
	// destroy tree
	AVL_Destroy( tree);

	return 0;
}


/* Allocates dynamic memory for a AVL_TREE head node and returns its address to caller
    return    head node pointer
            NULL if overflow
*/
AVL_TREE *AVL_Create( void){
    AVL_TREE* newAvl = (AVL_TREE*)malloc(sizeof(AVL_TREE));
    if(newAvl == NULL) return NULL;
    newAvl->count = 0;
    return newAvl;
}

/* Deletes all data in tree and recycles memory
*/
void AVL_Destroy( AVL_TREE *pTree){
    if(pTree){
        _destroy( pTree->root);
    }
    free( pTree);
}
static void _destroy( NODE *root){
    if(root == NULL) return;
    _destroy(root->left);
    _destroy(root->right);
    free(root->data);
    free(root);
}

/* Inserts new data into the tree
    return    1 success
            0 overflow
*/
int AVL_Insert( AVL_TREE *pTree, char *data){
    NODE* temp = _makeNode( data);
    if(temp == NULL) return 0;
    else{
        pTree->root = _insert(pTree->root, temp);
        (pTree->count)++;
    }
    return 1;
}

/* internal function
    This function uses recursion to insert the new data into a leaf node
    return    pointer to new root
*/
static NODE *_insert( NODE *root, NODE *newPtr){
    int idx;
    
    if(root == NULL){
        root = newPtr;
        return root;
    }else{
        idx = strcmp( newPtr->data, root->data);
        if( idx < 0){ // newPtr->data < root->data
            root->left = _insert( root->left, newPtr);
            root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
            if( (getHeight(root->left) - getHeight(root->right)) > 1 ){
                if( (getHeight(root->left->left) > getHeight(root->left->right)) ){
                    root = rotateRight( root);
                }else{
                    root->left = rotateLeft( root->left);
                    root = rotateRight( root);
                }
            }
        }else{
            root->right = _insert( root->right, newPtr);
            root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
            if( (getHeight(root->right) - getHeight(root->left)) > 1 ){
                if( (getHeight(root->right->right) > getHeight(root->right->left)) ){
                    root = rotateLeft( root);
                }else{
                    root->right = rotateRight( root->right);
                    root = rotateLeft( root);
                }
            }
        }
    }
    return root;
}

static NODE *_makeNode( char *data){
    NODE* newNode = (NODE*)malloc(sizeof(NODE));
    if(newNode == NULL) return NULL;
    else{
        newNode->left = NULL;
        newNode->right = NULL;
        newNode->data = strdup( data);
        newNode->height = 1;
    }
    return newNode;
}


/* Retrieve tree for the node containing the requested key
    return    address of data of the node containing the key
            NULL not found
*/
char *AVL_Retrieve( AVL_TREE *pTree, char *key){
    if(!pTree->root) return NULL;
    if(_retrieve( pTree->root, key) == NULL) return NULL;
    return (_retrieve( pTree->root, key))->data;
}

/* internal function
    Retrieve node containing the requested key
    return    address of the node containing the key
            NULL not found
*/
static NODE *_retrieve( NODE *root, char *key){
    NODE *cur;
    NODE *tmp = NULL;
    cur = root;
    
    while(cur!=NULL){
        if( strcmp(cur->data, key) > 0){
            cur = cur->left;
        }
        else if( strcmp(cur->data, key) < 0){
            cur = cur->right;
        }
        else{
            tmp = cur;
            break;
        }
    }
    return tmp;
}

/* Prints tree using inorder traversal
*/
void AVL_Traverse( AVL_TREE *pTree){
    _traverse( pTree->root);
    return;
}
static void _traverse( NODE *root){
    if(root == NULL) return;
    else{
        _traverse(root->left);
        printf("%s ", root->data);
        _traverse(root->right);
    }
    return;
}

/* Prints tree using inorder right-to-left traversal
*/
void printTree( AVL_TREE *pTree){
    _infix_print( pTree->root, 0);
    return;
}
/* internal traversal function
*/
static void _infix_print( NODE *root, int level){
    int i;
    if(root == NULL) return;
    _infix_print(root->right, level+1);
    for(i=0; i<level; i++){
        printf("\t");
    }
    printf("%s\n", root->data);
    _infix_print(root->left, level+1);
    return;
}

/* internal function
    return    height of the (sub)tree from the node (root)
*/
static int getHeight( NODE *root){
    if(root == NULL) return 0;
    return root->height;
}

/* internal function
    Exchanges pointers to rotate the tree to the right
    updates heights of the nodes
    return    new root
*/
static NODE *rotateRight( NODE *root){
    NODE* temp = root->left;
    root->left = temp->right;
    temp->right = root;
    root = temp;
    if( (root->left != NULL) && (root->right != NULL) ){
        root->right->height = root->height - 1;
    }else{
        (root->height)++;
        root->right->height = root->height - 1;
    }
    return root;
}

/* internal function
    Exchanges pointers to rotate the tree to the left
    updates heights of the nodes
    return    new root
*/
static NODE *rotateLeft( NODE *root){
    NODE* temp = root->right;
    root->right = temp->left;
    temp->left = root;
    root = temp;
    if( (root->left != NULL) && (root->right != NULL) ){
        root->left->height = root->height - 1;
    }else{
        (root->height)++;
        root->left->height = root->height - 1;
    }
    return root;
}

