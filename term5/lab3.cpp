#include <omp.h>
#include <iostream>
#include <ctime>
#include <stdio.h> 

static void solve()
{
    srand(time(0));
    const int size = 10000000;
    int* A = new int[size];
    int* B = new int[size];
    int start, i;
    int res = 0;
    for (i = 0; i < size; i++)
    {
        A[i] = rand() % 100 - rand() % 100;
        B[i] = rand() % 100 - rand() % 100;
    }
    start = clock();

    for (i = 0; i < size; i++)
    {
        res += A[i] * B[i];
    }
    std::cout << "synch res: " << res << ", time: " << clock() - start << std::endl;
    
    res = 0;
    int chunk = size / 8;
    start = clock();

    #pragma omp parallel for default(shared) schedule(static, chunk)\
        private(i) reduction(+:res)
    for (i = 0; i < size; i++)
    {
        res += A[i] * B[i];
    }
    std::cout << "paral res: " << res << ", time: " << clock() - start << std::endl << std::endl;
}

int main()
{
    solve();
    solve();
    solve();

    return 0;
}
