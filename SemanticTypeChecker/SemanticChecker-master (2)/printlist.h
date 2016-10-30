struct symbolNode {
	char* id;
	struct symbolNode* next;
};

struct typeNode {
	char* id;
	struct symbolNode* head;
	struct symbolNode* curr;
};

struct typeNode types[MAX_SYMBOLS];
struct symbolNode* add_to_list(struct symbolNode* head, char* id) {
	struct symbolNode* ptr = (struct symbolNode*) malloc(sizeof (struct symbolNode));

	if (ptr == NULL) {
		printf("\nERROR Could not create Node\n");
		return NULL;
	}


	if (head == NULL) {
		// Insert First Node
		ptr->id = id;
		ptr->next = NULL;
		head = ptr;
	} else {
		// Insert to End of List
		struct symbolNode* curr = head;

		ptr->id = id;
		ptr->next = NULL;

		while (curr->next != NULL) {
			curr = curr->next;
		}

		curr->next = ptr;
		curr = ptr;
	}

	// Return the head of the Linked List
	if (DEBUG) printf("SYMBOL INSERTED: ID (%s)\n", ptr->id);
	return head;
}
