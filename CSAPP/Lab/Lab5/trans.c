/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, ii, jj;
    int a0, a1, a2, a3, a4, a5, a6, a7;
    if (M == 32) {
        for (i = 0; i < N; i += 8) {
            for (j = 0; j < M; j += 8) {
                if (i != j) {
                    for (ii = i; ii < i + 8; ii++)
                        for (jj = j; jj < j + 8; jj++)
                            B[jj][ii] = A[ii][jj];
                } else {
                    // A row0
                    a0 = A[i][j]; a1 = A[i][j+1]; a2 = A[i][j+2]; a3 = A[i][j+3]; 
                    a4 = A[i][j+4]; a5 = A[i][j+5]; a6 = A[i][j+6]; a7 = A[i][j+7];
                    // B row0
                    B[i][j] = a0; B[i][j+1] = a1; B[i][j+2] = a2; B[i][j+3] = a3;
                    B[i][j+4] = a4; B[i][j+5] = a5; B[i][j+6] = a6; B[i][j+7] = a7;
                    // A row1
                    a0 = A[i+1][j]; a1 = A[i+1][j+1]; a2 = A[i+1][j+2]; a3 = A[i+1][j+3]; 
                    a4 = A[i+1][j+4]; a5 = A[i+1][j+5]; a6 = A[i+1][j+6]; a7 = A[i+1][j+7];
                    // B row1
                    B[i+1][j] = B[i][j+1]; B[i+1][j+1] = a1; B[i+1][j+2] = a2; B[i+1][j+3] = a3;
                    B[i+1][j+4] = a4; B[i+1][j+5] = a5; B[i+1][j+6] = a6; B[i+1][j+7] = a7;
                    B[i][j+1] = a0; /// attention: update the element above current row of dst matrix
                    // A row2
                    a0 = A[i+2][j]; a1 = A[i+2][j+1]; a2 = A[i+2][j+2]; a3 = A[i+2][j+3]; 
                    a4 = A[i+2][j+4]; a5 = A[i+2][j+5]; a6 = A[i+2][j+6]; a7 = A[i+2][j+7];
                    // B row2
                    B[i+2][j] = B[i][j+2]; B[i+2][j+1] = B[i+1][j+2]; B[i+2][j+2] = a2; B[i+2][j+3] = a3;
                    B[i+2][j+4] = a4; B[i+2][j+5] = a5; B[i+2][j+6] = a6; B[i+2][j+7] = a7;
                    B[i][j+2] = a0; B[i+1][j+2] = a1;
                    // A row3
                    a0 = A[i+3][j]; a1 = A[i+3][j+1]; a2 = A[i+3][j+2]; a3 = A[i+3][j+3]; 
                    a4 = A[i+3][j+4]; a5 = A[i+3][j+5]; a6 = A[i+3][j+6]; a7 = A[i+3][j+7];
                    // B row3
                    B[i+3][j] = B[i][j+3]; B[i+3][j+1] = B[i+1][j+3]; B[i+3][j+2] = B[i+2][j+3]; B[i+3][j+3] = a3;
                    B[i+3][j+4] = a4; B[i+3][j+5] = a5; B[i+3][j+6] = a6; B[i+3][j+7] = a7;
                    B[i][j+3] = a0; B[i+1][j+3] = a1; B[i+2][j+3] = a2;
                    // A row4
                    a0 = A[i+4][j]; a1 = A[i+4][j+1]; a2 = A[i+4][j+2]; a3 = A[i+4][j+3]; 
                    a4 = A[i+4][j+4]; a5 = A[i+4][j+5]; a6 = A[i+4][j+6]; a7 = A[i+4][j+7];
                    // B row4
                    B[i+4][j] = B[i][j+4]; B[i+4][j+1] = B[i+1][j+4]; B[i+4][j+2] = B[i+2][j+4]; B[i+4][j+3] = B[i+3][j+4];
                    B[i+4][j+4] = a4; B[i+4][j+5] = a5; B[i+4][j+6] = a6; B[i+4][j+7] = a7;
                    B[i][j+4] = a0; B[i+1][j+4] = a1; B[i+2][j+4] = a2; B[i+3][j+4] = a3;
                    // A row5
                    a0 = A[i+5][j]; a1 = A[i+5][j+1]; a2 = A[i+5][j+2]; a3 = A[i+5][j+3]; 
                    a4 = A[i+5][j+4]; a5 = A[i+5][j+5]; a6 = A[i+5][j+6]; a7 = A[i+5][j+7];
                    // B row5
                    B[i+5][j] = B[i][j+5]; B[i+5][j+1] = B[i+1][j+5]; B[i+5][j+2] = B[i+2][j+5]; B[i+5][j+3] = B[i+3][j+5];
                    B[i+5][j+4] = B[i+4][j+5]; B[i+5][j+5] = a5; B[i+5][j+6] = a6; B[i+5][j+7] = a7;
                    B[i][j+5] = a0; B[i+1][j+5] = a1; B[i+2][j+5] = a2; B[i+3][j+5] = a3; B[i+4][j+5] = a4; 
                    // A row6
                    a0 = A[i+6][j]; a1 = A[i+6][j+1]; a2 = A[i+6][j+2]; a3 = A[i+6][j+3]; 
                    a4 = A[i+6][j+4]; a5 = A[i+6][j+5]; a6 = A[i+6][j+6]; a7 = A[i+6][j+7];
                    // B row6
                    B[i+6][j] = B[i][j+6]; B[i+6][j+1] = B[i+1][j+6]; B[i+6][j+2] = B[i+2][j+6]; B[i+6][j+3] = B[i+3][j+6];
                    B[i+6][j+4] = B[i+4][j+6]; B[i+6][j+5] = B[i+5][j+6]; B[i+6][j+6] = a6; B[i+6][j+7] = a7;
                    B[i][j+6] = a0; B[i+1][j+6] = a1; B[i+2][j+6] = a2; B[i+3][j+6] = a3; B[i+4][j+6] = a4; 
                    B[i+5][j+6] = a5;
                    // A row7
                    a0 = A[i+7][j]; a1 = A[i+7][j+1]; a2 = A[i+7][j+2]; a3 = A[i+7][j+3]; 
                    a4 = A[i+7][j+4]; a5 = A[i+7][j+5]; a6 = A[i+7][j+6]; a7 = A[i+7][j+7];
                    // B row7
                    B[i+7][j] = B[i][j+7]; B[i+7][j+1] = B[i+1][j+7]; B[i+7][j+2] = B[i+2][j+7]; B[i+7][j+3] = B[i+3][j+7];
                    B[i+7][j+4] = B[i+4][j+7]; B[i+7][j+5] = B[i+5][j+7]; B[i+7][j+6] = B[i+6][j+7]; B[i+7][j+7] = a7;
                    B[i][j+7] = a0; B[i+1][j+7] = a1; B[i+2][j+7] = a2; B[i+3][j+7] = a3; B[i+4][j+7] = a4; 
                    B[i+5][j+7] = a5; B[i+6][j+7] = a6; 
                }
            }
        }
    } else if (M == 64) {
        for (i = 0; i < N; i += 8) {
            // diagonal
            // borrow
            for (j = 0; j < M; j += 8) {
                if (i == j) {
                    // A row0
                    a0 = A[i][j]; a1 = A[i][j+1]; a2 = A[i][j+2]; a3 = A[i][j+3]; 
                    a4 = A[i][j+4]; a5 = A[i][j+5]; a6 = A[i][j+6]; a7 = A[i][j+7];
                    // B row0
                    B[i][j] = a0; B[i][j+1] = a1; B[i][j+2] = a2; B[i][j+3] = a3;
                    B[i][j+4] = a4; B[i][j+5] = a5; B[i][j+6] = a6; B[i][j+7] = a7;
                    // A row1
                    a0 = A[i+1][j]; a1 = A[i+1][j+1]; a2 = A[i+1][j+2]; a3 = A[i+1][j+3]; 
                    a4 = A[i+1][j+4]; a5 = A[i+1][j+5]; a6 = A[i+1][j+6]; a7 = A[i+1][j+7];
                    // B row1
                    B[i+1][j] = B[i][j+1]; B[i+1][j+1] = a1; B[i+1][j+2] = a2; B[i+1][j+3] = a3;
                    B[i+1][j+4] = a4; B[i+1][j+5] = a5; B[i+1][j+6] = a6; B[i+1][j+7] = a7;
                    B[i][j+1] = a0; /// attention: update the element above current row of dst matrix
                    // A row2
                    a0 = A[i+2][j]; a1 = A[i+2][j+1]; a2 = A[i+2][j+2]; a3 = A[i+2][j+3]; 
                    a4 = A[i+2][j+4]; a5 = A[i+2][j+5]; a6 = A[i+2][j+6]; a7 = A[i+2][j+7];
                    // B row2
                    B[i+2][j] = B[i][j+2]; B[i+2][j+1] = B[i+1][j+2]; B[i+2][j+2] = a2; B[i+2][j+3] = a3;
                    B[i+2][j+4] = a4; B[i+2][j+5] = a5; B[i+2][j+6] = a6; B[i+2][j+7] = a7;
                    B[i][j+2] = a0; B[i+1][j+2] = a1;
                    // A row3
                    a0 = A[i+3][j]; a1 = A[i+3][j+1]; a2 = A[i+3][j+2]; a3 = A[i+3][j+3]; 
                    a4 = A[i+3][j+4]; a5 = A[i+3][j+5]; a6 = A[i+3][j+6]; a7 = A[i+3][j+7];
                    // B row3
                    B[i+3][j] = B[i][j+3]; B[i+3][j+1] = B[i+1][j+3]; B[i+3][j+2] = B[i+2][j+3]; B[i+3][j+3] = a3;
                    B[i+3][j+4] = a4; B[i+3][j+5] = a5; B[i+3][j+6] = a6; B[i+3][j+7] = a7;
                    B[i][j+3] = a0; B[i+1][j+3] = a1; B[i+2][j+3] = a2;
                    // A row4
                    a0 = A[i+4][j]; a1 = A[i+4][j+1]; a2 = A[i+4][j+2]; a3 = A[i+4][j+3]; 
                    a4 = A[i+4][j+4]; a5 = A[i+4][j+5]; a6 = A[i+4][j+6]; a7 = A[i+4][j+7];
                    // B row4
                    B[i+4][j] = B[i][j+4]; B[i+4][j+1] = B[i+1][j+4]; B[i+4][j+2] = B[i+2][j+4]; B[i+4][j+3] = B[i+3][j+4];
                    B[i+4][j+4] = a4; B[i+4][j+5] = a5; B[i+4][j+6] = a6; B[i+4][j+7] = a7;
                    B[i][j+4] = a0; B[i+1][j+4] = a1; B[i+2][j+4] = a2; B[i+3][j+4] = a3;
                    // A row5
                    a0 = A[i+5][j]; a1 = A[i+5][j+1]; a2 = A[i+5][j+2]; a3 = A[i+5][j+3]; 
                    a4 = A[i+5][j+4]; a5 = A[i+5][j+5]; a6 = A[i+5][j+6]; a7 = A[i+5][j+7];
                    // B row5
                    B[i+5][j] = B[i][j+5]; B[i+5][j+1] = B[i+1][j+5]; B[i+5][j+2] = B[i+2][j+5]; B[i+5][j+3] = B[i+3][j+5];
                    B[i+5][j+4] = B[i+4][j+5]; B[i+5][j+5] = a5; B[i+5][j+6] = a6; B[i+5][j+7] = a7;
                    B[i][j+5] = a0; B[i+1][j+5] = a1; B[i+2][j+5] = a2; B[i+3][j+5] = a3; B[i+4][j+5] = a4; 
                    // A row6
                    a0 = A[i+6][j]; a1 = A[i+6][j+1]; a2 = A[i+6][j+2]; a3 = A[i+6][j+3]; 
                    a4 = A[i+6][j+4]; a5 = A[i+6][j+5]; a6 = A[i+6][j+6]; a7 = A[i+6][j+7];
                    // B row6
                    B[i+6][j] = B[i][j+6]; B[i+6][j+1] = B[i+1][j+6]; B[i+6][j+2] = B[i+2][j+6]; B[i+6][j+3] = B[i+3][j+6];
                    B[i+6][j+4] = B[i+4][j+6]; B[i+6][j+5] = B[i+5][j+6]; B[i+6][j+6] = a6; B[i+6][j+7] = a7;
                    B[i][j+6] = a0; B[i+1][j+6] = a1; B[i+2][j+6] = a2; B[i+3][j+6] = a3; B[i+4][j+6] = a4; 
                    B[i+5][j+6] = a5;
                    // A row7
                    a0 = A[i+7][j]; a1 = A[i+7][j+1]; a2 = A[i+7][j+2]; a3 = A[i+7][j+3]; 
                    a4 = A[i+7][j+4]; a5 = A[i+7][j+5]; a6 = A[i+7][j+6]; a7 = A[i+7][j+7];
                    // B row7
                    B[i+7][j] = B[i][j+7]; B[i+7][j+1] = B[i+1][j+7]; B[i+7][j+2] = B[i+2][j+7]; B[i+7][j+3] = B[i+3][j+7];
                    B[i+7][j+4] = B[i+4][j+7]; B[i+7][j+5] = B[i+5][j+7]; B[i+7][j+6] = B[i+6][j+7]; B[i+7][j+7] = a7;
                    B[i][j+7] = a0; B[i+1][j+7] = a1; B[i+2][j+7] = a2; B[i+3][j+7] = a3; B[i+4][j+7] = a4; 
                    B[i+5][j+7] = a5; B[i+6][j+7] = a6; 
                } else {
                    // dis-diagonal
                    // Phase1
                    for (ii = 0; ii < 4; ii++) {
                        a0 = A[i+ii][j]; a1 = A[i+ii][j+1]; a2 = A[i+ii][j+2]; a3 = A[i+ii][j+3];
                        a4 = A[i+ii][j+4]; a5 = A[i+ii][j+5]; a6 = A[i+ii][j+6]; a7 = A[i+ii][j+7];
                        B[j][i+ii] = a0; B[j+1][i+ii] = a1; B[j+2][i+ii] = a2; B[j+3][i+ii] = a3;
                        B[j][i+ii+4] = a4; B[j+1][i+ii+4] = a5; B[j+2][i+ii+4] = a6; B[j+3][i+ii+4] = a7; 
                    }
                    // Phase2
                    for (ii = 0; ii < 4; ii++) {
                        a0 = A[i+4][j+ii]; a1 = A[i+5][j+ii]; a2 = A[i+6][j+ii]; a3 = A[i+7][j+ii];
                        a4 = B[j+ii][i+4]; a5 = B[j+ii][i+5]; a6 = B[j+ii][i+6]; a7 = B[j+ii][i+7];
                        B[j+ii][i+4] = a0; B[j+ii][i+5] = a1; B[j+ii][i+6] = a2; B[j+ii][i+7] = a3;
                        B[j+ii+4][i] = a4; B[j+ii+4][i+1] = a5; B[j+ii+4][i+2] = a6; B[j+ii+4][i+3] = a7; 
                    }
                    // Phase3
                    for (ii = 0; ii < 4; ii++) {
                        for (jj = 0; jj < 4; jj++) {
                            B[j+jj+4][i+ii+4] = A[i+ii+4][j+jj+4];
                        }
                    }
                }
            }
        }
    } else if (M == 61) {
        for (i = 0; i < N; i += 17) {
            for (j = 0; j < M; j += 17) {
                for (ii = i; ii < ((N < i + 17) ? N : i + 17); ii++) {
                    for (jj = j; jj < ((M < j + 17) ? M : j + 17); jj++) {
                        B[jj][ii] = A[ii][jj];
                    }
                }
            }
        }
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

