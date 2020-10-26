#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>


typedef int Elem_t;

const int SIZE_OF_STACK = 10;

const int POISON = -666;

const int NAME_LEN = 100;

const int64_t canary = 999999999999999;


#define INFORMFDUMP __FILE__, __PRETTY_FUNCTION__, __LINE__

#define STACK(x) Stack_t x;               \
                 StackConstructor(&x, #x)

#define DUMP(x, reason) Dump ((x), (x)->name, __LINE__, __PRETTY_FUNCTION__, (reason))

#define DEBUG

#ifdef DEBUG
    #define ON_DEBUG(code) code
#else
    #define ON_DEBUG
#endif

#define STACK_OK(x) assert (stk_ok (x));


struct Stack_t
{
    uint64_t canary_begin;

    int size_of_fulled_stack;
    char name[NAME_LEN];
    Elem_t data[SIZE_OF_STACK];
    char errno;

    uint64_t canary_end;
};


void Dump (const Stack_t* stk_adr, char* name, const int line, const char* name_of_func, const char* reason);
void StackConstructor (Stack_t* stk_adress, const char* name);
void StackPush (Stack_t* stk_adr, Elem_t t);
Elem_t  StackPop (Stack_t* stk_adr);
void StackDestruct (Stack_t* stk_adr);
bool stk_ok (Stack_t* stk_adr);


void Unittest ()
{
    STACK(teststk);

    STACK_OK(&teststk);

    DUMP(&teststk, "Just for looking");

    for (int i = 0; i < SIZE_OF_STACK; ++i)
        StackPush(&teststk, i);

    DUMP(&teststk, "Just for looking");

    for (int i = 0; i < SIZE_OF_STACK; ++i)
    {
        Elem_t t = StackPop (&teststk);
        printf ("elem from stack: %d\n", t);
    }

    DUMP(&teststk, "Just for looking");

    *(int*)(&teststk - 1) = 5;
}


int main ()
{
    STACK(stk1);

    Elem_t t = 0;

    StackPush (&stk1, t);

    Elem_t x = StackPop (&stk1);

    StackDestruct (&stk1);

    Unittest ();
}

void Dump (const Stack_t* stk_adr, char* name, int line, const char* name_of_func, const char* reason)
{
    printf ("%s from %s (%d)", reason, name_of_func, line);

    char con[3] = {};

    if (strcmp (reason, "Just for looking") == 0) //Just for looking
    sprintf (con, "ok");

    if (nullptr == stk_adr)
        printf ("\tBroken stack address :(\n");

    else
    {
        printf ("Stack_t %s %#x (%s)" "\n{\n", name, stk_adr, con);
        printf ("\t" "size = %d" "\n", stk_adr->size_of_fulled_stack);
        printf ("\t" "data[%d] = %#x" "\n\t{\n", SIZE_OF_STACK, &stk_adr->data[0]);
        for (int i = 0; i < SIZE_OF_STACK; ++i)
        {
            if (stk_adr->data[i] != POISON)
                printf ("\t\t*[%d] = %d\n\n", i, stk_adr->data[i]);
            else
                printf ("\t\t [%d] = %d [POISON]\n\n", i, stk_adr->data[i]);
        }
        printf ("\t}\n}\n\n");
    }
}

void StackConstructor (Stack_t* stk_adr, const char* name)
{
    assert (nullptr != stk_adr);

    strncpy (stk_adr->name, name, NAME_LEN);

    for (int i = 0; i < SIZE_OF_STACK; ++i)
        stk_adr->data[i] = POISON;

    stk_adr->size_of_fulled_stack = 0;

    stk_adr->canary_begin = canary;
    stk_adr->canary_end = canary;

    STACK_OK(stk_adr);
}

void StackPush (Stack_t* stk_adr, Elem_t t)
{
    STACK_OK(stk_adr);

    assert (stk_adr->size_of_fulled_stack < SIZE_OF_STACK);

    stk_adr->data[stk_adr->size_of_fulled_stack] = t;

    ++stk_adr->size_of_fulled_stack;

    STACK_OK(stk_adr);
}

Elem_t  StackPop (Stack_t* stk_adr)
{
    STACK_OK(stk_adr);

    assert (stk_adr->size_of_fulled_stack > 0);

    --stk_adr->size_of_fulled_stack;

    Elem_t t = stk_adr->data[stk_adr->size_of_fulled_stack];

    stk_adr->data[stk_adr->size_of_fulled_stack] = POISON;

    STACK_OK(stk_adr);

    return t;
}

void StackDestruct (Stack_t* stk_adr)
{
        STACK_OK(stk_adr);

        for (int i = 0; i < SIZE_OF_STACK; ++i)
            stk_adr->data[i] = POISON;
        stk_adr->size_of_fulled_stack = 0;

        STACK_OK(stk_adr);
}

bool stk_ok (Stack_t* stk_adr)
{
    assert (nullptr != stk_adr);

    if ((stk_adr->canary_begin != canary) or (stk_adr->canary_end != canary))
        return false;

    if (stk_adr->size_of_fulled_stack < 0)
        return false;

    return true;
}
