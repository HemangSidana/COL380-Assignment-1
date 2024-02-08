#include <iostream>
#include <cstdlib>
#include <cassert>
#include <ctime>
#include <chrono>
#include <random>
#include <omp.h>
using namespace std;

double ** create(int n){
    double ** m= new double * [n];
    for(int i=0;i<n;i++){
        m[i]= new double[n];
    }
    return m;
}

void display(double ** m, int n){
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            cout<<m[i][j]<<" ";
        }
        cout<<'\n';
    }
}

double ** copy(double ** m, int n){
    double ** result= create(n);
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            result[i][j]=m[i][j];
        }
    }
    return result;
}

double ** permute(int * p, double ** a, int n){
    double ** m= create(n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            m[i][j] = a[p[i]][j];
        }
    }
    return m;
}

double ** mult(double ** l, double ** u, int n){
    double **result = create(n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0.0;
            for (int k = 0; k < n; k++) {
                sum += l[i][k] * u[k][j];
            }
            result[i][j] = sum;
        }
    }
    return result;
}

double norm(double ** x, double ** y, int n){
    double ans=0.0;
    for(int i=0;i<n;i++){
        double val=0.0;
        for(int j=0;j<n;j++){
            val+= (x[j][i]-y[j][i])*(x[j][i]-y[j][i]);
        }
        ans+= sqrt(val);
    }
    return ans;
}

int main(int argc, const char * argv[]){
    int n= stoi(argv[1]);
    int t= stoi(argv[2]);
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> dist(-1000.0, 1000.0);

    double **a= create(n);
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            a[i][j]= dist(gen);
        }
    }
    double **arr= copy(a,n);
    double **l= create(n);
    double **u= create(n);
    for(int i=0;i<n;i++) l[i][i]=1.0;
    for(int i=0;i<n;i++){
        for(int j=0;j<i;j++){
            u[i][j]=0.0;
            l[j][i]=0.0;
        }
    }
    int *p = new int[n];
    auto start = std::chrono::high_resolution_clock::now();
    for(int i=0;i<n;i++) p[i]=i;
    for(int k=0;k<n;k++){
        double m=0.0; int j=-1;
        for(int i=k;i<n;i++){
            if(m< abs(a[i][k])){
                m=abs(a[i][k]);
                j=i;
            }
        }
        assert(j!=-1);
        swap(p[k],p[j]);
        for(int z=0;z<n;z++){
            swap(a[k][z],a[j][z]);
        }
        for(int z=0;z<k;z++){
            swap(l[k][z],l[j][z]);
        }
        u[k][k]=a[k][k];
        for(int z=k+1;z<n;z++){
            l[z][k]=a[z][k]/u[k][k];
            u[k][z]=a[k][z];
        }
        #pragma omp parallel for num_threads(t)
        for(int i=k+1;i<n;i++){
            for(int z=k+1;z<n;z++){
                a[i][z]-=l[i][k]*u[k][z];
            }
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    // cout<<"permutation"<<endl;
    // for(int i=0;i<n;i++){
    //     cout<<p[i]<<" ";
    // }
    // cout<<'\n';
    // cout<<"lower"<<endl;
    // display(l,n);
    // cout<<"upper"<<endl;
    // display(u,n);
    // double ** pa= permute(p,arr,n);
    // double ** lu= mult(l,u,n);
    // cout<<"pa"<<endl;
    // display(pa,n);
    // cout<<"lu"<<endl;
    // display(lu,n);
    // cout<<"L2,1 norm of matrix "<<norm(pa,lu,n)<<endl;
    std::chrono::duration<double> duration_seconds = end - start;
    double seconds = duration_seconds.count();
    std::cout << "Time taken: " << seconds << " seconds" << std::endl;
    return 0;
}