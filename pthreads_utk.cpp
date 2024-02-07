#include <iostream>
#include <cstdlib>
#include <cassert>
#include <ctime>
#include <chrono>
#include <random>
#include <pthread.h>
#include <cmath>
using namespace std;

// Define a struct to pass parameters to the thread function
struct LUParams {
    double **a;
    double **l;
    double **u;
    int k;
    int n;
    int start;
    int end;
};

// Function to perform LU decomposition for a range of rows
void* LU_decomposition(void* arg) {
    LUParams* params = (LUParams*)arg;
    double **a = params->a;
    double **l = params->l;
    double **u = params->u;
    int k = params->k;
    int n = params->n;
    int start = params->start;
    int end = params->end;

    for (int i = start; i < end; i++) {
        for (int z = k + 1; z < n; z++) {
            a[i][z] -= l[i][k] * u[k][z];
        }
    }

    pthread_exit(NULL);
}

int main(int argc, const char * argv[]) {
    int n = stoi(argv[1]);
    int t = stoi(argv[2]);
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> dist(-1000.0, 1000.0);

    double **a = new double*[n];
    for (int i = 0; i < n; i++) {
        a[i] = new double[n];
        for (int j = 0; j < n; j++) {
            a[i][j] = dist(gen);
        }
    }

    double **l = new double*[n];
    double **u = new double*[n];
    for (int i = 0; i < n; i++) {
        l[i] = new double[n];
        u[i] = new double[n];
    }

    int *p = new int[n];
    for (int i = 0; i < n; i++) p[i] = i;
    auto start_time = std::chrono::high_resolution_clock::now();
    // Perform LU decomposition
    for (int k = 0; k < n; k++) {
        // Create threads
        int num_threads = t;
        pthread_t threads[num_threads];
        LUParams params[num_threads];

        // Divide the work among threads
        int rows_per_thread = (n - 1 - k) / num_threads;
        int remaining_rows = (n - 1 - k) % num_threads;
        int start_row = k + 1;

        vector<int>rows_per_threads1(n);
        // ll sum=0;
        // vector<ll>prev(n-1-k+1,n-1-k);
        // for(int j=1;j<=(n-1-k);j++){
        //     prev[j]=prev[j-1]+n-1-k
        // }

        for (int i = 0; i < num_threads; i++) {
            int extra_rows = (i < remaining_rows) ? 1 : 0;

            int end_row = start_row + rows_per_thread + extra_rows;
            params[i] = {a, l, u, k, n, start_row, end_row};
            pthread_create(&threads[i], NULL, LU_decomposition, (void*)&params[i]);

            start_row = end_row;
        }

        // Wait for all threads to finish
        for (int i = 0; i < num_threads; i++) {
            pthread_join(threads[i], NULL);
        }
    }
    auto end_time = std::chrono::high_resolution_clock::now();

    // Calculate the duration (time difference) between start and end
    std::chrono::duration<double> duration = end_time - start_time;

    // Convert duration to seconds and output the result
    std::cout << "Execution time: " << duration.count() << " seconds" << std::endl;
    // Free allocated memory
    for (int i = 0; i < n; i++) {
        delete[] a[i];
        delete[] l[i];
        delete[] u[i];
    }
    delete[] a;
    delete[] l;
    delete[] u;
    delete[] p;

    return 0;
}
