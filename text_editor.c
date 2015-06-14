#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BLOCKSIZE 256   //arbitary value


// this is line number(index)
typedef int key_t;

typedef struct node {
	key_t key;
	//char *object;
	struct node *left;
	struct node *right;
	int weight;
} text_t;


text_t *free_list = NULL;
text_t *currentblock = NULL;
int size_left;
int line_number = 1;


//creates an empty text, whose length is 0.
text_t *get_node() {

	text_t *tmp;
	if (free_list != NULL) {
		tmp = free_list;
		free_list = free_list->left;
	} else {
		if (currentblock == NULL || size_left == 0) {
			currentblock = (text_t *) malloc( BLOCKSIZE * sizeof(text_t));
			size_left = BLOCKSIZE;
		}
		tmp = currentblock++;
		size_left -= 1;
		//printf("address of this tmp%p\n",tmp);
	}
	return (tmp);

}


text_t *create_text(void) {
	text_t *tmp_node;
	tmp_node = get_node();
	tmp_node->left = NULL;
	return (tmp_node);
}

int length_text(text_t *txt) {
	if (txt->left == NULL) {
		return 0;
	} else {
		return (txt->weight);
	}

}


void return_node(text_t *node) {
	node->left = free_list;
	free_list = node;
}

void left_rotation(text_t *n) {
	text_t *tmp_node;
	key_t tmp_key;
	tmp_node = n->left;
	tmp_key = n->key;
	n->left = n->right;
	n->key = n->right->key;
	n->right = n->left->right;
	n->left->right = n->left->left;
	n->left->left = tmp_node;
	n->left->key = tmp_key;
}

void right_rotation(text_t *n) {
	text_t *tmp_node;
	key_t tmp_key;
	tmp_node = n->right;
	tmp_key = n->key;
	n->right = n->left;
	n->key = n->left->key;
	n->left = n->right->left;
	n->right->left = n->right->right;
	n->right->right = tmp_node;
	n->right->key = tmp_key;
}


#define ALPHA 0.288
#define EPSILON 0.005

int insert_tree(text_t *tree, char *new_object, key_t new_key) {
	text_t *tmp_node;
	text_t * path_stack[100];
	int path_st_p = 0;
	tmp_node = tree;
	while (tmp_node->right != NULL) {
		path_stack[path_st_p++] = tmp_node;
		if (new_key < tmp_node->key)
			tmp_node = tmp_node->left;
		else
			tmp_node = tmp_node->right;
	}
	/* found the candidate leaf. Test whether key distinct */
	if (tmp_node->key == new_key)
		return (-1);
	/* key is distinct, now perform the insert */
	{
		text_t *old_leaf, *new_leaf;
		old_leaf = get_node();
		old_leaf->left = tmp_node->left;
		old_leaf->key = tmp_node->key;
		old_leaf->right = NULL;
		old_leaf->weight = 1;
		new_leaf = get_node();
		new_leaf->left = (text_t *) new_object;
		new_leaf->key = new_key;
		new_leaf->right = NULL;
		new_leaf->weight = 1;
		if (tmp_node->key < new_key) {
			tmp_node->left = old_leaf;
			tmp_node->right = new_leaf;
			tmp_node->key = new_key;
		} else {
			tmp_node->left = new_leaf;
			tmp_node->right = old_leaf;
		}
		tmp_node->weight = 2;
	}
	/* rebalance */
	while (path_st_p > 0) {
		tmp_node = path_stack[--path_st_p];
		tmp_node->weight = tmp_node->left->weight + tmp_node->right->weight;
		if (tmp_node->right->weight < ALPHA * tmp_node->weight) {
			if (tmp_node->left->left->weight
					> (ALPHA + EPSILON) * tmp_node->weight) {
				right_rotation(tmp_node);
				tmp_node->right->weight = tmp_node->right->left->weight
						+ tmp_node->right->right->weight;
			} else {
				left_rotation(tmp_node->left);
				right_rotation(tmp_node);
				tmp_node->right->weight = tmp_node->right->left->weight
						+ tmp_node->right->right->weight;
				tmp_node->left->weight = tmp_node->left->left->weight
						+ tmp_node->left->right->weight;
			}
		} else if (tmp_node->left->weight < ALPHA * tmp_node->weight) {
			if (tmp_node->right->right->weight
					> (ALPHA + EPSILON) * tmp_node->weight) {
				left_rotation(tmp_node);
				tmp_node->left->weight = tmp_node->left->left->weight
						+ tmp_node->left->right->weight;
			} else {
				right_rotation(tmp_node->right);
				left_rotation(tmp_node);
				tmp_node->right->weight = tmp_node->right->left->weight
						+ tmp_node->right->right->weight;
				tmp_node->left->weight = tmp_node->left->left->weight
						+ tmp_node->left->right->weight;
			}
		}

	}

	return (0);

}



void append_line(text_t *txt, char *new_line) {
	int result;
	char *appended_line; // just a pointer to show appended line
	if (txt->left == NULL) {
		// set index(key) as 1 and next as new_line
		// go to next node and set it next as NULL
		txt->key = line_number;
		txt->left = (text_t *) new_line;
		txt->weight = 1;
		txt->right = NULL;
		//printf("added first word at line %d - ", line_number);
		appended_line = new_line;
/*
		while (*appended_line)
			printf("%c", *appended_line++);
		printf("\n");
*/
	} else {

		line_number++;
		result = insert_tree(txt, new_line, line_number);
		if (result == 0) {

			//printf("successfully appended newline at line number %d - ",
			//		line_number);
			appended_line = new_line;
/*
			while (*appended_line)
				printf("%c", *appended_line++);
			printf("\n");
*/

		} else {
			printf("failed to append\n");
		}

	}
}


void insert_line(text_t *tree, int index, char *new_line) {

	text_t *tmp_node;
	text_t * path_stack[100];
	int path_st_p = 0;


	int intial_index=index;
	tmp_node = tree;

	// stores only the root node.


	while (index!=1 || tmp_node->weight!=1) {
		path_stack[path_st_p++] = tmp_node;
		if (index <= tmp_node->left->weight) {

			//printf("goes to the left <----\n");
			tmp_node=tmp_node->left;

		} else {
			//printf("goes to the right---->\n");
			index= index - tmp_node->left->weight;
			tmp_node=tmp_node->right;
		}

	}


	        text_t *old_leaf, *new_leaf;
	         old_leaf = get_node();
	         old_leaf->left = tmp_node->left;
	         old_leaf->key = tmp_node->key;
	         old_leaf->right  = NULL;
	         old_leaf->weight = 1;
	         new_leaf = get_node();
	         new_leaf->left = (text_t *) new_line;
	         new_leaf->key = intial_index;
	         new_leaf->right  = NULL;
	         new_leaf->weight = 1;

	         tmp_node->left  = new_leaf;
	         tmp_node->right = old_leaf;

	         tmp_node->key=intial_index+1;
	         tmp_node->right->key=intial_index+1;
	         tmp_node->weight = 2;

	/* rebalance */
	while (path_st_p > 0) {
		tmp_node = path_stack[--path_st_p];
		tmp_node->weight = tmp_node->left->weight + tmp_node->right->weight;
		if (tmp_node->right->weight < ALPHA * tmp_node->weight) {
			if (tmp_node->left->left->weight
					> (ALPHA + EPSILON) * tmp_node->weight) {
				right_rotation(tmp_node);
				tmp_node->right->weight = tmp_node->right->left->weight
						+ tmp_node->right->right->weight;
			} else {
				left_rotation(tmp_node->left);
				right_rotation(tmp_node);
				tmp_node->right->weight = tmp_node->right->left->weight
						+ tmp_node->right->right->weight;
				tmp_node->left->weight = tmp_node->left->left->weight
						+ tmp_node->left->right->weight;
			}
		} else if (tmp_node->left->weight < ALPHA * tmp_node->weight) {
			if (tmp_node->right->right->weight
					> (ALPHA + EPSILON) * tmp_node->weight) {
				left_rotation(tmp_node);
				tmp_node->left->weight = tmp_node->left->left->weight
						+ tmp_node->left->right->weight;
			} else {
				right_rotation(tmp_node->right);
				left_rotation(tmp_node);
				tmp_node->right->weight = tmp_node->right->left->weight
						+ tmp_node->right->right->weight;
				tmp_node->left->weight = tmp_node->left->left->weight
						+ tmp_node->left->right->weight;
			}
		}

	}





}


char *get_line( text_t *txt, int index){

	text_t *tmp_node;
	tmp_node = txt;


	// stores only the root node.


	while (index!=1 || tmp_node->weight!=1) {
		if (index <= tmp_node->left->weight) {

			//printf("goes to the left <----\n");
			tmp_node=tmp_node->left;

		} else {
			//printf("goes to the right---->\n");
			index= index - tmp_node->left->weight;
			tmp_node=tmp_node->right;
		}

	}

if(index==1 && tmp_node->weight==1)
	return ((char *)tmp_node->left);
else
	return NULL;


}

char * delete_line(text_t *txt, int index) {

	char *deleted_object;
	text_t *path_stack[100];
	int path_st_p = 0;
	text_t *tmp_node, *upper_node, *other_node;
	tmp_node = txt;

	while (index != 1 || tmp_node->weight != 1) {
		path_stack[path_st_p++] = tmp_node;
		upper_node = tmp_node;
		if (index <= tmp_node->left->weight) {
			//printf("goes to the left <----\n");
			tmp_node = tmp_node->left;
			tmp_node = upper_node->left;
			other_node = upper_node->right;


		} else {
			//printf("goes to the right---->\n");
			index = index - tmp_node->left->weight;
			tmp_node = tmp_node->right;
			tmp_node = upper_node->right;
			other_node = upper_node->left;

		}

	}


	upper_node->key = other_node->key;
	upper_node->left = other_node->left;
	upper_node->right = other_node->right;
	upper_node->weight = other_node->weight;
	deleted_object = (char *) tmp_node->left;
	return_node(tmp_node);
	return_node(other_node);

	/*start rebalance*/
	path_st_p -= 1;
	while (path_st_p > 0) {
		tmp_node = path_stack[--path_st_p];
		tmp_node->weight = tmp_node->left->weight + tmp_node->right->weight;
		if (tmp_node->right->weight < ALPHA * tmp_node->weight) {
			if (tmp_node->left->left->weight
					> (ALPHA + EPSILON) * tmp_node->weight) {
				right_rotation(tmp_node);
				tmp_node->right->weight = tmp_node->right->left->weight
						+ tmp_node->right->right->weight;
			} else {
				left_rotation(tmp_node->left);
				right_rotation(tmp_node);
				tmp_node->right->weight = tmp_node->right->left->weight
						+ tmp_node->right->right->weight;
				tmp_node->left->weight = tmp_node->left->left->weight
						+ tmp_node->left->right->weight;
			}
		} else if (tmp_node->left->weight < ALPHA * tmp_node->weight) {
			if (tmp_node->right->right->weight
					> (ALPHA + EPSILON) * tmp_node->weight) {
				left_rotation(tmp_node);
				tmp_node->left->weight = tmp_node->left->left->weight
						+ tmp_node->left->right->weight;
			} else {
				right_rotation(tmp_node->right);
				left_rotation(tmp_node);
				tmp_node->right->weight = tmp_node->right->left->weight
						+ tmp_node->right->right->weight;
				tmp_node->left->weight = tmp_node->left->left->weight
						+ tmp_node->left->right->weight;
			}
		}
	}
	/*end rebalance*/
	return (deleted_object);





}


char *set_line( text_t *txt, int index, char * new_line)
{

	char *previous_line;
	text_t *tmp_node;
	tmp_node = txt;


	// stores only the root node.


	while (index!=1 || tmp_node->weight!=1) {
		if (index <= tmp_node->left->weight) {

			//printf("goes to the left <----\n");
			tmp_node=tmp_node->left;

		} else {
			//printf("goes to the right---->\n");
			index= index - tmp_node->left->weight;
			tmp_node=tmp_node->right;
		}

	}

if(index==1 && tmp_node->weight==1){

	previous_line=(char *) tmp_node->left;
	tmp_node->left=(text_t *) new_line;


	return previous_line;
}
else
	return NULL;


}



void check_tree(text_t *tr, int depth, int lower, int upper) {
	if (tr->left == NULL) {
		printf("Tree Empty\n");
		return;
	}
	if (tr->key < lower || tr->key >= upper)
		printf("Wrong Key Order \n");
	if (tr->right == NULL) {
		char *ptr = ((char *) tr->left);

		while (*ptr)
			printf("%c", *(ptr)++);
		printf("%d(%d)  ", tr->key, depth);
	} else {
		check_tree(tr->left, depth + 1, lower, tr->key);
		check_tree(tr->right, depth + 1, tr->key, upper);
	}
}



int main() {

	int i, tmp; text_t *txt1, *txt2, *txt3; char *c;
	   printf("starting \n");
	   txt1 = create_text();
	   txt2 = create_text();
	   txt3 = create_text();
	   append_line(txt1, "line one" );

	   if( (tmp = length_text(txt1)) != 1)
	      {  printf("Test 1: length should be 1, is %d\n", tmp); exit(-1);
	      }

	   append_line(txt1, "line hundred" );


	   insert_line(txt1, 2, "line ninetynine" );
	   insert_line(txt1, 2, "line ninetyeight" );
	   insert_line(txt1, 2, "line ninetyseven" );
	   insert_line(txt1, 2, "line ninetysix" );
	   insert_line(txt1, 2, "line ninetyfive" );

	   for( i = 2; i <95; i++ )
	        insert_line(txt1, 2, "some filler line between 1 and 95" );
	      if( (tmp = length_text(txt1)) != 100)
	      {  printf("Test 2: length should be 100, is %d\n", tmp); exit(-1);
	      }

	      printf("found at line 1:   %s\n",get_line(txt1,  1));
	         printf("found at line 2:   %s\n",get_line(txt1,  2));
	         printf("found at line 99:  %s\n",get_line(txt1, 99));
	         printf("found at line 100: %s\n",get_line(txt1,100));
	         for(i=1; i<=10000; i++)
	            {  if( i%2==1 )
	                 append_line(txt2, "A");
	               else
	                 append_line(txt2, "B");
	            }
	            if( (tmp = length_text(txt2)) != 10000)
	            {  printf("Test 3: length should be 10000, is %d\n", tmp); exit(-1);
	            }



 c = get_line(txt2, 9876 );
    if( *c != 'B')
      {  printf("Test 4: line 9876 of txt2 should be B, found %s\n", c); exit(-1);
    }

    for( i= 10000; i > 1; i-=2 )
      {  c = delete_line(txt2, i);
         if( *c != 'B')
         {  printf("Test 5: line %d of txt2 should be B, found %s\n", i, c); exit(-1);
         }
         append_line( txt2, c );
      }

    for( i=1; i<= 5000; i++ )
       {  c = get_line(txt2, i);
          if( *c != 'A')
          {  printf("Test 6: line %d of txt2 should be A, found %s\n", i, c); exit(-1);
          }
       }



    for( i=1; i<= 5000; i++ )
        delete_line(txt2, 1 );
      for( i=1; i<= 5000; i++ )
      {  c = get_line(txt2, i);
         if( *c != 'B')
         {  printf("Test 7: line %d of txt2 should be B, found %s\n", i, c); exit(-1);
         }
      }



      set_line(txt1, 100, "the last line");

      for( i=99; i>=1; i-- )
           delete_line(txt1, i );
         printf("found at the last line:   %s\n",get_line(txt1,  1));

         for(i=0; i<1000000; i++)
              append_line(txt3, "line" );
            if( (tmp = length_text(txt3)) != 1000000)
            {  printf("Test 8: length should be 1000000, is %d\n", tmp); exit(-1);
            }
            for(i=0; i<500000; i++)
              delete_line(txt3, 400000 );
            if( (tmp = length_text(txt3)) != 500000)
            {  printf("Test 9: length should be 500000, is %d\n", tmp); exit(-1);
            }
            printf("End of tests\n");





	return (0);
}
