#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <limits.h>

#define pi 3.14159265359
struct sigaction act;
int flag = 1;

void countExp(double start, double step, int deep) {
    
    double* coeffs = (double*)malloc((deep+1)*sizeof(double));
    coeffs[0] = exp(start);
    long long k = 1;
    
    for (int i = 1; i <= deep; ++i){
        coeffs[i] = (exp(start)/k);
        k *= (k+1);
    }
    
    double sum = 0;
    double delta = step;
    while(flag) {
        for (int i = 0; i <= deep; ++i)
            sum += pow(delta, i)*coeffs[i];
        printf("sum: %lf\n", sum);
        sum = 0;
        delta += step;
        sleep(1);
    }
    free(coeffs);
    flag = 1;
}

void countLn(double start, double step, double deep) {
    
    double* coeffs = (double*)malloc((deep+1)*sizeof(double));
    coeffs[0] = log(start);

    for (int i = 1; i <= deep; ++i){
        coeffs[i] = 1/(pow(start, i)*i);
    }
    
    double sum = 0;
    double delta = step;
    while(flag) {
        for (int i = 0; i <= deep; ++i)
            sum += pow(delta, i)*coeffs[i];
        printf("sum: %lf\n", sum);
        sum = 0;
        delta += step;
        sleep(1);
    }
    free(coeffs);
    flag = 1;
}

void countSin(double start, double step, double deep) {

    double* coeffs = (double*)malloc((deep+1)*sizeof(double));
    coeffs[0] = sin(start);
    long long k = 1;

    for (int i = 1; i <= deep; ++i){
        if (i%2 == 0)
            coeffs[i] = sin(start)/k;
        else
            coeffs[i] = cos(start)/k;
        if (i%4 == 2 || i%4 == 3)
            coeffs[i] *= (-1);
        k *= (k+1);
    }

    double sum = 0;
    double delta = step;
    
    while(flag) {
        for (int i = 0; i <= deep; ++i)
            sum += pow(delta, i)*coeffs[i];
        printf("sum: %lf\n", sum);
        sum = 0;
        delta += step;
        sleep(1);
    }
    free(coeffs);
    flag = 1;
}

void input() {
    double start, step, deep;
    char* func = (char*)malloc(sizeof(char)*5);
    printf("\ncommand: ");
    scanf("%s %lf %lf %lf", func, &start, &step, &deep);
    if (strcmp(func, "exp")==0) {
        countExp(start, step, deep);
    }else if (strcmp(func, "ln")==0) {
        countLn(start, step, deep);
    }else if (strcmp(func, "sin")==0) {
        countSin(start, step, deep);
    }else if (strcmp(func, "cos")==0) {
        countSin(start + pi/2, step, deep);
    }else{
        printf("wrong command");
        input();
    }
}

static void input_handler(int signum) {
    flag = 0;
    input();
}

int main() {
    
    memset(&act, 0, sizeof(act));
    act.sa_handler = input_handler;
    sigaction(SIGINT, &act, NULL);
    
    input();

    return 0;
}
