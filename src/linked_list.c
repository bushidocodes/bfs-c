#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct node
{
    int value;
    struct node *next; /* pointer to the successor in the linked list */
} node;

// Search the linked list for an int and return the address of the first match
node *find_node(node *pNode, int value)
{
    if (pNode == NULL)
        return NULL;

    if (pNode->value == value)
    {
        return pNode;
    }
    else
        return find_node(pNode->next, value);
}

void print_list(node *pNode, bool print_header)
{
    if (print_header == true)
    {
        printf("Nodes\n");
        printf("====================\n");
    }
    if (pNode != NULL)
    {
        printf("%d\n", pNode->value);
        if (pNode->next != NULL)
        {
            print_list(pNode->next, false);
        }
    }
    else
        printf("Is Null");
}

// We insert new nodes at the start of the linked list and set the head pointer to the
// address of this new node
void insert_node(node **pHead, int value)
{
    node *new_node;
    new_node = malloc(sizeof(node));
    new_node->value = value;

    new_node->next = *pHead;
    *pHead = new_node; /* and then set the new node to the head */
}

// This function searches for the predecessor of the node matching the passed value
node *find_predecessor(node *pNode, int value)
{
    if ((pNode == NULL) || (pNode->next == NULL))
    {
        printf("Error: predecessor sought on null list. \n");
        return (NULL);
    }

    if ((pNode->next)->value == value) // Is this pNode the predecessor?
        return (pNode);
    else
        return (find_predecessor(pNode->next, value)); // Otherwise keep looking later in the linked list
}

void delete_node(node **pHead, int value)
{
    node *pMatch;                           /* item pointer */
    node *pPred;                            /* predecessor pointer */
    node *find_node(), *find_predecessor(); /* Huh??? */

    pMatch = find_node(*pHead, value);
    if (pMatch != NULL)
    {
        pPred = find_predecessor(*pHead, value);
        if (pPred == NULL) /* Is the element we're deleting at the start of the linked list? */
            *pHead = (*pHead)->next;
        else /* Otherwise it is somewhere else in the linked list */
            pPred->next = pMatch->next;
        /* free memory used by the node that we're deleting */
        free(pMatch);
    }
}

int main(void)
{
    node *head = NULL;
    insert_node(&head, 5);
    insert_node(&head, 10);
    insert_node(&head, 15);
    insert_node(&head, 20);
    print_list(head, true);
    delete_node(&head, 15);
    print_list(head, true);

    return 0;
}