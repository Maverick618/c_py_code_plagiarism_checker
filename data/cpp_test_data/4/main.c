int* lexicalOrder(int n, int* returnSize){
    int *res = (int *)malloc(sizeof(int) * n);
    int num = 1;
    for (int i = 0; i < n; i++) {
        res[i] = num;
        if (num * 10 <= n) {
            num *= 10;
        } else {
            while (num % 10 == 9 || num + 1 > n) {
                num /= 10;
            }
            num++;
        }
    }
    *returnSize = n;
    return res;
}
