#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

typedef struct __element {
    char *value;
    struct __element *next;
    struct list_head list;
} list_ele_t;

/* typedef struct { */
/*     list_ele_t *head; /1* Linked list of elements *1/ */
/*     list_ele_t *tail; */
/*     size_t size; */
/*     struct list_head list; */
/* } queue_t; */

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

/* void list_merge_sort(queue_t *q) */
/* { */
/*     if (list_is_singular(&q->list)) */
/*         return; */

/*     queue_t left; */
/*     struct list_head sorted; */
/*     INIT_LIST_HEAD(&left.list); */
/*     list_cut_position(&left.list, &q->list, &get_middle(&q->list)->list); */
/*     list_merge_sort(&left); */
/*     list_merge_sort(q); */
/*     list_merge(&left.list, &q->list, &sorted); */
/*     INIT_LIST_HEAD(&q->list); */
/*     list_splice_tail(&sorted, &q->list); */
/* } */


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

/* static bool validate(queue_t *q) */
/* { */
/*     struct list_head *node; */
/*     list_for_each (node, &q->list) { */
/*         if (node->next == &q->list) */
/*             break; */
/*         if (strcmp(list_entry(node, list_ele_t, list)->value, */
/*                    list_entry(node->next, list_ele_t, list)->value) > 0) */
/*             return false; */
/*     } */
/*     return true; */
/* } */

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

/* static queue_t *q_new() */
/* { */
/*     queue_t *q = malloc(sizeof(queue_t)); */
/*     if (!q) return NULL; */

/*     q->head = q->tail = NULL; */
/*     q->size = 0; */
/*     INIT_LIST_HEAD(&q->list); */
/*     return q; */
/* } */

static void q_free(list_ele_t *head)
{
    if (!head) return;

    list_ele_t *current = head->next;
    while (current->next != head) {
        list_ele_t *tmp = current;
        current = current->next;
        free(tmp->value);
        free(tmp);
    }
    free(head);
}

bool q_insert_head(list_ele_t *head, char *s)
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
    head->value = "";
    struct list_head list;
    head->list = list;
    INIT_LIST_HEAD(&head->list);

    char buf[256];
    while (fgets(buf, 256, fp))
        q_insert_head(head, buf);
    fclose(fp);

    list_merge_sort(head);
    assert(validate(head));

    /* q_free(head); */

    printf("done!\n");
    return 0;
}
