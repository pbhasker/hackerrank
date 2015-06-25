#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define ASSERT_EQUAL(a, b) do {int c = ((a) != (b)); if (c){printf("FAILED at %d - 0x%x", __LINE__, a);}} while(0);
#define ASSERT_SUCCESS(a) ASSERT_EQUAL(a, kSuccess)

#define SUM(a, b) ((a) + (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define DEBUG 1
#if DEBUG
#define printf_dbg(args...) printf(args)
#else
#define printf_dbg(args...) {}
#endif

enum status
{
    kSuccess = 0,
    kInvalidArgument,
    kUnimplemented,
};

struct city
{
    int64_t pop;
    int64_t loc;
    int64_t index;
};

int cmp_pop(struct city *a, struct city *b)
{
    if (a->pop > b->pop)
    {
        return 1;
    }
    if (a->pop < b->pop)
    {
        return -1;
    }
    
    return 0;
}

int cmp_loc(struct city *a, struct city *b)
{
    if (a->loc > b->loc)
    {
        return 1;
    }
    if (a->loc < b->loc)
    {
        return -1;
    }
    
    return 0;
}

int next_power_of_two(size_t in)
{
    size_t out = 1;
    
    while (1)
    {
        if (out >= in)
        {
            break;
        }
        out = out << 1;
    }
    
    return out;
}

int64_t seg_tree_query(int64_t *seg_tree, int qlo, int qhi, int lo, int hi, int pos)
{
    if (qlo <= lo && qhi >= hi) // Total overlap
    {
        return seg_tree[pos];
    }
    if (qlo > hi || qhi < lo) // No overlap
    {
        return 0;
    }
    
    int mid = (lo + hi) / 2;
    
    int64_t a = seg_tree_query(seg_tree, qlo, qhi, lo, mid, (2 * pos) + 1);
    int64_t b = seg_tree_query(seg_tree, qlo, qhi, mid + 1, hi, (2 * pos) + 2);
    
    return a + b;
    
    //return SUM(seg_tree_query(seg_tree, qlo, qhi, lo, mid, (2 * pos) + 1), seg_tree_query(seg_tree, qlo, qhi, mid + 1, hi, (2 * pos) + 2));
}

int seg_tree_get_size(size_t in)
{
    return (next_power_of_two(in) << 2) - 1;
}

int seg_tree_build(int64_t *input, int64_t *seg_tree, int lo, int hi, int pos)
{
    if (lo == hi)
    {
        seg_tree[pos] = input[lo];
        return kSuccess;
    }
    
    int mid = (lo + hi) / 2;
    
    seg_tree_build(input, seg_tree, lo, mid, 2 * pos + 1);
    seg_tree_build(input, seg_tree, mid + 1, hi, 2 * pos + 2);
    seg_tree[pos] = SUM(seg_tree[2 * pos + 2], seg_tree[2 * pos + 1]);
    
    return kSuccess;
}

int check_order(struct city *cities, size_t cities_count, int (*cmp)(struct city *a, struct city *b))
{
    for (size_t i = 1; i < cities_count; i++)
    {
        if (cmp(&(cities[i - 1]), &(cities[i])) < 1)
        {
            return kInvalidArgument;
        }
    }
    
    return kSuccess;
}

int swap(struct city *a, struct city *b)
{
    struct city temp = *a;
    *a = *b;
    *b = temp;
    
    return kSuccess;
}

int partition(struct city *arr, int lo, int hi, int (*cmp)(struct city *a, struct city *b))
{
    // TODO choose better pivot
    int pivotIndex = lo;
    struct city pivotValue = arr[pivotIndex];
    
    swap(&(arr[pivotIndex]), &(arr[hi]));
    
    int store_index = lo;
    
    for (size_t i = lo; i < hi; i++)
    {
        if (cmp(&(arr[i]), &pivotValue) > 0){
            swap(&(arr[i]), &arr[store_index]);
            store_index++;
        }
    }
    
    swap(&(arr[hi]), &arr[store_index]);
    
    return store_index;
}

int quicksort(struct city *arr, int lo, int hi, int (*cmp)(struct city *a, struct city *b))
{
    if (lo < hi)
    {
        int p = partition(arr, lo, hi, cmp);
        quicksort(arr, lo, p - 1, cmp);
        quicksort(arr, p + 1, hi, cmp);
    }
    
    return kSuccess;
}

int compute_cable(struct city *cities, size_t cities_count)
{

    return kSuccess;
}

int direct_connections()
{

    return kSuccess;
}

int main(void)
{
    int64_t input[5] = {-1, 0, 3, 6, 8};
    int64_t seg_tree[17] = {0};
    seg_tree_build(input, seg_tree, 0, 4, 0);
    int64_t test = seg_tree_query(seg_tree, 1, 3, 0, 3, 0);
    
    return kSuccess;
}