/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-lltl-lib
 * Created on: 20 июн. 2024 г.
 *
 * lsp-lltl-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-lltl-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-lltl-lib. If not, see <https://www.gnu.org/licenses/>.
 */

#include <lsp-plug.in/lltl/darray.h>
#include <lsp-plug.in/lltl/state.h>
#include <lsp-plug.in/test-fw/utest.h>

UTEST_BEGIN("lltl", state)

    typedef struct state_t
    {
        size_t id;
        size_t check;
    } state_t;

    static void deleter1(state_t *st)
    {
        ++st->check     = 1;
    }

    static void deleter2(state_t *st, size_t *value)
    {
        st->check      += *value;
    }

    bool init_states(lltl::darray<state_t> & states)
    {
        for (size_t i=0; i<10; ++i)
        {
            state_t *st = states.add();
            if (st == NULL)
                return false;

            st->id      = i + 1;
            st->check   = 0;
        }

        return true;
    }

    void test_push_pull_no_deleter()
    {
        printf("Testing test_push_pull_no_deleter...\n");

        lltl::darray<state_t> st;
        UTEST_ASSERT(init_states(st));

        {
            lltl::state<state_t> state;

            // Step 1
            UTEST_ASSERT(state.pull() == NULL);
            state.push(st[0]);
            UTEST_ASSERT(st[0]->check == 0);
            UTEST_ASSERT(state.pull() == st[0]);
            UTEST_ASSERT(state.pull() == st[0]);
            UTEST_ASSERT(st[0]->check == 0);

            // Step 2
            state.push(st[1]);
            UTEST_ASSERT(state.pull() == st[1]);
            UTEST_ASSERT(st[0]->check == 0);
            UTEST_ASSERT(state.pull() == st[1]);

            // Step 3
            state.push(st[2]);
            state.push(st[3]);
            UTEST_ASSERT(state.pull() == st[3]);
            UTEST_ASSERT(st[1]->check == 0);
            UTEST_ASSERT(st[2]->check == 0);

            // Step 4
            state.push(st[4]);
            state.push(st[5]);
            UTEST_ASSERT(state.pull() == st[5]);
            UTEST_ASSERT(st[3]->check == 0);
            UTEST_ASSERT(st[4]->check == 0);
        }

        UTEST_ASSERT(st[3]->check == 0);
        UTEST_ASSERT(st[4]->check == 0);
    }

    void test_push_pull_deleter1()
    {
        printf("Testing test_push_pull_deleter1...\n");

        lltl::darray<state_t> st;
        UTEST_ASSERT(init_states(st));

        {
            lltl::state<state_t> state(deleter1);

            // Step 1
            UTEST_ASSERT(state.pull() == NULL);
            state.push(st[0]);
            UTEST_ASSERT(st[0]->check == 0);
            UTEST_ASSERT(state.pull() == st[0]);
            UTEST_ASSERT(state.pull() == st[0]);
            UTEST_ASSERT(st[0]->check == 0);

            // Step 2
            state.push(st[1]);
            UTEST_ASSERT(state.pull() == st[1]);
            UTEST_ASSERT(st[0]->check == 1);
            UTEST_ASSERT(state.pull() == st[1]);
            UTEST_ASSERT(st[1]->check == 0);

            // Step 3
            state.push(st[2]);
            state.push(st[3]);
            UTEST_ASSERT(state.pull() == st[3]);
            UTEST_ASSERT(st[1]->check == 1);
            UTEST_ASSERT(st[2]->check == 1);
            UTEST_ASSERT(state.pull() == st[3]);
            UTEST_ASSERT(st[2]->check == 1);
            UTEST_ASSERT(st[3]->check == 0);

            // Step 4
            state.push(st[4]);
            state.push(st[5]);
            UTEST_ASSERT(state.pull() == st[5]);
            UTEST_ASSERT(st[3]->check == 1);
            UTEST_ASSERT(st[4]->check == 1);
            UTEST_ASSERT(state.pull() == st[5]);
            UTEST_ASSERT(st[4]->check == 1);
            UTEST_ASSERT(st[5]->check == 0);
        }
        UTEST_ASSERT(st[0]->check == 1);
        UTEST_ASSERT(st[1]->check == 1);
        UTEST_ASSERT(st[2]->check == 1);
        UTEST_ASSERT(st[3]->check == 1);
        UTEST_ASSERT(st[4]->check == 1);
        UTEST_ASSERT(st[5]->check == 1);
    }

    void test_push_pull_deleter2()
    {
        printf("Testing test_push_pull_deleter2...\n");

        lltl::darray<state_t> st;
        UTEST_ASSERT(init_states(st));

        static constexpr size_t check = 0xfeedbeef;
        size_t value = check;

        {
            lltl::state<state_t> state(deleter2, &value);

            // Step 1
            UTEST_ASSERT(state.pull() == NULL);
            state.push(st[0]);
            UTEST_ASSERT(st[0]->check == 0);
            UTEST_ASSERT(state.pull() == st[0]);
            UTEST_ASSERT(state.pull() == st[0]);
            UTEST_ASSERT(st[0]->check == 0);

            // Step 2
            state.push(st[1]);
            UTEST_ASSERT(state.pull() == st[1]);
            UTEST_ASSERT(st[0]->check == check);
            UTEST_ASSERT(state.pull() == st[1]);

            // Step 3
            state.push(st[2]);
            state.push(st[3]);
            UTEST_ASSERT(state.pull() == st[3]);
            UTEST_ASSERT(st[1]->check == check);
            UTEST_ASSERT(st[2]->check == check);
            UTEST_ASSERT(state.pull() == st[3]);
            UTEST_ASSERT(st[2]->check == check);

            // Step 4
            state.push(st[4]);
            state.push(st[5]);
            UTEST_ASSERT(state.pull() == st[5]);
            UTEST_ASSERT(st[3]->check == check);
            UTEST_ASSERT(st[4]->check == check);
            UTEST_ASSERT(state.pull() == st[5]);
            UTEST_ASSERT(st[4]->check == check);
            UTEST_ASSERT(st[5]->check == 0);
        }
        UTEST_ASSERT(st[0]->check == check);
        UTEST_ASSERT(st[1]->check == check);
        UTEST_ASSERT(st[2]->check == check);
        UTEST_ASSERT(st[3]->check == check);
        UTEST_ASSERT(st[4]->check == check);
        UTEST_ASSERT(st[5]->check == check);
    }

    void test_push_get()
    {
        printf("Testing test_push_get...\n");

        lltl::darray<state_t> st;
        UTEST_ASSERT(init_states(st));

        {
            lltl::state<state_t> state(deleter1);

            // Step 1
            UTEST_ASSERT(state.get() == NULL);
            state.push(st[0]);
            UTEST_ASSERT(st[0]->check == 0);
            UTEST_ASSERT(state.get() == st[0]);
            UTEST_ASSERT(state.get() == st[0]);
            UTEST_ASSERT(st[0]->check == 0);

            // Step 2
            state.push(st[1]);
            UTEST_ASSERT(state.get() == st[1]);
            UTEST_ASSERT(st[0]->check == 0);
            UTEST_ASSERT(state.get() == st[1]);
            UTEST_ASSERT(st[0]->check == 0);

            // Step 3
            state.push(st[2]);
            UTEST_ASSERT(state.get() == st[2]);
            UTEST_ASSERT(st[0]->check == 1);
            UTEST_ASSERT(st[1]->check == 0);
            UTEST_ASSERT(state.get() == st[2]);
            UTEST_ASSERT(st[1]->check == 0);

            // Step 4
            state.push(st[3]);
            state.push(st[4]);
            UTEST_ASSERT(state.get() == st[4]);
            UTEST_ASSERT(st[1]->check == 1);
            UTEST_ASSERT(st[2]->check == 0);
            UTEST_ASSERT(st[3]->check == 1);
            UTEST_ASSERT(st[4]->check == 0);
            UTEST_ASSERT(state.get() == st[4]);
            UTEST_ASSERT(st[3]->check == 1);
            UTEST_ASSERT(st[4]->check == 0);

            // Step 5
            state.push(st[5]);
            state.push(st[6]);
            UTEST_ASSERT(state.get() == st[6]);
            UTEST_ASSERT(st[2]->check == 1);
            UTEST_ASSERT(st[4]->check == 0);
            UTEST_ASSERT(st[5]->check == 1);
            UTEST_ASSERT(st[6]->check == 0);
            UTEST_ASSERT(state.get() == st[6]);
            UTEST_ASSERT(st[5]->check == 1);
            UTEST_ASSERT(st[6]->check == 0);
        }
        UTEST_ASSERT(st[0]->check == 1);
        UTEST_ASSERT(st[1]->check == 1);
        UTEST_ASSERT(st[2]->check == 1);
        UTEST_ASSERT(st[3]->check == 1);
        UTEST_ASSERT(st[4]->check == 1);
        UTEST_ASSERT(st[5]->check == 1);
        UTEST_ASSERT(st[6]->check == 1);
    }

    void test_set_pull()
    {
        printf("Testing test_set_pull...\n");

        lltl::darray<state_t> st;
        UTEST_ASSERT(init_states(st));

        {
            lltl::state<state_t> state(deleter1);

            // Step 1
            UTEST_ASSERT(state.pull() == NULL);
            UTEST_ASSERT(state.set(st[0]));
            UTEST_ASSERT(st[0]->check == 0);
            UTEST_ASSERT(state.pull() == st[0]);
            UTEST_ASSERT(state.pull() == st[0]);
            UTEST_ASSERT(st[0]->check == 0);

            // Step 2
            UTEST_ASSERT(state.set(st[1]));
            UTEST_ASSERT(state.pull() == st[1]);
            UTEST_ASSERT(st[0]->check == 1);
            UTEST_ASSERT(st[1]->check == 0);
            UTEST_ASSERT(state.pull() == st[1]);
            UTEST_ASSERT(st[0]->check == 1);
            UTEST_ASSERT(st[1]->check == 0);

            // Step 3
            UTEST_ASSERT(state.set(st[2]));
            UTEST_ASSERT(state.set(st[3]));
            UTEST_ASSERT(state.pull() == st[3]);
            UTEST_ASSERT(st[1]->check == 1);
            UTEST_ASSERT(st[2]->check == 1);
            UTEST_ASSERT(st[3]->check == 0);
            UTEST_ASSERT(state.pull() == st[3]);
            UTEST_ASSERT(st[1]->check == 1);
            UTEST_ASSERT(st[2]->check == 1);
            UTEST_ASSERT(st[3]->check == 0);

            // Step 4
            UTEST_ASSERT(state.set(st[4]));
            UTEST_ASSERT(state.set(st[5]));
            UTEST_ASSERT(!state.set(st[6]));
            UTEST_ASSERT(state.pull() == st[5]);
            UTEST_ASSERT(st[3]->check == 1);
            UTEST_ASSERT(st[4]->check == 1);
            UTEST_ASSERT(st[5]->check == 0);
            UTEST_ASSERT(st[6]->check == 0);
            UTEST_ASSERT(state.pull() == st[5]);
            UTEST_ASSERT(st[3]->check == 1);
            UTEST_ASSERT(st[4]->check == 1);
            UTEST_ASSERT(st[5]->check == 0);
            UTEST_ASSERT(st[6]->check == 0);
        }
        UTEST_ASSERT(st[0]->check == 1);
        UTEST_ASSERT(st[1]->check == 1);
        UTEST_ASSERT(st[2]->check == 1);
        UTEST_ASSERT(st[3]->check == 1);
        UTEST_ASSERT(st[4]->check == 1);
        UTEST_ASSERT(st[5]->check == 1);
        UTEST_ASSERT(st[6]->check == 0);
    }

    UTEST_MAIN
    {
        test_push_pull_no_deleter();
        test_push_pull_deleter1();
        test_push_pull_deleter2();
        test_push_get();
        test_set_pull();
    }

UTEST_END;



