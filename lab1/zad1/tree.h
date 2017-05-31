#ifndef TREE_H
#define TREE_H

#include <stdlib.h>

#include "contacts.h"
typedef struct TreeNode TreeNode;
struct TreeNode{
	TreeNode* parent;
	TreeNode* left;
	TreeNode* right;
	Contact* data;
};

typedef struct Tree{
	TreeNode* root;
	ContactKey key;
}Tree;


/*
- tworzenie i usuwanie ksiazki kontaktowej 
*/
TreeNode* makeTreeNode(Contact* data);
void delTreeNode(TreeNode* n);
Tree* makeTree(ContactKey key);
void delNodeAsRoot(TreeNode* r);
void delTree(Tree* t);
/*
- dodanie i usuniecie  kontaktu do/z ksiazki (bez utraty innych kontaktow i bez wyciekow pamieci) 
*/
void treeAddContact(Tree* t,Contact* c);
TreeNode* treeNodeMinVal(TreeNode* n);
void treeDelContact(Tree* t,Contact* c);
/*
- wyszukiwanie elementu w ksiazce
*/
TreeNode* treeFindContactNode(Tree* t,Contact* c);
/*
- sortowanie/przebudowanie ksiazki wg wybranego pola (Nazwisko, Data urodzenia, email, telefon)
*/
void treeNodeTraverseAdd(Tree* t,TreeNode* n);
Tree* treeTraverseRemake(Tree* t1,ContactKey key);
/* extra */
void treeNodeTraversePrint(TreeNode* n);
void treeTraversePrint(Tree* t);
#endif