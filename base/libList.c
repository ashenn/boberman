#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libList.h"

/**
 * Initialize A list
 * @return ListManager
 */
ListManager* initListMgr(){	
	ListManager* lstMgr = malloc(sizeof(ListManager));
	if (lstMgr == NULL){
		return NULL;
	}

	lstMgr->lastId = 0;
	lstMgr->nodeCount = 0;
	lstMgr->first = NULL;
	lstMgr->last = NULL;

	return lstMgr;
}

/**
 * Add a Node
 * @param lstMgr List Targer
 * @param params Node Key
 * @return Node*
 */
void* addNode(ListManager* lstMgr, void* params){
	int id;
	char* name = (char*) params;
	// printf("New Node Name %s\n", name);
	Node* newNode = malloc(sizeof(Node));
	if (newNode == NULL){
		printf("\n### Fail to alloc Node###\n");
		return NULL;
	}

	id = ++lstMgr->lastId;
	newNode->id = id;
	newNode->lstMgr = lstMgr;
	newNode->name = name;
	newNode->value = NULL;

	if (!lstMgr->nodeCount) // Is the first node
	{
		lstMgr->first = newNode;
		lstMgr->last = newNode;
		newNode->prev = NULL;
		newNode->next = NULL;
	}
	else{
		newNode->next = NULL;
		newNode->prev = lstMgr->last;

		lstMgr->last->next = newNode;
		lstMgr->last = newNode;
	}

	lstMgr->nodeCount++;
	return newNode;
}

/**
 * Add a Node whith value
 * @param lstMgr List Targer
 * @param params Node Key
 * @param value      Node Value
 * @param valueAlloc Value is malloc 
 * @return	Node*
 */
void* addNodeV(ListManager* lstMgr, void* params, void* value, short valueAlloc){
	Node* n = addNode(lstMgr, params);
	if (n == NULL) {
		return NULL;
	}

	n->value = value;
	n->valIsAlloc = valueAlloc;
	return n;
}

/**
 * Print Node details
 * @param node [description]
 */
void printNode(Node* node){
	if (node == NULL)
	{
		printf("node for printNode is NULL\n");
		return;
	}

	printf("id: %d \n", node->id);
	printf("name: %s \n", node->name);
	

	if (node->value == NULL)
	{
		printf("no Value Set\n");
	}
	else{
		printf("Value is Set\n");

		if (node->valIsAlloc){
			printf("Value is Alloc\n");
		}
		else {
			printf("Value is NOT Alloc\n");
		}
	}

}

/**
 * Print List Content
 * @param lstMgr [description]
 */
void printNodes(ListManager* lstMgr){
	Node* currentNode;
	currentNode = NULL;

	int i;
	for (i = 0; i < lstMgr->nodeCount; ++i)
	{
		if (currentNode == NULL)
		{
			currentNode = lstMgr->first;
		}
		else{
			currentNode = currentNode->next;
		}

		printNode(currentNode);
		printf("\n");
	}
}

/**
 * Get Node By Id
 * @param  lstMgr List
 * @param  id     Id
 * @return        Node*
 */
Node* getNode(ListManager* lstMgr, int id){
	Node* currentNode = NULL;

	int i;
	for (i = 0; i < lstMgr->nodeCount; ++i)
	{
		if (currentNode == NULL)
		{
			currentNode = lstMgr->first;
		}
		else{
			currentNode = currentNode->next;
		}

		if (currentNode->id == id)
		{
			return currentNode;
		}
	}

	return NULL;
}

/**
 * Get Node By Key
 * @param  lstMgr List
 * @param  name   Key
 * @return        Node*
 */
Node* getNodeByName(ListManager* lstMgr, char* name){
	Node* currentNode = NULL;

	if (strlen(name) == 0)
	{
		printf("## Error: name param for getNode is invalid\n");
		return NULL;
	}

	int i;
	for (i = 0; i < lstMgr->nodeCount; ++i)
	{
		if (currentNode == NULL)
		{
			currentNode = lstMgr->first;
		}
		else{
			currentNode = currentNode->next;
		}

		if (!strcmp(currentNode->name, name))
		{
			return currentNode;
		}
	}

	// printf("## Error: Node with name: %s not found", name);

	return NULL;
}

/**
 * Delete Node By Name
 * @param lstMgr List
 * @param id     Id
 */
void deleteNodeByName(ListManager* lstMgr, char* name) {
	Node* n = getNodeByName(lstMgr, name);
	if (n == NULL) {
		return;
	}

	deleteNode(lstMgr, n->id);
}

/**
 * Delete Node By Name
 * @param lstMgr List
 * @param id     Id
 */
Node* deleteNodeByNameNoFree(ListManager* lstMgr, char* name) {
	Node* n = getNodeByName(lstMgr, name);
	if (n == NULL) {
		return NULL;
	}

	return deleteNodeNoFree(lstMgr, n->id);
}

/**
 * Delete Node By Id
 * @param lstMgr List
 * @param id     Id
 */
Node* deleteNodeNoFree(ListManager* lstMgr, int id){
	Node* node = getNode(lstMgr, id);
	if (node == NULL){
		return NULL;
	}

	Node* prev = node->prev;
	Node* next = node->next;

	if (prev != NULL && next != NULL){
		prev->next = next;
		next->prev = prev;
	}

	if (lstMgr->first == node){
		lstMgr->first = next;
	}

	if (lstMgr->last == node){
		lstMgr->last = prev;
	}

	if (--lstMgr->nodeCount == 1){
		lstMgr->first->next = NULL;
		lstMgr->first->prev = NULL;
	}
	else if (lstMgr->nodeCount == 0){
		lstMgr->first = NULL;
		lstMgr->first = NULL;
	}

	return node;
}

/**
 * Delete Node By Id
 * @param lstMgr List
 * @param id     Id
 */
void* deleteNode(ListManager* lstMgr, int id){
	Node* node = deleteNodeNoFree(lstMgr, id);
	if (node == NULL) {
		return NULL;
	}

	if (node->valIsAlloc){
		free(node->value);
	}

	free(node);

	return NULL;
}

/**
 * Set Node Value
 * @param  node    Node
 * @param  value   Value
 * @param  asAlloc Value Is Alloc
 * @return         1
 */
int setValue(Node* node, void* value, short asAlloc){
	if (node->value != NULL){
		free(node->value);
	}

	node->value = value;
	node->valIsAlloc = asAlloc;
	return 1;
}

/**
 * Get Array Of Node Ids
 * @param  lstMgr List
 * @param  ids    int[]
 * @return        int*
 */
int* getIds(ListManager* lstMgr, int* ids){
	if (lstMgr->nodeCount <= 0){
		return NULL;
	}

	int i=0;
	Node* currentNode = NULL;

	do{
		if (currentNode == NULL){
			currentNode = lstMgr->first;
		}
		else{
			currentNode = currentNode->next;
		}

		ids[i] = currentNode->id;
				
		i++;
	}while(lstMgr->last != currentNode);

	return ids;
}


/**
 * Clear List
 * @param lstMgr List
 */
void* clearList(ListManager* lstMgr){
	int i;
	int ids[lstMgr->nodeCount];

	getIds(lstMgr, ids);

	for (i = 0; lstMgr->nodeCount > 0; ++i){
		deleteNode(lstMgr, ids[i]);
	}
}


/**
 * Delete List
 * @param lstMgr List
 */
void deleteList(ListManager* lstMgr){
	clearList(lstMgr);
	free(lstMgr);
}

Node* listIterate(ListManager* list, Node* n) {	
	if (list->first == NULL){
		return NULL;
	}
	else if (n == NULL) {
		n = list->first;
		return n;
	} 

	if (n->next == NULL) {
		return NULL;
	}

	return n->next;
}

Node* listRevIterate(ListManager* list, Node* n) {
	if (list->last == NULL){
		return NULL;
	}
	else if (n == NULL) {
		n = list->last;
		return n;
	} 

	if (n->prev == NULL) {
		return NULL;
	}

	return n->prev;
}

short listInsertAfter(ListManager* lst, Node* n, short id) {
	Node* prev = getNode(lst, id);
	if (prev == NULL){
		return 0;
	}

	short inList = getNode(lst, n->id) == NULL ? 0 : 1;
	if (inList) {
		return 0;
	}

	n->prev = prev;
	n->next = prev->next;
	n->lstMgr = lst;

	if (prev->next != NULL)
	{
		prev->next->prev = n;
	}

	prev->next = n;

	if (prev == lst->last){
		lst->last = n;
	}

	lst->nodeCount++;

	return 1;
}