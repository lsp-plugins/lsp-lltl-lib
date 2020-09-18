/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-lltl-lib
 * Created on: 26 авг. 2020 г.
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

#include <lsp-plug.in/test-fw/mtest.h>
#include <lsp-plug.in/lltl/types.h>
#include <lsp-plug.in/stdlib/math.h>

MTEST_BEGIN("lltl.func", ptrhash)

    static void clear(int *p, size_t n)
    {
        for (size_t i=0; i<n; ++i)
            p[i]    = 0;
    }

    void stats(int *p, size_t n)
    {
        int min = p[0], max = p[0];
        double kn = 1.0/double(n);
        double v = p[0];

        double avg = v * kn, disp = v*v*kn;

        for (size_t i=1; i<n; ++i)
        {
            if (p[i] > max)
                max = p[i];
            if (p[i] < min)
                min = p[i];

            v = p[i];
            avg  += v * kn;   // M[x]   = sum{ p[i] / n }
            disp += v*v * kn; // M[x^2] = sum{ p[i]^2 / n }
        }

        disp -= avg*avg; // M[x^2] - M[x]^2

        printf("0x%04x: min=%d, max=%d, avg=%f, disp=%f, qdisp=%f\n",
                int(n), int(min), int(max), avg, disp, sqrt(disp));
    }

    void test_sequential()
    {
        printf("Testing sequential data allocation\n");
        int *v = new int[0x100000];
        int b1[0x10], b2[0x100], b3[0x1000];

        clear(b1, 0x10);
        clear(b2, 0x100);
        clear(b3, 0x1000);

        for (int i=0; i<0x100000; ++i)
        {
            size_t hash = lltl::ptr_hash_func(&v[i], sizeof(int));
            b1[hash & 0xf]++;
            b2[hash & 0xff]++;
            b3[hash & 0xfff]++;
        }

        delete [] v;

        stats(b1, 0x10);
        stats(b2, 0x100);
        stats(b3, 0x1000);
    }

    void test_random()
    {
        printf("Testing random data allocation\n");
        int **v = new int *[0x100000];
        int b1[0x10], b2[0x100], b3[0x1000];

        clear(b1, 0x10);
        clear(b2, 0x100);
        clear(b3, 0x1000);

        for (int i=0; i<0x100000; ++i)
            v[i] = new int(0);

        for (int i=0; i<0x100000; ++i)
        {
            size_t hash = lltl::ptr_hash_func(v[i], sizeof(int));
            b1[hash & 0xf]++;
            b2[hash & 0xff]++;
            b3[hash & 0xfff]++;
        }

        for (int i=0; i<0x100000; ++i)
            delete v[i];
        delete [] v;

        stats(b1, 0x10);
        stats(b2, 0x100);
        stats(b3, 0x1000);
    }

    MTEST_MAIN
    {
        test_sequential();
        test_random();
    }

MTEST_END;

