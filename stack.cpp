#include "D:\TX\TXlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define GUARD 32234
#define GUARDPOIS -1
#define SIZEPOIS -1
#define HASHPOIS -1
#define HASHDEFAULT 0
#define ERROROK 0
#define ERRORDATA 1
#define ERRORCAPACITY 2
#define ERRORSIZE 3
#define ERRORPOP 4
#define ERRORMEMORY 5
#define ERRORSTRUCT 6


#ifndef $
#define $(x) std::cout<<"~"<<#x " = "<<x<<"\n";
#endif

#define DUMP(ERRNUMBER,stack) { printf("~In File: %s\n~In Line: %d\n~In Function: %s\n", __FILE__, __LINE__, __FUNCTION__); \
                                printf("~stack " #stack " [0x%X]\n~{\n~   Capacity = %u\n~   Size = %u\n~   data [0x%X]:\n~",  (stack), (stack)->StackCapacity,\
                                (stack)->StackSize, (stack)->data);\
                                int bufn = (stack)->StackSize;\
                                for (int i = 0; i < bufn; i++)\
                                { \
                                    printf("      data[%d] = %f\n~", i, (stack)->data[i]);\
                                } \
                                printf("   In memory StackHash = %i\n~",          (stack)->StackHash_stack);\
                                int StackHashBuf = (stack)->StackHash_stack;\
                                (stack)->StackHash_stack = HASHDEFAULT;\
                                printf("       Real stack_hash = %i\n~",         make_Stack_hash(stack));\
                                (stack)->StackHash_stack = StackHashBuf;\
                                printf("   In memory DataHash  = %i\n~",           (stack)->StackHash_data);\
                                printf("       Real data_hash  = %i\n~",          make_data_hash(stack));\
                                printf("   Struct_guard_begin  = %s\n~", (((stack)->stack_guard_begin) == GUARD) ? "GUARD": "ERROR");\
                                printf("   Struct_guard_end    = %s\n~",   (((stack)->stack_guard_end) == GUARD) ? "GUARD": "ERROR");\
                                printf("   Data_guard_begin    = %s\n~", iszero(*((stack)->data_guard_begin) - GUARD) ? "GUARD": "ERROR");\
                                printf("   Data_guard_end      = %s\n~",   iszero(*((stack)->data_guard_end) - GUARD) ? "GUARD": "ERROR");\
                                printf("   INFO: \n~");\
                                switch (ErNum)\
                                { \
                                    case ERROROK:\
                                        printf("      Stack is OK\n~");\
                                        break;\
                                    case ERRORDATA:\
                                        printf("      Data or hash was damaged\n~");\
                                        break;\
                                    case ERRORCAPACITY:\
                                        printf("      Capacity is not suitable\n~");\
                                        break;\
                                    case ERRORSIZE:\
                                        printf("      Size is not suitable\n~");\
                                        break;\
                                    case ERRORPOP:\
                                        printf("      Can't pop from the stack\n~");\
                                        break;\
                                    case ERRORMEMORY:\
                                        printf("      Data memory was damaged\n~");\
                                        break;\
                                    case ERRORSTRUCT:\
                                        printf("      Struct elements of stack was damaged\n~");\
                                        break;\
                                    default:\
                                        printf("      Stack is OK\n~");\
                                }\
                                printf("}\n\n");\
                              }

#define UTEST(result,wanted) {\
                                    if ( !iszero(result - wanted))\
                                    {\
                                        printf("~Error in testing: in File: %s\n~In Line: %d\n~In Function: %s\n", __FILE__, __LINE__, __FUNCTION__);\
                                        printf("~Test result " #result " = %f is not correct ( != %f)", result, wanted);\
                                        ErNum = ERRORPOP;\
                                        DUMP (ErNum,stak)\
                                        ErNum =0;\
                                        return 0;\
                                    }\
                            }
#define UNERTEST(ErrorName, stak){\
                                if ((stack_is_OK(stak) != 0) || (ErNum != ErrorName))\
                                {\
                                    printf("~In %s, line %d, function: %s\n   ~Error in stack_is_OK(with error code " #ErrorName " = %d), error code = %d\n", __FILE__,  __LINE__, __FUNCTION__, ErrorName, ErNum);\
                                    DUMP (ErNum,(stak))\
                                    ErNum = 0;\
                                    return 0;\
                                }\
                                \
                            }

typedef double stack_type;
typedef struct MyStack MyStack;

const unsigned int StartCapacity = 10;
int ErNum = ERROROK;

struct MyStack
{
    int stack_guard_begin = GUARDPOIS;

    stack_type* data = NULL;

    unsigned int StackSize = SIZEPOIS;
    unsigned int StackCapacity = SIZEPOIS;

    int StackHash_data = HASHDEFAULT;
    int StackHash_stack = HASHDEFAULT;

    stack_type* data_guard_begin = NULL;
    stack_type* data_guard_end = NULL;

    int stack_guard_end = GUARDPOIS;
};

int iszero(const double x);
void stack_Ctor(MyStack*);
void set_guards(MyStack*);
void stack_Dtor(MyStack*);
void stack_Clear(MyStack*);
void stack_push(MyStack*, stack_type);
stack_type stack_pop(MyStack*);
void stack_extend (MyStack*);
void stack_contract (MyStack*);
void make_hash (MyStack*);
int make_Stack_hash (const MyStack*);
int make_data_hash (const MyStack*);
int stack_is_OK(MyStack*);
int test_stack(MyStack*);
int test_error_size(const MyStack*);
int test_error_capacity(const MyStack*);
int test_error_data(const MyStack*);
int test_error_memory(const MyStack*);
int test_error_stackbuf(const MyStack*);

int main()
{
    txSetConsoleAttr(0xf0);
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

    s->stack_guard_begin = GUARD;
    s->stack_guard_end = GUARD;

    s->data = (stack_type*) calloc(StartCapacity + 2, sizeof(stack_type));
    s->StackSize = 0;
    s->StackCapacity = StartCapacity;

    set_guards(s);

    for (unsigned int i = 0; i < (s->StackCapacity); i++)
    {
        s->data[i] = NAN;
    }

    make_hash(s);
}

//************************************
/// Sets guards of the stack's array
///
/// Parameters: [in] struct MyStack* s - pointer to MyStack structure
///
///
/// Output: void
///
//************************************

void set_guards(MyStack* s)
{
    assert(s);

    s->data_guard_begin = s->data;
    *(s->data_guard_begin) = GUARD;

    s->data = s->data + 1;
    s->data_guard_end = s->data + s->StackCapacity;
    *(s->data_guard_end) = GUARD;
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

    s->StackSize = SIZEPOIS;
    s->StackCapacity = SIZEPOIS;

    s->StackHash_data = HASHPOIS;
    s->StackHash_stack = HASHPOIS;

    *(s->data_guard_begin) = NAN;
    *(s->data_guard_end) = NAN;
    s->data_guard_begin = NULL;
    s->data_guard_end = NULL;

    s->stack_guard_begin = GUARDPOIS;
    s->stack_guard_end = GUARDPOIS;

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

    s->data = (stack_type*) realloc(s->data, (StartCapacity + 2) * sizeof(stack_type));
    s->StackCapacity = StartCapacity;

    set_guards(s);

    make_hash(s);
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
    if ( !stack_is_OK(s)) {DUMP(ErNum,s); abort();}

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

    make_hash(s);

    if ( !stack_is_OK(s)) {DUMP(ErNum,s); abort();}
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

    if ( !stack_is_OK(s)) {DUMP(ErNum,s); abort();}

    stack_type buf = s->data[--(s->StackSize)];
    s->data[(s->StackSize)] = NAN;

    if (((s->StackCapacity) >= 4 * (s->StackSize)) && (s->StackCapacity > 10))
    {
        stack_contract(s);
    }

    make_hash(s);

    if ( !stack_is_OK(s)) {DUMP(ErNum,s); abort();}
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

    s->data = (stack_type*) realloc(s->data_guard_begin, ((s->StackCapacity) * 2 + 2) * sizeof(stack_type));
    s->StackCapacity *= 2;

    set_guards(s);

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

    s->data = (stack_type*) realloc(s->data_guard_begin, (s->StackCapacity / 2 + 2) * sizeof(stack_type) );
    s->StackCapacity /= 2;

    set_guards(s);
}

//************************************
/// Hashes the  stack.
///
/// Parameters: [in] struct MyStack* s - pointer to MyStack structure
///
/// Output: changes StackHash_stack and StackHash_data in stack.
///
//************************************

void make_hash (MyStack* s)
{
    assert(s);

    s->StackHash_data = make_data_hash(s);
    s->StackHash_stack = HASHDEFAULT;
    s->StackHash_stack = make_Stack_hash(s);
}

//************************************
/// Hashes the structure of the stack.
///
/// Parameters: [in] struct MyStack* s - pointer to MyStack structure
///
/// Output: changes StackHash_stack in stack.
///
//************************************

int make_Stack_hash (const MyStack* s)
{
    assert(s);

    int hashSum = 1;
    for (char* p = (char*) s; p < (char*) s + sizeof(MyStack); p++)
    {
        hashSum = 22220 * hashSum  + abs(int(*p));
        hashSum = hashSum % 20002;
    }

    return hashSum;
}

//************************************
/// Hashes the array in the stack.
///
/// Parameters: [in] struct MyStack* s - pointer to MyStack structure
///
/// Output: changes StackHash_data in stack.
///
//************************************

int make_data_hash (const MyStack* s)
{
    assert(s);

    int hashSum = 0;
    for (char* p = (char*) s->data; p < (char*) (s->data + s->StackCapacity); p++)
    {
        hashSum = 22222 * hashSum  + abs(int(*p));
        hashSum = hashSum % 20002;
    }

    return hashSum;
}


//************************************
/// Checks if stack is fine or not, and writes error code in global variable
///
/// Parameters: [in] struct MyStack* s - pointer to MyStack structure\n
///
/// Output: 1 if stack is fine\n 0 if not,\n changes the global variable ErNum (0 - all is right\n, 1 - data in stack is damaged\n 2 - not suitable stack capacity,\n
/// 3 - not suitable stack size,\n 5 - memory of stack data was damaged,\n 6 - memory of stack struct was damaged)
///
//************************************

int stack_is_OK(MyStack* s)
{
    assert(s);
    int not_error = 1;

    if ( !(s->data)){ ErNum = ERRORDATA; return 0;}

    for (unsigned int i = s->StackSize + 1; i < s->StackCapacity; i++ )
    {
        if ( isfinite(s->data[i]))
        {
            ErNum = ERRORMEMORY;
            not_error = 0;
            break;
        }
    }

    if ( !iszero(make_data_hash(s) - (s->StackHash_data))){                                                                                                             ErNum = ERRORDATA;      not_error = 0; }

    if ( !iszero(*(s->data_guard_begin) - GUARD) || !iszero(*(s->data_guard_end) - GUARD)){                                                                             ErNum = ERRORMEMORY;    not_error = 0;}

    int StackHashBuf = s->StackHash_stack;
    s->StackHash_stack = HASHDEFAULT;
    if ( !iszero(make_Stack_hash(s) - StackHashBuf) || ((s->stack_guard_begin) != GUARD) || ((s->stack_guard_end != GUARD))){                                           ErNum = ERRORSTRUCT;    not_error = 0;}
    s->StackHash_stack = StackHashBuf;

    if ( !isfinite(s->StackCapacity) || ((s->StackCapacity) < StartCapacity) || (((s->StackCapacity) % StartCapacity) != 0) || ((s->StackSize) > (s->StackCapacity))){  ErNum = ERRORCAPACITY;  not_error = 0;}

    if ( !isfinite(s->StackSize) || (((s->StackSize) > (s->StackCapacity)) && !isfinite(s->data[s->StackSize-1])) || isfinite(s->data[s->StackSize])) {                 ErNum = ERRORSIZE;      not_error = 0;}

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

int test_error_size(const MyStack* stak)
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
        UNERTEST(ERRORSIZE, &s)
    ErNum = 0;
    //
    s.StackSize = 150;
        UNERTEST(ERRORSIZE, &s)
    ErNum = 0;
    //
    s.StackSize = NAN;
        UNERTEST(ERRORSIZE, &s)
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

int test_error_capacity(const MyStack* stak)
{
    assert(stak);
    MyStack s = *stak;
    stack_Ctor(&s);

    for(double i = 2.01; i < 44.01; i = i + 1.0)
    {
        stack_push((&s), 5.023 + i);
    }

    s.StackCapacity = 10;
        UNERTEST(ERRORCAPACITY, &s)
    ErNum = 0;
    //
    s.StackCapacity = 77;
        UNERTEST(ERRORCAPACITY, &s)
    ErNum = 0;
    //
    s.StackCapacity = NAN;
        UNERTEST(ERRORCAPACITY, &s)
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

int test_error_data(const MyStack* stak)
{
    assert(stak);
    MyStack s = *stak;
    stack_Ctor(&s);


    for(double i = 2.01; i < 44.01; i = i + 1.0)
    {
        stack_push((&s), 5.023 + i);
    }

    stack_type buf = s.data[41];
    s.data[41] = 90;
        UNERTEST(ERRORDATA, &s)
    ErNum = 0;
    s.data[41] = buf;

    buf = s.data[1];
    s.data[1] = 92020;
        UNERTEST(ERRORDATA, &s)
    ErNum = 0;
    s.data[1] = buf;

    buf = s.data[20];
    s.data[20] = 12340;
        UNERTEST(ERRORDATA, &s)
    ErNum = 0;
    s.data[20] = buf;

    void* buf_point = s.data;
    s.data = NULL;
        UNERTEST(ERRORDATA, &s)
    ErNum = 0;
    s.data = (stack_type*) buf_point;

    stack_Dtor(&s);

    return 1;
}

//************************************
/// Tests stack_is_OK in case of data memory
///
/// Parameters: [in] struct MyStack* s - pointer to MyStack structure\n
///
/// Output: 1 if test was passed, 0 if not and calls for DUMP
///
//************************************

int test_error_memory(const MyStack* stak)
{
    assert(stak);
    MyStack s = *stak;
    stack_Ctor(&s);


    for(double i = 2.01; i < 44.01; i = i + 1.0)
    {
        stack_push((&s), 5.023 + i);
    }

    *(s.data_guard_begin) = 500;
        UNERTEST(ERRORMEMORY, &s)
    ErNum = 0;
    *(s.data_guard_begin) = GUARD;

    *(s.data_guard_end) = -2200;
        UNERTEST(ERRORMEMORY, &s)
    ErNum = 0;
    *(s.data_guard_end) = GUARD;

    stack_Dtor(&s);

    return 1;
}

//************************************
/// Tests stack_is_OK in case of struct memory
///
/// Parameters: [in] struct MyStack* s - pointer to MyStack structure\n
///
/// Output: 1 if test was passed, 0 if not and calls for DUMP
///
//************************************

int test_error_stackbuf(const MyStack* stak)
{
    assert(stak);
    MyStack s = *stak;
    stack_Ctor(&s);

    for(double i = 2.01; i < 44.01; i = i + 1.0)
    {
        stack_push((&s), 5.023 + i);
    }

    void* buf_point = s.data;
    s.data += 3;
        UNERTEST(ERRORSTRUCT, &s)
    ErNum = 0;
    s.data = (stack_type*) buf_point;

    buf_point = s.data_guard_begin;
    s.data_guard_begin += 5;
        UNERTEST(ERRORSTRUCT, &s)
    ErNum = 0;
    s.data_guard_begin = (stack_type*) buf_point;

    buf_point = s.data_guard_end;
    s.data_guard_end -= 50;
        UNERTEST(ERRORSTRUCT, &s)
    ErNum = 0;
    s.data_guard_end = (stack_type*) buf_point;

    s.stack_guard_begin = 432200;
        UNERTEST(ERRORSTRUCT, &s)
    ErNum = 0;
    s.stack_guard_begin = GUARD;

    s.stack_guard_end = -4352328;
        UNERTEST(ERRORSTRUCT, &s)
    ErNum = 0;
    s.stack_guard_end = GUARD;

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
    if ( !test_error_data(stak))        return 0;
    //
    if ( !test_error_capacity(stak))    return 0;
    //
    if ( !test_error_size(stak))        return 0;
    //
    if ( !test_error_memory(stak))      return 0;
    //
    if ( !test_error_stackbuf(stak))    return 0;
    //
    //
    //Test error popping
    int counter = 0;
    for(double i = 2.01; i < 1225; i = i + 1.111, counter++)
    {
        printf("Testing stack: %.*s %.*s\r", counter * 55 / 2200, "||||||||||||||||||||||||||||||||||||||||||||||||||||||||", (2200 - counter) * 55 / 2200, "--------------------------------------------------------");
        stack_push((stak), 5.023 + i);
    }

    for(double i = 1224.11; i >= 2.01; i = i - 1.111, counter++)
    {
        printf("Testing stack: %.*s %.*s\r", counter * 55 / 2200, "||||||||||||||||||||||||||||||||||||||||||||||||||||||||", (2200 - counter) * 55 / 2200, "--------------------------------------------------------");
        UTEST(stack_pop(stak),(5.023 + i))
    }
    printf("\n");
    return 1;
}
