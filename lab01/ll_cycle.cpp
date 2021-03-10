#include "ll_cycle.h"

bool ll_has_cycle(node *head) {
    node *turtle = head;
    node *rabbit = head;

    while (rabbit && rabbit->next)
    {
        rabbit = rabbit->next->next;
        turtle = turtle->next;
        if(rabbit == turtle) return true;
    }
    return false;
}
