#include <stdbool.h>
#include <stdio.h>

extern void do_test_trch(void);

void do_tests(void)
{
        do_test_trch();
}

void expect(char *name, bool expect, bool val)
{
        printf("%s %s", name, expect == val ? "ok" : "ng");
}
