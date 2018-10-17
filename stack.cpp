#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define ERROROK 0
#define ERRORDATA 1
#define ERRORCAP 2
#define ERRORSIZE 3
#define ERRORPOP 4

#define $(x) std::cout<<"~"<<#x " = "<<x<<"\n";
#define DUMP(ERRNUMBER,stack) { printf("~In File: %s\n~In Line: %d\n~In Function: %s\n", __FILE__, __LINE__, __FUNCTION__); \
                                printf("~stack " #stack " [0x%X]\n~{\n~   Capacity = %u\n~   Size = %u\n~   data [0x%X]:\n~",  (stack), (stack)->StackCapacity,\
                                (stack)->StackSize, (stack)->data);\
                                int bufn = (stack)->StackSize;\
                                for (int i = 0; i < bufn; i++)\
                                { \
                                    printf("      data[%d] = %f\n~", i, (stack)->data[i]);\
                                } \
                                printf("   hash = %f\n~", make_hash (stack));\
                                printf("   StackHash = %f\n~", (stack)->StackHash);\
                                printf("   INFO: \n~");\
                                switch (ErNum)\
                                { \
                                    case ERROROK:\
                                        printf("      Stack is OK\n~");\
                                        break;\
                                    case ERRORDATA:\
                                        printf("      Data or hash was damaged\n~");\
                                        break;\
                                    case ERRORCAP:\
                                        printf("      Capacity is not suitable\n~");\
                                        break;\
                                    case ERRORSIZE:\
                                        printf("      Size is not suitable\n~");\
                                        break;\
                                    case ERRORPOP:\
                                        printf("      Can't pop from the stack");\
                                        break;\
                                    default:\
                                        printf("      Stack is OK\n~");\
                                }\
                                printf("}\n\n");\
                              }

#define UTEST(result,wanted) {if ( !iszero(result - wanted))\
                                    {\
                                        printf("~Error in testing: in File: %s\n~In Line: %d\n~In Function: %s\n", __FILE__, __LINE__, __FUNCTION__)\
                                        printf("~Test result " #result " = %f is not correct ( != %f)", result, wanted)\
                                    }\
                            }

typedef double stack_type;
typedef struct MyStack MyStack;

const unsigned int StartCapacity = 10;
int ErNum = ERROROK;

struct MyStack
{
    stack_type* data;
    unsigned int StackSize;
    unsigned int StackCapacity;
    stack_type StackHash;
};

int iszero(double x);
void stack_Ctor(MyStack*);
void stack_Dtor(MyStack*);
void stack_Clear(MyStack*);
void stack_push(MyStack*, stack_type);
stack_type stack_pop(MyStack*);
void stack_extend (MyStack*);
void stack_contract (MyStack*);
stack_type make_hash (MyStack*);
int stack_is_OK(MyStack*);
int test_stack(MyStack*);
int test_error_size(MyStack*);
int test_error_capacity(MyStack*);
int test_error_data(MyStack*);

int main()
{
    MyStack sStack;
    stack_Ctor(&sStack);

    if (test_stack(&sStack)) printf("Stack is OK\n");
    else printf("Stack is wrong\n");

    stack_Dtor(&sStack);
    return 0;
}

//************************************
/// Compares double parameter with 0.
///
/// Parameters: [in] double x
///
/// Output: 1 if variable equals to 0 (less than 0.000001),\n
///         0 if variable is bigger than 0.000001
//************************************

int iszero( double x )
{
    return ( fabs( x )<0.000001 ) ? 1 : 0;
}

//************************************
/// Constructs MyStack structure and initialize it.
///
/// Parameters: [in] struct MyStack* s - pointer to MyStack structure
///
///
/// Output: void
///
//************************************

void stack_Ctor(MyStack* s)
{
    assert(s);

    s->data = (stack_type*) calloc(10, sizeof(stack_type));

    s->StackSize = 0;
    s->StackCapacity = StartCapacity;
    for (unsigned int i = 0; i < (s->StackCapacity); i++)
    {
        s->data[i] = NAN;
    }

    s->StackHash = make_hash(s);
}

//************************************
/// Deletes MyStack structure and its data.
///
/// Parameters: [in] struct MyStack* s - pointer to MyStack structure
///
///
/// Output: void
///
//************************************

void stack_Dtor(MyStack* s)
{
    assert(s);

    for (;s->StackSize >= 1; s->StackSize --)
    {
        s->data[s->StackSize - 1] = NAN;
    }

    free(s->data);
    s->data = NULL;
    s->StackSize = -1;
    s->StackCapacity = -1;
    s->StackHash = NAN;
    s = NULL;
}

//************************************
/// Clears MyStack structure's data.
///
/// Parameters: [in] struct MyStack* s - pointer to MyStack structure
///
///
/// Output: void
///
//************************************


void stack_Clear(MyStack* s)
{
    assert(s);

    for (;s->StackSize >= 1; s->StackSize --)
    {
        s->data[s->StackSize - 1] = NAN;
    }

    s->data = (stack_type*) realloc(s->data, StartCapacity * sizeof(stack_type));
    s->StackCapacity = StartCapacity;
    s->StackHash = make_hash(s);
}

//************************************
/// Inserts an element on the top of stack
///
/// Parameters: [in] struct MyStack* s - pointer to MyStack structure
///
///
/// Output: gives info about stack in console if there are any troubles
///
//************************************

void stack_push(MyStack* s, stack_type val)
{
    assert(s);
    if ( !stack_is_OK(s)) DUMP(ErNum,s);

    if (s->StackSize + 1 < s->StackCapacity)
    {
        s->data[s->StackSize] = val;
        s->StackSize ++;
    }
    else
    {
        stack_extend(s);
        s->data[s->StackSize] = val;
        s->StackSize ++;
    }
    s->StackHash = make_hash(s);
    if ( !stack_is_OK(s)) DUMP(ErNum,s);
}

//************************************
/// Takes one element from the top of stack
///
/// Parameters: [in] struct MyStack* s - pointer to MyStack structure
///
///
/// Output: stack_type element from the top of the stack, gives info about stack in console if there are any troubles
///
//************************************

stack_type stack_pop(MyStack* s)
{
    assert(s);
    if(s->StackSize == 0)
    {
        ErNum = 4;
        DUMP(ErNum, s)
        return NAN;
    }

    if ( !stack_is_OK(s)) DUMP(ErNum,s);

    stack_type buf = s->data[--(s->StackSize)];
    s->data[(s->StackSize)] = NAN;

    if (((s->StackCapacity) >= 4 * (s->StackSize)) && (s->StackCapacity > 10))
    {
        stack_contract(s);
    }

    s->StackHash = make_hash(s);
    if ( !stack_is_OK(s)) DUMP(ErNum,s);
    return buf;

}

//************************************
/// Extends the array in the stack.
///
/// Parameters: [in] struct MyStack* s - pointer to MyStack structure
///
/// Output: changes the size of arrow in stack.
///
//************************************

void stack_extend (MyStack* s)
{
    assert(s);
    s->data = (stack_type*) realloc(s->data, (s->StackCapacity) * sizeof(stack_type) * 2);
    s->StackCapacity *= 2;
    for (unsigned int i = (s->StackSize); i < (s->StackCapacity); i++)
    {
        s->data[i] = NAN;
    }
}

//************************************
/// Contracts the array in the stack.
///
/// Parameters: [in] struct MyStack* s - pointer to MyStack structure
///
/// Output: changes the size of arrow in stack.
///
//************************************

void stack_contract (MyStack* s)
{
    assert(s);
    s->data = (stack_type*) realloc(s->data, (s->StackCapacity) * sizeof(stack_type) / 2 );
    s->StackCapacity /= 2;
}

//************************************
/// Hashes the array in the stack.
///
/// Parameters: [in] struct MyStack* s - pointer to MyStack structure
///
/// Output: changes the global variable StackHash.
///
//************************************

stack_type make_hash (MyStack* s)
{
    assert(s);
    stack_type hashSum = 0;
    for (unsigned int i=0; i < s->StackSize; ++i)
    {
        hashSum = 2.5 * hashSum + s->data[i];
        hashSum = hashSum / 10;
    }
    return hashSum;
}

//************************************
/// Checks if stack is fine or not, and writes error code in global variable
///
/// Parameters: [in] struct MyStack* s - pointer to MyStack structure\n
///
/// Output: 1 if stack is fine\n 0 if not,\n changes the global variable ErNum (0 - all is right, 1 - data in stack is damaged\n 2 - not suitable stack capacity,\n
/// 3 - not suitable stack size)
///
//************************************

int stack_is_OK(MyStack* s)
{
    assert(s);
    int not_error = 1;

    if ( !iszero(make_hash(s) - (s->StackHash))){                                                                                                                       ErNum = ERRORDATA; not_error = 0;}
    if ( !isfinite(s->StackCapacity) || ((s->StackCapacity) < StartCapacity) || (((s->StackCapacity) % StartCapacity) != 0) || ((s->StackSize) > (s->StackCapacity))){  ErNum = ERRORCAP;  not_error = 0;}
    if ( !isfinite(s->StackSize) || (((s->StackSize) > (s->StackCapacity)) && !isfinite(s->data[s->StackSize-1])) || isfinite(s->data[s->StackSize])) {                 ErNum = ERRORSIZE; not_error = 0;}

    return not_error;
}

//************************************
/// Tests stack_is_OK in case of StackSize
///
/// Parameters: [in] struct MyStack* s - pointer to MyStack structure\n
///
/// Output: 1 if test was passed, 0 if not and calls for DUMP
///
//************************************

int test_error_size(MyStack* stak)
{
    assert(stak);
    MyStack s = *stak;
    stack_Ctor(&s);

    for(double i = 2.01; i < 44.01; i = i + 1.0)
    {
        stack_push((&s), 5.023 + i);
    }
    //
    s.StackSize = 20;
    if ((stack_is_OK(&s) != 0) || (ErNum != ERRORSIZE))
        {
            printf("~In %s, line %d, function: %s\n   ~Error in stack_is_OK(with error in size), error code = %d\n", __FILE__, __LINE__,__FUNCTION__, ErNum);
            DUMP (ErNum,&s)
            ErNum = 0;
            return 0;
        }
    ErNum = 0;
    //
    s.StackSize = 150;
    if ((stack_is_OK(&s) != 0) || (ErNum != ERRORSIZE))
        {
            printf("~In %s, line %d, function: %s\n   ~Error in stack_is_OK(with error in size), error code = %d\n", __FILE__, __LINE__,__FUNCTION__, ErNum);
            DUMP (ErNum,&s)
            ErNum = 0;
            return 0;
        }
    ErNum = 0;
    //
    s.StackSize = NAN;
    if ((stack_is_OK(&s) != 0) || (ErNum != ERRORSIZE))
        {
            printf("~In %s, line %d, function: %s\n   ~Error in stack_is_OK(with error in size), error code = %d\n, size = NAN", __FILE__, __LINE__,__FUNCTION__, ErNum);
            DUMP (ErNum,&s)
            ErNum = 0;
            return 0;
        }
    ErNum = 0;

    stack_Dtor(&s);

    return 1;
}

//************************************
/// Tests stack_is_OK in case of StackCapacity
///
/// Parameters: [in] struct MyStack* s - pointer to MyStack structure\n
///
/// Output: 1 if test was passed, 0 if not and calls for DUMP
///
//************************************

int test_error_capacity(MyStack* stak)
{
    assert(stak);
    MyStack s = *stak;
    stack_Ctor(&s);

    for(double i = 2.01; i < 44.01; i = i + 1.0)
    {
        stack_push((&s), 5.023 + i);
    }

    s.StackCapacity = 10;
    if ((stack_is_OK(&s) != 0) || (ErNum != ERRORCAP))
        {
            printf("~In %s, line %d, function: %s\n   ~Error in stack_is_OK(with error in capacity), error code = %d\n", __FILE__, __LINE__,__FUNCTION__, ErNum);
            DUMP (ErNum,&s)
            ErNum = 0;
            return 0;
        }
    ErNum = 0;
    //
    s.StackCapacity = 77;
    if ((stack_is_OK(&s) != 0) || (ErNum != ERRORCAP))
        {
            printf("~In %s, line %d, function: %s\n   ~Error in stack_is_OK(with error in capacity), error code = %d\n", __FILE__, __LINE__,__FUNCTION__, ErNum);
            DUMP (ErNum,&s)
            ErNum = 0;
            return 0;
        }
    ErNum = 0;
    //
    s.StackCapacity = NAN;
    if ((stack_is_OK(&s) != 0) || (ErNum != ERRORCAP))
        {
            printf("~In %s, line %d, function: %s\n   ~Error in stack_is_OK(with error in capacity), error code = %d\n, capacity = NAN", __FILE__, __LINE__,__FUNCTION__, ErNum);
            DUMP (ErNum,&s)
            ErNum = 0;
            return 0;
        }
    ErNum = 0;

    stack_Dtor(&s);

    return 1;
}

//************************************
/// Tests stack_is_OK in case of data
///
/// Parameters: [in] struct MyStack* s - pointer to MyStack structure\n
///
/// Output: 1 if test was passed, 0 if not and calls for DUMP
///
//************************************

int test_error_data(MyStack* stak)
{
    assert(stak);
    MyStack s = *stak;
    stack_Ctor(&s);


    for(double i = 2.01; i < 44.01; i = i + 1.0)
    {
        stack_push((&s), 5.023 + i);
    }

    (&s)->data[41] = 90;
    if ((stack_is_OK(&s) != 0) || (ErNum != ERRORDATA))
        {
            printf("~In %s, line %d, function: %s\n   ~Error in stack_is_OK(with error in data), error code = %d\n", __FILE__, __LINE__,__FUNCTION__, ErNum);
            DUMP (ErNum,&s)
            ErNum = 0;
            return 0;
        }
    ErNum = 0;

    stack_Dtor(&s);

    return 1;
}

//************************************
/// Tests stack
///
/// Parameters: [in] struct MyStack* s - pointer to MyStack structure\n
///
/// Output: 1 if test was passed, 0 if not and calls for DUMP
///
//************************************

int test_stack(MyStack* stak)
{
    assert(stak);

    //
    if ( !test_error_data(stak)) return 0;
    //
    if ( !test_error_capacity(stak)) return 0;
    //
    if ( !test_error_size(stak)) return 0;
    //
    //
    //Test error popping
    for(double i = 2.01; i < 44.01; i = i + 1.0)
    {
        stack_push((stak), 5.023 + i);
    }

    for(double i = 43.01; i > 1.01; i = i - 1.0)
        {
            if ( !iszero((stack_pop(stak))-(5.023 + i)))
                {
                    printf("~In %s, line %d, function: %s\n   ~Error in pushing and popping in %.0f element tested: stack_pop() != %f\n", __FILE__, __LINE__,__FUNCTION__, (i - 2.01 + 1),5.023 + i);
                    DUMP (ErNum,stak)
                    ErNum = 0;
                    return 0;
                }
        }
    return 1;
}
