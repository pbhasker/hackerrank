#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define ASSERT_EQUAL(a, b) do {int c = ((a) != (b)); if (c){printf("FAILED at %d - 0x%x", __LINE__, a);}} while(0);
#define ASSERT_SUCCESS(a) ASSERT_EQUAL(a, kSuccess)

#define MOD         1000007
#define MAX_CITIES  200000

#define PARENT(a) ((a) / 2)
#define LEFT(a) (2 * (a) + 1)
#define RIGHT(a) (2 * (a) + 2)

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
    int64_t idx;
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

int seg_tree_update(int64_t *seg_tree, int idx, int64_t new_val, int lo, int hi, int pos)
{
    if (idx < lo || idx > hi)
    {
        return kInvalidArgument;
    }
    
    if (lo == hi)
    {
        seg_tree[pos] = new_val;
        return kSuccess;
    }
    
    int mid = (lo + hi)/2;
    
    seg_tree_update(seg_tree, idx, new_val, lo, mid, LEFT(pos));
    seg_tree_update(seg_tree, idx, new_val, mid + 1, hi, RIGHT(pos));
    seg_tree[pos] = SUM(seg_tree[LEFT(pos)], seg_tree[RIGHT(pos)]);
    
    return kSuccess;
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
    
    int64_t a = seg_tree_query(seg_tree, qlo, qhi, lo, mid, LEFT(pos));
    int64_t b = seg_tree_query(seg_tree, qlo, qhi, mid + 1, hi, RIGHT(pos));
    
    return SUM(a, b);
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
    
    seg_tree_build(input, seg_tree, lo, mid, LEFT(pos));
    seg_tree_build(input, seg_tree, mid + 1, hi, RIGHT(pos));
    seg_tree[pos] = SUM(seg_tree[LEFT(pos)], seg_tree[RIGHT(pos)]);
    
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

int display_cable(int cable)
{
    printf("%d\r\n", cable);
    return kSuccess;
}

int compute_cable(struct city *cities, size_t cities_count)
{
    // Set location index in each struct
    quicksort(cities, 0, cities_count - 1, cmp_loc);
    
    for (size_t i = 0; i < cities_count; i++)
    {
        cities[i].idx = i;
    }
    
    // Build location seg tree
    quicksort(cities, 0, cities_count - 1, cmp_pop);
    
    int64_t locs[MAX_CITIES];
    size_t locs_count = 0;
    size_t locs_seg_tree_count = seg_tree_get_size(cities_count);
    
    int64_t *locs_seg_tree = (int64_t *) malloc(locs_seg_tree_count * sizeof(int64_t));
    memset(locs_seg_tree, 0, locs_seg_tree_count * sizeof(locs_seg_tree[0]));
    
    for (size_t i = 0; i < cities_count; i++)
    {
        locs[i] = cities[i].loc;
    }
    
    seg_tree_build(locs, locs_seg_tree, 0, 0, 0);
    
    // Build previous cities seg tree
    int64_t previous_cities[MAX_CITIES];
    size_t previous_cities_count = 0;
    size_t previous_cities_seg_tree_count = seg_tree_get_size(cities_count);
    
    int64_t *previous_cities_seg_tree = (int64_t *) malloc(previous_cities_seg_tree_count * sizeof(int64_t));
    memset(previous_cities_seg_tree, 0, previous_cities_seg_tree_count * sizeof(previous_cities_seg_tree[0]));
    
    seg_tree_build(previous_cities, previous_cities_seg_tree, 0, 0, 0);
    
    int64_t cable = 0;
    int64_t cable_left = 0;
    int64_t cable_right = 0;
    int cr = 0;
    int cl = 0;
    
    for (size_t i = 0; i < cities_count; i++)
    {
        cl = seg_tree_query(previous_cities_seg_tree, 0, i - 1, 0, previous_cities_count, 0);
        cr = seg_tree_query(previous_cities_seg_tree, i + 1, previous_cities_count, 0, cities_count - 1, 0);
        
        cable_left = cl * cities[i].loc - seg_tree_query(locs_seg_tree, 0, i - 1, 0, cities_count - 1, 0);
        cable_right = seg_tree_query(locs_seg_tree, i + 1, i, 0, cities_count - 1, 0) - cr * cities[i].loc;
        
        // Update previous cities tree
        previous_cities_count++;
        seg_tree_update(previous_cities_seg_tree, i, 1, 0, previous_cities_count, 0);
        
        // Update locs tree
        locs_count++;
        seg_tree_update(locs_seg_tree, i, cities[i].loc, 0, locs_seg_tree_count, 0);
        
        cable += cable_left + cable_right;
        
        cable %= MOD;
    }
    
    // Free seg trees
    free(locs_seg_tree);
    free(previous_cities_seg_tree);
    
    return cable;
}

int direct_connections()
{
    int test_count = 0;
    int pop = 0;
    int loc = 0;
    int cities_count = 0;
    struct city cities[MAX_CITIES];
    
    scanf("%d", &test_count);
    
    for (size_t i = 0; i < test_count; i++)
    {
        // Read number of cities
        scanf("%d", &cities_count);
        
        // Read city locations
        for (size_t i = 0; i < cities_count; i++)
        {
            scanf("%d", &(loc));
            cities[i].loc = loc;
        }    
        
        // Read city populations
        for (size_t i = 0; i < cities_count; i++)
        {
            scanf("%d", &pop);
            cities[i].pop = pop;
        }
        
        int cable = compute_cable(cities, cities_count);
        
        display_cable(cable);
    
    }
    
    return kSuccess;
}

int main(void)
{
    freopen("test0", "r", stdin);
    direct_connections();
    
    return kSuccess;
}