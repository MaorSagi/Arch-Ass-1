#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <ctype.h>
#include <memory.h>


typedef struct bignum {
    long number_of_digits;
    long *digit;
    int isNeg; //Positive = 0. Negative = 1
} bignum;

extern void add_bignum(long *, long *, long);

int NUM_OF_DIGITS = 100000;

extern void sub_bignum(long *, long *, long);

extern void mult_bignum(long *, long *, long, long *, long *);

extern void div_bignum(long *, long *, long, long *, long *, long *, long *);

//Stack Variables
bignum bnArr[1024];
bignum trash[1024];
int trashTop = -1;
int top = -1;  /* I.e., empty */
int isDig = 0;

//Functions Declaration
void subFunc(int isLarge, int first);

void mulFunc();

void divFunc();

int StackIsEmpty();

void addNum(struct bignum bignum);

void printFirst();

void addDigit(char a);

void changeSize(bignum *bignum, long toAdd);

//Stack Functions
int StackIsEmpty() {
    return top < 0;
}

void removeFirstZeroes(struct bignum *tmp) {
    int isZeroStart = 0;
    int count = 0;
    for (long i = 0; i < tmp->number_of_digits && isZeroStart == 0; i++) {
        if (*(tmp->digit + i) == 0) {
            count++;
        } else {
            isZeroStart = 1;
        }
    }
    if (count != 0) {
        long tmpDigits[tmp->number_of_digits];
        for (int i = 0; i < tmp->number_of_digits; ++i) {
            ((tmpDigits[i])) = (*(tmp->digit + i));
        }
        tmp->digit = malloc((tmp->number_of_digits - count) * sizeof(long));
        for (int k = 0; k < tmp->number_of_digits - count; ++k) {
            *(tmp->digit + k) = tmpDigits[k + count];
        }
        tmp->number_of_digits = tmp->number_of_digits - count;
    }
}

void removeFirstZeroesMul(struct bignum *tmp) {
    int isZeroStart = 0;
    int count = 0;
    for (long i = 0; i < tmp->number_of_digits && isZeroStart == 0; i++) {
        if (*(tmp->digit + i) == 0) {
            count++;
        } else {
            isZeroStart = 1;
        }
    }
    if (count != 0) {
        long tmpDigits[tmp->number_of_digits];
        for (int i = 0; i < tmp->number_of_digits; ++i) {
            ((tmpDigits[i])) = (*(tmp->digit + i));
        }
        tmp->digit = realloc(tmp->digit, (tmp->number_of_digits - count) * sizeof(long));
        for (int k = 0; k < tmp->number_of_digits - count; ++k) {
            *(tmp->digit + k) = tmpDigits[k + count];
        }
        tmp->number_of_digits = tmp->number_of_digits - count;
    }
}

void push(struct bignum bn) {
    removeFirstZeroes(&bn);
    top++;
    bnArr[top] = bn;
}

void pushTrash(struct bignum bn) {
    removeFirstZeroes(&bn);
    trashTop++;
    trash[trashTop] = bn;
}

bignum popTrash() {
    if (StackIsEmpty()) {
        fprintf(stderr, "Can't pop element from stack: stack is empty.\n");
        exit(1);  /* Exit, returning error code. */
    }
    trashTop--;
    return trash[trashTop + 1];

}

void pushMul(struct bignum bn) {
    //removeFirstZeroesMul(&bn);
    top++;
    bnArr[top] = bn;
}

bignum peek() {
    return bnArr[top];
}

bignum pop() {
    if (StackIsEmpty()) {
        fprintf(stderr, "Can't pop element from stack: stack is empty.\n");
        exit(1);  /* Exit, returning error code. */
    }
    top--;
    return bnArr[top + 1];

}

void addZero(struct bignum *tmp);

int checkIfZero(bignum toCheck);

void revBignum(struct bignum *toRev) {
    long tmp[toRev->number_of_digits];
    for (int i = 0; i < toRev->number_of_digits; ++i) {
        ((tmp[i])) = (*(toRev->digit + i));
    }
    for (int j = 0; j < toRev->number_of_digits; ++j) {
        (*(toRev->digit + j)) = ((tmp[toRev->number_of_digits - 1 - j]));
    }

}

void remFirstZero(struct bignum *tmp) {
    //tmp->digit = realloc((size - 1) * sizeof(long));
    for (int k = 0; k < tmp->number_of_digits - 1; ++k) {
        *(tmp->digit + k) = *(tmp->digit + k + 1);
    }
}

//Assembly Function
void addFunc() {
    bignum bn1 = pop();
    bignum bn2 = pop();
    if (bn1.isNeg == 1 && bn2.isNeg == 0) {
        bn1.isNeg = 0;
        push(bn1);
        push(bn2);
        subFunc(0, 1);
    } else if (bn1.isNeg == 0 && bn2.isNeg == 1) {
        bn2.isNeg = 0;
        push(bn2);
        push(bn1);
        subFunc(0, 1);
    } else {
        if (bn1.number_of_digits > bn2.number_of_digits) {
            long num = bn2.number_of_digits;
            for (int i = 0; i < (bn1.number_of_digits - num); ++i) {
                addZero(&bn2);
                bn2.number_of_digits++;
            }
        } else if (bn1.number_of_digits < bn2.number_of_digits) {
            long num = bn1.number_of_digits;
            for (int i = 0; i < (bn2.number_of_digits - num); ++i) {
                addZero(&bn1);
                bn1.number_of_digits++;
            }
        }

        addZero(&bn1);//+1 for overflow option
        bn1.number_of_digits++;
        addZero(&bn2);//+1 for overflow option
        bn2.number_of_digits++;

        revBignum(&bn1);
        revBignum(&bn2);

        addZero(&bn1);//+1 for overflow option
        bn1.number_of_digits++;
        addZero(&bn2);//+1 for overflow option
        bn2.number_of_digits++;

        add_bignum(bn1.digit, bn2.digit, bn1.number_of_digits);

        remFirstZero(&bn1);
        bn1.number_of_digits--;
        //Free bn2 digit
        revBignum(&bn1);

        if (bn1.isNeg == 1 && bn2.isNeg == 1) {
            bn1.isNeg = 1;
        } else {
            bn1.isNeg = 0;
        }
        push(bn1);
        pushTrash(bn2);
    }
}

void addZero(struct bignum *tmp) {
    long tmp1[tmp->number_of_digits];
    for (int i = 0; i < tmp->number_of_digits + 1; ++i) {
        ((tmp1[i])) = (*(tmp->digit + i));
    }
    tmp->digit = malloc((tmp->number_of_digits * sizeof(long)) + sizeof(long));
    for (long i = 1; i < tmp->number_of_digits + 1; i++) {
        *(tmp->digit + i) = tmp1[i - 1];
    }
    *(tmp->digit) = 0;
}

int is_larger(long *f, long *s, long len) {
    for (int i = 0; i < (int) (len); i++) {
        if (f[i] < s[i])
            return 1;
        if (f[i] > s[i])
            return 0;
    }
    return 0;
}

void subFunc(int isLarge, int first) {
    bignum bn1 = pop();
    bignum bn2 = pop();
    if (first == 0 && !((bn1.isNeg == 1 && bn2.isNeg == 0) || (bn1.isNeg == 0 && bn2.isNeg == 1))) {
        push(bn1);
        push(bn2);
        subFunc(0, 1);
    } else {
        if (bn1.isNeg == 0 && bn2.isNeg == 1) {
            bn1.isNeg = 1;
            push(bn2);
            push(bn1);
            addFunc();
        } else if (bn1.isNeg == 1 && bn2.isNeg == 0) {
            bn1.isNeg = 0;
            push(bn2);
            push(bn1);
            addFunc();
        } else if (bn1.isNeg == 1 && bn2.isNeg == 1) {
            bn1.isNeg = 0;
            bn2.isNeg = 0;
            push(bn2);
            push(bn1);
            subFunc(0, 0);
        } else {
            if (bn1.number_of_digits > bn2.number_of_digits) {
                long num = bn2.number_of_digits;
                for (int i = 0; i < (bn1.number_of_digits - num); ++i) {
                    addZero(&bn2);
                    bn2.number_of_digits++;
                }
            } else if (bn1.number_of_digits < bn2.number_of_digits) {
                long num = bn1.number_of_digits;
                for (int i = 0; i < (bn2.number_of_digits - num); ++i) {
                    addZero(&bn1);
                    bn1.number_of_digits++;
                }
            }
            int larger = is_larger(bn1.digit, bn2.digit, bn1.number_of_digits);
            if (larger == 1) {
                push(bn1);
                push(bn2);
                subFunc(1, 1);
            } else {
                addZero(&bn1);//+1 for overflow option
                bn1.number_of_digits++;
                addZero(&bn2);//+1 for overflow option
                bn2.number_of_digits++;


                revBignum(&bn1);
                revBignum(&bn2);

                addZero(&bn1);//+1 for overflow option
                bn1.number_of_digits++;
                addZero(&bn2);//+1 for overflow option
                bn2.number_of_digits++;

                sub_bignum(bn1.digit, bn2.digit, bn1.number_of_digits);
                //Free bn2 digit
                remFirstZero(&bn1);
                bn1.number_of_digits--;

                

                revBignum(&bn1);

                if (isLarge == 1) {
                    bn1.isNeg = 1;
                }

                push(bn1);
                pushTrash(bn2);
            }
        }
    }
}

void divFunc() {
    bignum bn2 = pop();
    bignum bn1 = pop();
    if(checkIfZero(bn2) == 0){
        printf("divided by Zero. Pushed 0 to the stack \n");
        for (int i = 0; i < bn2.number_of_digits; ++i) {
            *(bn2.digit + i) = 0;
        }
        push(bn2);
    }
    else {
        if (bn1.number_of_digits > bn2.number_of_digits) {
            long num = bn2.number_of_digits;
            for (int i = 0; i < (bn1.number_of_digits - num); ++i) {
                addZero(&bn2);
                bn2.number_of_digits++;
            }
        } else if (bn1.number_of_digits < bn2.number_of_digits) {
            long num = bn1.number_of_digits;
            for (int i = 0; i < (bn2.number_of_digits - num); ++i) {
                addZero(&bn1);
                bn1.number_of_digits++;
            }
        }
        /*
        addZero(&bn1);//+1 for overflow option
        bn1.number_of_digits++;
        addZero(&bn2);//+1 for overflow option
        bn2.number_of_digits++;
        */
        revBignum(&bn1);
        revBignum(&bn2);

        addZero(&bn1);//+1 for overflow option
        bn1.number_of_digits++;
        addZero(&bn2);//+1 for overflow option
        bn2.number_of_digits++;

        long* result;
        long* factor;
        int len = (int)bn1.number_of_digits;
        result=malloc(sizeof(long)*(len + 1));
        factor=malloc(sizeof(long)*(len + 1));


        for(int i=1; i<len + 1;i++){
            result[i]=0;
            factor[i]=0;
        }
        factor[1] = 1;
        long* tmp1;
        long* tmp2;
        tmp2=malloc(sizeof(long)*(len + 1));
        tmp1=malloc(sizeof(long)*(len + 1));
        for(int i=1; i<len + 1;i++){
            tmp1[i]=0;
            tmp2[i]=0;
        }
/*
        long *result = malloc((bn1.number_of_digits  * 2) * sizeof(long));
        for (int k = 0; k < bn1.number_of_digits; ++k) {
            *(result + k) = 0;
        }

        long *factor = malloc((bn1.number_of_digits  * 2) * sizeof(long));
        factor[0] = 0;
        factor[1] = 1;
        for (int j = 2; j < bn1.number_of_digits; ++j) {
            *(factor + j) = 0;
        }


        long *tmp1 = malloc((bn1.number_of_digits * 2) * sizeof(long));
        for (int k = 0; k < bn1.number_of_digits; ++k) {
            *(tmp1 + k) = 0;
        }
        long *tmp2 = malloc((bn1.number_of_digits * 2) * sizeof(long));
        for (int k = 0; k < bn1.number_of_digits; ++k) {
            *(tmp2 + k) = 0;
        }
*/
        int size = (int)bn1.number_of_digits;
        for (int j = 0; j < (size % 2) ; ++j) {
            addZero(&bn1);//+1 for overflow option
            bn1.number_of_digits++;
            addZero(&bn2);//+1 for overflow option
            bn2.number_of_digits++;
        }


        div_bignum(bn1.digit, bn2.digit, bn1.number_of_digits, result, factor, tmp1, tmp2);

        /*free(result);
        free(factor);
        free(tmp1);
        free(tmp2);*/

        remFirstZero(&bn1);
        bn1.number_of_digits--;
        //Free bn2 digit
        revBignum(&bn1);

        if (bn1.isNeg == bn2.isNeg) {
            bn1.isNeg = 0;
        } else {
            bn1.isNeg = 1;
        }
        push(bn1);
        pushTrash(bn2);
    }
}

int checkIfZero(struct bignum toCheck){
    for (int i = 0; i < toCheck.number_of_digits; ++i) {
        if (toCheck.digit[i] != 0)
            return 1;
    }
    return 0;
}

void mulFunc() {
    bignum bn1 = pop();
    bignum bn2 = pop();
    if (bn1.number_of_digits > bn2.number_of_digits) {
        long num = bn2.number_of_digits;
        for (int i = 0; i < (bn1.number_of_digits - num); ++i) {
            addZero(&bn2);
            bn2.number_of_digits++;
        }
    } else if (bn1.number_of_digits < bn2.number_of_digits) {
        long num = bn1.number_of_digits;
        for (int i = 0; i < (bn2.number_of_digits - num); ++i) {
            addZero(&bn1);
            bn1.number_of_digits++;
        }
    }
    int larger = is_larger(bn1.digit, bn2.digit, bn1.number_of_digits);
    if (larger == 1) {
        push(bn1);
        push(bn2);
        mulFunc();
    } else {
        long num = bn2.number_of_digits;
        for (int j = 0; j < num; ++j) {

            addZero(&bn1);//+1 for overflow option
            bn1.number_of_digits++;
            addZero(&bn2);//+1 for overflow option
            bn2.number_of_digits++;
        }

        revBignum(&bn1);
        revBignum(&bn2);

        addZero(&bn1);//+1 for overflow option
        bn1.number_of_digits++;
        addZero(&bn2);//+1 for overflow option
        bn2.number_of_digits++;

        long *result = malloc(2 * bn1.number_of_digits * sizeof(long));
        for (int k = 0; k < 2 * bn1.number_of_digits; ++k) {
            result[k] = 0;
        }

        long *tmp = malloc(2 * bn1.number_of_digits * sizeof(long));
        for (int k = 0; k < 2 * bn1.number_of_digits; ++k) {
            tmp[k] = 0;
        }


        mult_bignum(bn1.digit, bn2.digit, bn1.number_of_digits, result, tmp);
        //Free bn2 digit, result digit, tmp digit
        remFirstZero(&bn1);
        bn1.number_of_digits--;

        

        revBignum(&bn1);

        if (bn1.isNeg == bn2.isNeg) {
            bn1.isNeg = 0;
        } else {
            bn1.isNeg = 1;
        }

        pushMul(bn1);
        pushTrash(bn2);
    }
}

struct bignum bn;

int main(int argc, char **argv) {
    /*long *f = malloc(3 * sizeof(long));
    long *s = malloc(3 * sizeof(long));
    f[0] = 0;
    f[1] = 5;
    f[2] = 0;
    s[0] = 0;
    s[1] = 3;
    s[2] = 0;
    sub_bignum(f, s, 2);
    for (int i = 0; i < 3; ++i) {
        printf("%lu", f[i]);
    }
    */
    FILE *readingFile = stdin;
    char c;

    int negCheck = 0;

    bn.number_of_digits = 0;
    bn.digit = (long *) malloc(NUM_OF_DIGITS * sizeof(long));
    bn.isNeg = 0;

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "<") == 0)
            readingFile = fopen(argv[++i], "r");
    }

    while ((c = (char) getc(readingFile)) != 'q') {
        switch (c) {
            case 'p':
                printFirst();
                isDig = 0;
                break;
            case 'c':
                top = -1;
                isDig = 0;
                break;
            case '_':
                negCheck = 1;
                isDig = 0;
                break;
            case '+':
                isDig = 0;
                addFunc();
                break;
            case '-':
                isDig = 0;
                subFunc(0, 0);
                break;
            case '*':
                isDig = 0;
                mulFunc();
                break;
            case '/':
                isDig = 0;
                divFunc();
                break;
            default:
                if (c <= 32) {
                    if (bn.number_of_digits > 0) {
                        bn.isNeg = negCheck;
                        addNum(bn);
                        negCheck = 0;
//                        if (isDig)
//                            bn.digit = malloc(sizeof(long) * bn.number_of_digits);
                    }
                } else if (isdigit(c)) {
                    isDig = 1;
                    addDigit(c);
                }
                break;
        }
    }
    for (int j = 0; j < top; ++j) {
        free(trash[j].digit);
    }
    for (int j = 0; j < top; ++j) {
        free(bnArr[j].digit);
    }
}

//C Functions
void addDigit(char a) {
    if (bn.number_of_digits * sizeof(long) >= NUM_OF_DIGITS * sizeof(long)) {
        bn.digit = realloc(bn.digit, bn.number_of_digits * sizeof(long) + (NUM_OF_DIGITS * sizeof(long)));
    }
    bn.digit[bn.number_of_digits] = a - '0';
    bn.number_of_digits++;
}

void printFirst() {
    bignum tmp = peek();
    int wasPrinted = 0;
    int isZeroStart = 0;
    if (tmp.isNeg == 1) {
        printf("-");
    }
    long toPrint[tmp.number_of_digits * sizeof(long)];
    for (int j = 0; j < tmp.number_of_digits; ++j) {
        toPrint[j] = *(tmp.digit + j);
    }
    for (long i = 0; i < tmp.number_of_digits; i++) {
        if (*(tmp.digit + i) != 0) {
            isZeroStart = 1;
            wasPrinted = 1;
            printf("%lu", toPrint[i]);
        } else if (isZeroStart != 0) {
            printf("%lu", toPrint[i]);
            wasPrinted = 1;
        }
    }
    if (wasPrinted == 0) {
        printf("%d", 0);
    }
    printf("\n");
}

void addNum(struct bignum bignum) {
    if (bignum.number_of_digits > 0) {
        struct bignum tmp = {
                tmp.number_of_digits = bignum.number_of_digits,
                tmp.digit = malloc(bignum.number_of_digits * sizeof(long)),
                tmp.isNeg = bignum.isNeg};
        for (int i = 0; i < tmp.number_of_digits; ++i) {
            tmp.digit[i] = bignum.digit[i];
        }
        push(tmp);
        bn.number_of_digits = 0;
        bn.digit = realloc(bn.digit, NUM_OF_DIGITS * sizeof(long));
        for (int j = 0; j < NUM_OF_DIGITS; ++j) {
            *(bn.digit + j) = 0;
        }
    }
}

void changeSize(struct bignum *bn, long toAdd) {
    bn->digit = (long *) realloc(bn->digit, sizeof(bn) + (toAdd * sizeof(long)));
    for (int i = (int) bn->number_of_digits; i < toAdd; ++i) {
        *(bn->digit + i) = 0;
    }
}


