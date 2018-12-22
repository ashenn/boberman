#include "libList.h"
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>


void lockList(ListManager* lst) {
	pid_t curTH = getpid();
	// fprintf(stdout, "LOCKING LIST: %d / %d \n", (int) curTH, (int) lst->pid);

	if (curTH != lst->pid) {
		pthread_mutex_lock(&lst->mutex);
		lst->pid = curTH;
	}
	else {
		// fprintf(stdout, "LOCKING LIST IN SAME THREAD: %d \n", (int) lst->pid);
		//assert(0);
	}
	
}


void unlockList(ListManager* lst) {
	//fprintf(stdout, "UN-LOCKING LIST: %d \n", (int) getpid());
	lst->pid = -1;
	pthread_mutex_unlock(&lst->mutex);
}

void lockNode(Node* n) {
	pid_t curTH = getpid();
	//fprintf(stdout, "LOCKING Node: %d \n", (int) curTH);
	//fprintf(stdout, "CURRENT Node: %d \n", (int) n->pid);

	if (curTH != n->pid) {
		pthread_mutex_lock(&n->mutex);
		n->pid = curTH;
	}
	else {
		// fprintf(stdout, "LOCKING NODE IN SAME THREAD: %d \n", (int) n->pid);
		//assert(0);
	}

}

void unlockNode(Node* n) {
	//fprintf(stdout, "UN-LOCKING NODE: %d \n", (int) getpid());
	n->pid = -1;
	pthread_mutex_unlock(&n->mutex);
}


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

	lstMgr->cond = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
	lstMgr->mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;

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

	// lockList(lstMgr);

	newNode->name = malloc(strlen(name)+1);
	strcpy(newNode->name, name);

	id = ++lstMgr->lastId;
	newNode->id = id;
	newNode->lstMgr = lstMgr;
	newNode->valIsAlloc = 0;
	newNode->value = NULL;
	newNode->del = NULL;
	
	newNode->cond = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
	newNode->mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;

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
	// unlockList(lstMgr);

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

	// lockNode(node);

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

	// unlockNode(node);
}

/**
 * Print List Content
 * @param lstMgr [description]
 */
void printNodes(ListManager* lstMgr){
	Node* currentNode;
	currentNode = NULL;

	// lockList(lstMgr);

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

	// unlockList(lstMgr);
}

/**
 * Get Node By Id
 * @param  lstMgr List
 * @param  id     Id
 * @return        Node*
 */
Node* getNode(ListManager* lstMgr, int id){
	Node* currentNode = NULL;

	// lockList(lstMgr);

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

		//fprintf(stderr, "TEST %d => %s / %d\n", currentNode->id, currentNode->name, id );

		if (
			currentNode != NULL 
			&& currentNode->id ==
			id
		) {
			return currentNode;
		}
	}

	// unlockList(lstMgr);
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

	// lockList(lstMgr);

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
			// unlockList(lstMgr);
			return currentNode;
		}
	}

	// unlockList(lstMgr);
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


	// lockList(lstMgr);
	// lockNode(node);
	Node* prev = node->prev;
	Node* next = node->next;

	if (prev != NULL) {
		prev->next = next;
	}

	if (next != NULL){
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
	else if (!lstMgr->nodeCount){
		lstMgr->first = NULL;
		lstMgr->last = NULL;
	}

	// unlockNode(node);
	// unlockList(lstMgr);
	return node;
}

/**
 * Delete Node By Id
 * @param lstMgr List
 * @param id     Id
 */
void* deleteNode(ListManager* lstMgr, int id){
	//printf("DELETING NODE: %d\n", id);
	Node* node = deleteNodeNoFree(lstMgr, id);
	if (node == NULL) {
		return NULL;
	}

	// lockList(lstMgr);
	// lockNode(node);
	if (node->del != NULL) {
		node->del(node->value);
	}
	else if (node->valIsAlloc){
		free(node->value);
		node->value = NULL;
	}

	free(node->name);
	// unlockNode(node);
	free(node);

	// unlockList(lstMgr);

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
	if (node == NULL) {
		fprintf(stderr, "SETTING VALUE ON NULL NODE !!!\n");
		assert(1);
	}
	// lockNode(node);
	if (node->valIsAlloc && node->value != NULL){
		free(node->value);
	}

	node->value = value;
	node->valIsAlloc = asAlloc;

	// unlockNode(node);
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
	// lockList(lstMgr);

	do{
		if (currentNode == NULL){
			currentNode = lstMgr->first;
		}
		else{
			currentNode = currentNode->next;
		}
		// lockNode(currentNode);

		ids[i] = currentNode->id;
				
		i++;
		// unlockNode(currentNode);
	}while(lstMgr->last != currentNode);

	// unlockList(lstMgr);
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
	// lockList(list);
	if (list->first == NULL){
		// unlockList(list);
		return NULL;
	}
	else if (n == NULL) {
		n = list->first;
		// lockNode(n);
		// unlockList(list);
		return n;
	}
	else{
		// unlockNode(n);
	}

	// unlockList(list);
	if (n->next == NULL) {
		return NULL;
	}

	return n->next;
}

Node* listRevIterate(ListManager* list, Node* n) {
	// lockList(list);
	if (list->last == NULL){
		// unlockList(list);
		return NULL;
	}
	else if (n == NULL) {
		n = list->last;
		// lockNode(n);
		// unlockList(list);
		return n;
	} 
	else {
		// unlockNode(n);
	}
	if (n->prev == NULL) {
		// unlockList(list);
		return NULL;
	}

	// unlockList(list);
	return n->prev;
}

short listInsertAfter(ListManager* lst, Node* n, short id) {
	Node* prev = getNode(lst, id);
	if (prev == NULL){
		return 0;
	}

	short inList = (getNode(lst, n->id) == NULL) ? 0 : 1;
	if (!inList) {
		return 0;
	}

	//fprintf(stdout, "-- Inserting %s After %s\n", n->name, prev->name);
	if (n->prev == prev) {
		//fprintf(stdout, "-- Nothing to to\n");
		return 0;
	}


	//printNodes(lst);


	Node* tmpP = n->prev;
	Node* tmpN = n->next;

	if (tmpP != NULL) {
		if (tmpN != NULL) {
			//fprintf(stdout, "-- 1# %s Is Now Before %s\n", tmpP->name, tmpN->name);
			tmpP->next = tmpN;
		}
		else{
			//fprintf(stdout, "-- 1# %s Is Now LAST\n", tmpP->name);
			tmpP->next = NULL;
			lst->last = tmpP;
		}

	}

	if (tmpN != NULL) {
		if (tmpP != NULL) {
			//fprintf(stdout, "-- 2# %s Is Now After %s\n", tmpN->name, tmpP->name);
			tmpN->prev = tmpP;
		}
		else{
			//fprintf(stdout, "-- 2# %s Is Now FIRST\n", tmpN->name);
			tmpN->prev = NULL;
			lst->first = tmpN;
		}
	}

	n->prev = prev;
	//fprintf(stdout, "-- 3# %s Is Now Before %s\n", prev->name, n->name);
	
	n->next = prev->next;
	//fprintf(stdout, "-- 4# %s Is Now Before %s\n", n->name, prev->next->name);

	n->lstMgr = lst;

	if (prev->next != NULL) {
		prev->next->prev = n;
	}

	prev->next = n;
	//fprintf(stdout, "-- 5# %s Is Now Before %s\n", prev->name, n->name);

	if (prev == lst->last){
		lst->last = n;
		n->next = NULL;
	}

	//fprintf(stdout, "==============\n");
	//lst->nodeCount++;

	return 1;
}

void sortList(ListManager * lst, short (*fnc)(void*, void*)) {
	//fprintf(stdout, "========= SORTING LIST =========\n");
	
	int i;
	short sort = 0;
	Node* tmp = NULL;
	Node* comp = NULL;

	Node* key = lst->first->next;
	for (i = 1; i < lst->nodeCount; i++) {
    	comp = lst->first;

		//fprintf(stdout, "##### Node %d: %s #####\n", key->id, key->name);

        do {
			//fprintf(stdout, "-- Compare %d: %s\n", comp->id, comp->name);
        	if (key == comp) {
				//fprintf(stdout, "-- Skipping\n");
        		comp = key->next;
        		continue;
        	}

    		
    		sort = (*fnc)(comp->value, key->value);
			
			//fprintf(stdout, "-- Res: %d\n", sort);

    		if (sort < 0) {
    			tmp = NULL;
				//fprintf(stdout, "+++++ Moving: %s\n", key->name);
    			while((tmp = listIterate(lst, tmp)) != NULL) {
    				if (tmp == comp) {
						//fprintf(stdout, "-- Skipping\n");
    					continue;
    				}

					//fprintf(stdout, "-- Compare %d: %s\n", tmp->id, tmp->name);
					sort = fnc(comp->value, tmp->value);
					
					//fprintf(stdout, "-- Res: %d\n", sort);

    			    if (sort < 0) {
						listInsertAfter(lst, comp, tmp->id);
						listInsertAfter(lst, tmp, comp->id);

						comp = key;
						break;
    			    }

					//fprintf(stdout, "+++++++++++++++++++++++++\n");
    			}

    			//printNodes(lst);
				//assert(0);
    		}
	
    		comp = comp->next;
			//fprintf(stdout, "================================================\n");
        } while (comp != NULL);

		//fprintf(stdout, "--------------------------------------------------------------------\n");
   		key = key->next;
        if (key == NULL) {
        	break;
        }
	
	}	
	
	//fprintf(stdout, "==== Sort Completed ====\n");
	tmp = NULL;
	while((tmp = listIterate(lst, tmp)) != NULL) {
		//fprintf(stdout "-- Node: %d: %s\n", tmp->id, tmp->name);
	}

	//assert(0);
}