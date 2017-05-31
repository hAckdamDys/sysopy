#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "contacts.h"
#include "tree.h"
/*
- tworzenie i usuwanie ksiazki kontaktowej 
*/
TreeNode* makeTreeNode(Contact* data){
	TreeNode* n = malloc(sizeof(TreeNode));
	n->parent = NULL;
	n->left = NULL;
	n->right = NULL;
	if(data==NULL){
		n->data=NULL;
	}
	else{
		n->data = makeContact();
		initContact(n->data,data->name,data->surName,data->birthDate,data->email,data->phone,data->address);
	}
	return n;
}
void delTreeNode(TreeNode* n){
	assert(n!=NULL);
	if(n->data!=NULL){
		delContact(n->data);
	}
	free(n);
}
Tree* makeTree(ContactKey key){
	Tree* t = malloc(sizeof(Tree));
	t->root=NULL;
	t->key=key;
	return t;
}
void delNodeAsRoot(TreeNode* r){
	assert(r!=NULL);
	if(r->left!=NULL){
		delNodeAsRoot(r->left);
	}
	if(r->right!=NULL){
		delNodeAsRoot(r->right);
	}
	assert(r->data!=NULL);
	delContact(r->data);
	free(r);
}
void delTree(Tree* t){
	assert(t!=NULL);
	delNodeAsRoot(t->root);
	free(t);
}
/*
- dodanie i usuniecie  kontaktu do/z ksiazki (bez utraty innych kontaktow i bez wyciekow pamieci) 
*/

void treeAddContact(Tree* t,Contact* c){
	TreeNode* n=t->root;
	ContactKey key=t->key;
	if(n==NULL){
		t->root=makeTreeNode(c);
		return;
	}
	while(1){
		if(cmpContacts(n->data,c,key)<=0){
			if(n->right==NULL){
				n->right=makeTreeNode(c);
				n->right->parent=n;
				return;
			}
			n=n->right;
		}
		else{
			if(n->left==NULL){
				n->left=makeTreeNode(c);
				n->left->parent=n;
				return;
			}
			n=n->left;
		}
	}
}
void treeDelContact(Tree* t,Contact* c){
	TreeNode* n=treeFindContactNode(t,c);
	assert(n!=NULL);
	delContact(n->data);
	if(n->left==NULL){/*lewy null*/
		if(n->right==NULL){/*lewy i prawy*/
			if(n!=t->root){
				if(n==n->parent->left){
					n->parent->left=NULL;
				}
				else{
					n->parent->right=NULL;
				}
			}
			else{
				t->root=NULL;
			}
		}
		else{/*tylko lewy */
			if(n==t->root){
				t->root=n->right;
				n->right->parent=NULL;
			}
			else{
				if(n==n->parent->left){
					n->parent->left=n->right;
				}
				else{
					n->parent->right=n->right;
				}
				n->right->parent=n->parent;
			}
		}
		free(n);
		return;
	}
	if(n->right==NULL){/*tylko prawy*/
		if(n==t->root){
			t->root=n->left;
			n->left->parent=NULL;
		}
		else{
			if(n==n->parent->left){
				n->parent->left=n->left;
			}
			else{
				n->parent->right=n->left;
			}
			n->left->parent=n->parent;
		}
		free(n);
		return;
	}
	/* zaden nie jest nullem */
	TreeNode* m=treeNodeMinVal(n->right);/*nastepnik n-a*/
	n->data=m->data;
	m->data=NULL;
	if(m->right==NULL){/*lewy i prawy*/
		free(m);
		return;
	}
	/* tylko z lewej null*/
	if(m==m->parent->left){
		m->parent->left=m->right;
	}
	else{
		m->parent->right=m->right;
	}
	m->right->parent=m->parent;
	free(m);
}
TreeNode* treeNodeMinVal(TreeNode* n){
	assert(n!=NULL);
	if(n->left==NULL){
		return n;
	}
	return treeNodeMinVal(n->left);
}
/*
- wyszukiwanie elementu w ksiazce
*/
TreeNode* treeFindContactNode(Tree* t,Contact* c){
	assert(t!=NULL);
	assert(c!=NULL);
	TreeNode* n=t->root;
	ContactKey key=t->key;
	if(n==NULL){
		return NULL;
	}
	while(1){
		int result=cmpContacts(n->data,c,key);
		if(result==0){
			return n;
		}
		else if(result<0){
			if(n->right==NULL){
				return NULL;
			}
			n=n->right;
		}
		else{
			if(n->left==NULL){
				return NULL;
			}
			n=n->left;
		}
	}
	return NULL;
}
/*
- sortowanie/przebudowanie ksiazki wg wybranego pola (Nazwisko, Data urodzenia, email, telefon)
*/
void treeNodeTraverseAdd(Tree* t,TreeNode* n){
	if(n==NULL){
		return;
	}
	treeNodeTraverseAdd(t,n->left);
	treeAddContact(t,n->data);
	treeNodeTraverseAdd(t,n->right);
}
Tree* treeTraverseRemake(Tree* t1,ContactKey key){
	if(t1->key==key){
		return t1;
	}
	Tree* t2=makeTree(key);
	//treeNodeTraverseAdd(t2,t1->root);
	//delTree(t1);
	return t2;
}
/* extra */
void treeNodeTraversePrint(TreeNode* n){
	if(n==NULL){
		return;
	}
	treeNodeTraversePrint(n->left);
	printContact(n->data);
	treeNodeTraversePrint(n->right);
}
void treeTraversePrint(Tree* t){
	printf("printing tree: \n");
	if(t==NULL){
		return;
	}
	treeNodeTraversePrint(t->root);
	printf("printed tree\n");
}