#include "rtthread.h"
#include "gtest/gtest.h"

constexpr size_t N = 10;

class list_test : public testing::Test
{
protected:
    void SetUp() override
    {
        rt_list_init(&head);
    }

    void TearDown() override
    {
    }

    rt_list_t head;
};

TEST_F(list_test, insert_after_1)
{
    rt_list_t node1 = RT_LIST_OBJECT_INIT(node1);
    rt_list_t node2 = RT_LIST_OBJECT_INIT(node2);

    rt_list_insert_after(&head, &node1);
    EXPECT_EQ(rt_list_len(&head), 1u);
    EXPECT_EQ(head.next, &node1);

    rt_list_insert_after(&node1, &node2);
    EXPECT_EQ(rt_list_len(&head), 2u);
    EXPECT_EQ(node1.next, &node2);
}

TEST_F(list_test, insert_after_2)
{
    rt_list_t node[N];
    for (unsigned int i = 0; i < N; i++)
    {
        EXPECT_EQ(rt_list_len(&head), i);
        rt_list_insert_after(&head, &node[i]);
        EXPECT_EQ(rt_list_len(&head), i + 1);
        EXPECT_EQ(head.next, &node[i]);
    }
}

TEST_F(list_test, insert_before_1)
{
    rt_list_t node1 = RT_LIST_OBJECT_INIT(node1);
    rt_list_t node2 = RT_LIST_OBJECT_INIT(node2);
    rt_list_t node3 = RT_LIST_OBJECT_INIT(node3);

    rt_list_insert_before(&head, &node1);
    EXPECT_EQ(rt_list_len(&head), 1u);
    EXPECT_EQ(head.prev, &node1);

    rt_list_insert_before(&node1, &node2);
    EXPECT_EQ(rt_list_len(&head), 2u);
    EXPECT_EQ(node1.prev, &node2);

    rt_list_insert_before(&node2, &node3);
    EXPECT_EQ(rt_list_len(&head), 3u);
    EXPECT_EQ(node2.prev, &node3);
}

TEST_F(list_test, insert_before_2)
{
    rt_list_t node[N];
    for (unsigned int i = 0; i < N; i++)
    {
        EXPECT_EQ(rt_list_len(&head), i);
        rt_list_insert_before(&head, &node[i]);
        EXPECT_EQ(rt_list_len(&head), i + 1);
        EXPECT_EQ(head.prev, &node[i]);
    }
}

TEST_F(list_test, remove)
{
    rt_list_t node[N];
    for (unsigned int i = 0; i < N; i++)
    {
        rt_list_insert_after(&head, &node[i]);
    }
    EXPECT_EQ(rt_list_len(&head), 10u);

    for (unsigned int i = 0; i < N; i++)
    {
        EXPECT_EQ(rt_list_len(&head), N - i);
        rt_list_remove(&node[i]);
        EXPECT_EQ(rt_list_len(&head), N - i - 1);
    }
}

TEST_F(list_test, isempty)
{
    EXPECT_TRUE(rt_list_isempty(&head));

    rt_list_t node1 = RT_LIST_OBJECT_INIT(node1);
    rt_list_insert_after(&head, &node1);
    EXPECT_FALSE(rt_list_isempty(&head));
}

struct foo
{
    rt_list_t node;
    unsigned int value;
};

TEST_F(list_test, entry)
{
    foo f1 = {RT_LIST_OBJECT_INIT(f1.node), 1};
    foo f2 = {RT_LIST_OBJECT_INIT(f2.node), 2};
    foo f3 = {RT_LIST_OBJECT_INIT(f3.node), 3};

    EXPECT_EQ(rt_list_entry(&(f1.node), foo, node), &f1);
    EXPECT_EQ(rt_list_entry(&(f2.node), foo, node), &f2);
    EXPECT_EQ(rt_list_entry(&(f3.node), foo, node), &f3);
}

TEST_F(list_test, foreach)
{
    foo a[N];
    memset(a, 0, sizeof(a));

    for (unsigned int i = 0; i < N; i++)
    {
        a[i].value = i;
        rt_list_insert_before(&head, &a[i].node);
        // rt_list_insert_after(&head, &a[i].node);
    }
    EXPECT_EQ(rt_list_len(&head), 10u);
    unsigned int i = 0;
    rt_list_t *p;
    rt_list_for_each(p, &head)
    {
        foo *f = rt_list_entry(p, foo, node);
        EXPECT_EQ(f->value, i);
        i++;
    }
}

TEST_F(list_test, for_each_safe)
{
    foo a[N];
    memset(a, 0, sizeof(a));

    for (unsigned int i = 0; i < N; i++)
    {
        a[i].value = i;
        rt_list_insert_before(&head, &a[i].node);
        // rt_list_insert_after(&head, &a[i].node);
    }
    EXPECT_EQ(rt_list_len(&head), 10u);

    unsigned int i = 0;
    rt_list_t *p, *n;
    rt_list_for_each_safe(p, n, &head)
    {
        foo *f = rt_list_entry(p, foo, node);
        EXPECT_EQ(f->value, i);
        f->node.next = NULL; // it's OK in this case
        i++;
    }
}

TEST_F(list_test, for_each_entry)
{
    foo a[N];
    memset(a, 0, sizeof(a));

    for (unsigned int i = 0; i < N; i++)
    {
        a[i].value = i;
        rt_list_insert_before(&head, &a[i].node);
        // rt_list_insert_after(&head, &a[i].node);
    }
    EXPECT_EQ(rt_list_len(&head), 10u);

    unsigned int i = 0;
    foo *f;
    rt_list_for_each_entry(f, &head, node)
    {
        EXPECT_EQ(f->value, i);
        i++;
    }
}

TEST_F(list_test, for_each_entry_safe)
{
    foo a[N];
    memset(a, 0, sizeof(a));

    for (unsigned int i = 0; i < N; i++)
    {
        a[i].value = i;
        rt_list_insert_before(&head, &a[i].node);
        // rt_list_insert_after(&head, &a[i].node);
    }
    EXPECT_EQ(rt_list_len(&head), 10u);

    unsigned int i = 0;
    foo *f, *n;
    rt_list_for_each_entry_safe(f, n, &head, node)
    {
        EXPECT_EQ(f->value, i);
        f->node.next = NULL; // it's OK in this case
        i++;
    }
}

TEST_F(list_test, first_entry)
{
    foo f1 = {RT_LIST_OBJECT_INIT(f1.node), 0};
    rt_list_insert_after(&head, &f1.node);
    EXPECT_EQ(rt_list_len(&head), 1u);

    foo *f = rt_list_first_entry(&head, foo, node);
    EXPECT_EQ(f->value, 0u);
}
