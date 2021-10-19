#include <xmmintrin.h>
#include <stdio.h>
#include <time.h>

__m64 set_pi8(int8_t* arr)
{
    return _mm_set_pi8(arr[0], arr[1], arr[2], arr[3], arr[4], arr[5], arr[6], arr[7]);
}

__m64* set_pi16(int16_t* arr)
{
    return new __m64[2]
    {
        _mm_set_pi16(arr[4], arr[5], arr[6], arr[7]),
        _mm_set_pi16(arr[0], arr[1], arr[2], arr[3]),
    };
}

__m64 unpacklo_pi8(__m64 vec)
{
    __m64 zero{0};
    __m64 cmp = _mm_cmpgt_pi8(zero, vec);
    return _mm_unpacklo_pi8(vec, cmp);
}

__m64 unpackhi_pi8(__m64 vec)
{
    __m64 zero{0};
    __m64 cmp = _mm_cmpgt_pi8(zero, vec);
    return _mm_unpackhi_pi8(vec, cmp);
}

__m64* byteToWord(__m64 vec)
{
    return new __m64[2]
    {
        unpacklo_pi8(vec),
        unpackhi_pi8(vec)
    };
}

void solve()
{
    srand(time(0));
    int8_t A[8];
    int8_t B[8];
    int8_t C[8];
    int16_t D[8];
    __m64 *vecA, *vecB, *vecC, *vecD, *res = new __m64[2];

    for (int i = 0; i < 8; i++) 
    {
        A[i] = rand() %  128 - rand() % 128;
        B[i] = rand() % 128 - rand() % 128;
        C[i] = rand() % 128 - rand() % 128;
        D[i] = rand() % 32768 - rand() % 32768;
        printf("%i ", (A[i]+B[i])*C[i]+D[i]);
    }
    puts("");

    vecA = byteToWord(set_pi8(A));
    vecB = byteToWord(set_pi8(B));
    vecC = byteToWord(set_pi8(C));
    vecD = set_pi16(D);

    res[0] = _mm_adds_pi16(vecD[0], _mm_mullo_pi16(vecC[0], _mm_adds_pi16(vecA[0], vecB[0])));
    res[1] = _mm_adds_pi16(vecD[1], _mm_mullo_pi16(vecC[1], _mm_adds_pi16(vecA[1], vecB[1])));

    printf("%i %i %i %i ", _mm_extract_pi16(res[1], 3), _mm_extract_pi16(res[1], 2), _mm_extract_pi16(res[1], 1), _mm_extract_pi16(res[1], 0));
    printf("%i %i %i %i\n", _mm_extract_pi16(res[0], 3), _mm_extract_pi16(res[0], 2), _mm_extract_pi16(res[0], 1), _mm_extract_pi16(res[0], 0));

    _mm_empty();
}

int main()
{
    solve();

    return 0;
}