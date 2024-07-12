#include <stddef.h>
#include "ll_cycle.h"

int ll_has_cycle(node *head) {
    /* your code here */
    node* tortoise = head, *hare = head;
    while (1){
	if (hare->next == NULL || hare->next->next == NULL)
		return 0;
	if (hare->next == tortoise || hare->next->next == tortoise)
		return 1;
	hare = hare->next->next;
	tortoise = tortoise->next;
		
    }
    return 0;
}
