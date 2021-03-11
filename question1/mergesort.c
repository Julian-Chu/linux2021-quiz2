#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

typedef struct __element {
    char *value;
    struct list_head list;
} list_ele_t;


static list_ele_t *get_middle(struct list_head *list)
{
    struct list_head *fast = list->next, *slow;
    list_for_each (slow, list) {
        if (fast->next==list || fast->next->next==list)
            break;
        fast = fast->next->next;
    }
    return list_entry(slow, list_ele_t, list);
}

static void list_merge(struct list_head *lhs,
                       struct list_head *rhs,
                       struct list_head *head)
{
    INIT_LIST_HEAD(head);
    if (list_empty(lhs)) {
        list_splice_tail(lhs, head);
        return;
    }
    if (list_empty(rhs)) {
        list_splice_tail(rhs, head);
        return;
    }

    while (!list_empty(lhs) && !list_empty(rhs)) {
        char *lv = list_entry(lhs->next, list_ele_t, list)->value;
        char *rv = list_entry(rhs->next, list_ele_t, list)->value;
        struct list_head *tmp = strcmp(lv, rv) <= 0 ? lhs->next : rhs->next;
        list_del(tmp);
        list_add_tail(tmp, head);
    }
    list_splice_tail(list_empty(lhs) ? rhs : lhs, head);
}


void list_merge_sort(list_ele_t *head)
{
    if (list_is_singular(&head->list))
        return;

    list_ele_t left;
    struct list_head sorted;
    INIT_LIST_HEAD(&left.list);
    list_cut_position(&left.list, &head->list, &get_middle(&head->list)->list);
    list_merge_sort(&left);
    list_merge_sort(head);
    list_merge(&left.list, &head->list, &sorted);
    INIT_LIST_HEAD(&head->list);
    list_splice_tail(&sorted, &head->list);
}

static bool validate(list_ele_t *head)
{
    struct list_head *node;
    list_for_each (node, &head->list) {
        if (node->next == &head->list)
            break;
        if (strcmp(list_entry(node, list_ele_t, list)->value,
                   list_entry(node->next, list_ele_t, list)->value) > 0)
            return false;
    }
    return true;
}

#define list_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
        pos = n, n = pos->next)

static void list_free(list_ele_t *head)
{

    struct list_head *cur, *tmp;
    list_ele_t *element;
    list_for_each_safe(cur, tmp,  &head->list){
        element = list_entry(cur, list_ele_t, list);
        list_del(cur);
        free(element->value);
        free(element);
        element = NULL;
    }
    
    free(head->value);
    free(head);
    head = NULL;
}

bool list_insert(list_ele_t *head, char *s)
{

    list_ele_t *newh = malloc(sizeof(list_ele_t));
    if (!newh)
        return false;

    char *new_value = strdup(s);
    if (!new_value) {
        free(newh);
        return false;
    }

    newh->value = new_value;
    struct list_head list;
    newh->list = list;
    INIT_LIST_HEAD(&newh->list);

    list_add_tail(&newh->list, &head->list);

    return true;
}

int main(void)
{
    FILE *fp = fopen("cities.txt", "r");
    if (!fp) {
        perror("failed to open cities.txt");
        exit(EXIT_FAILURE);
    }

    list_ele_t *head = malloc(sizeof(list_ele_t));
    head->value = NULL;
    struct list_head list;
    head->list = list;
    INIT_LIST_HEAD(&head->list);

    char buf[256];
    while (fgets(buf, 256, fp))
        list_insert(head, buf);
    fclose(fp);

    list_merge_sort(head);
    assert(validate(head));

    list_free(head);

    printf("done!\n");
    return 0;
}
