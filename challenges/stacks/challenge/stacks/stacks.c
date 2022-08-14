/*
 * stacks | RPN Calculator Service
 *
 * Copyright (c) 2011 richthofen
 * Copyright (c) 2014 Alexander Taylor <ajtaylor@fuzyll.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "ctf.h"

#define STDIN STDIN_FILENO
#define STDOUT STDOUT_FILENO
#define BUFSIZE 16
#define MAXSTACKS 8

struct rpn_stack
{
    int *top;
    int data[40];
};

static int hash;
static int current_stack = 0;
static int stack_count = 0;
static struct rpn_stack *stack[MAXSTACKS];
static char buf[BUFSIZE*4];


/*
 * Check if stack is empty.
 */
bool stack_empty()
{
    return (stack[current_stack]->top == stack[current_stack]->data);
}


/*
 * Display top of the stack.
 */
void display_top()
{
    if (stack_count > 0) {
        if (!stack_empty()) {
            snprintf(buf, sizeof(buf), "%d\n", *(stack[current_stack]->top-1));
            ctf_writes(STDOUT, buf);
        } else {
            ctf_writes(STDOUT, "Current stack is empty.\n");
        }
    } else {
        ctf_writes(STDOUT, "There are no active stacks.\n");
    }
}


/*
 * Display entire stack.
 */
void display_all()
{
    int *stack_ptr;
    unsigned int i = 0;

    if (stack_count > 0) {
        if (!stack_empty()) {
            stack_ptr = stack[current_stack]->data;
            while (stack_ptr < stack[current_stack]->top) {
                if (i > sizeof(buf)-12) {
                    i = 0;
                    ctf_writes(STDOUT, buf);
                }
                snprintf(buf+i, sizeof(buf)-i, "|%-11d\n", *stack_ptr);
                stack_ptr++;
                i += 12;
            }
            ctf_writes(STDOUT, buf);
            ctf_writes(STDOUT, "\n");
        } else {
            ctf_writes(STDOUT, "Current stack is empty.\n");
        }
    } else {
        ctf_writes(STDOUT, "There are no active stacks.\n");
    }
}


/*
 * Create stack.
 */
void create_stack()
{
    struct rpn_stack *nstack;
    int i = 0;

    if (stack_count < MAXSTACKS) {
        nstack = (struct rpn_stack *)malloc(sizeof(struct rpn_stack));
        if (nstack == NULL) {
            ctf_writes(STDOUT, "Failed to create stack!\n");
            return;
        }
        while (stack[i] != NULL && i < MAXSTACKS) i++;

        if (i < MAXSTACKS) {
            stack[i] = nstack;
        } else {
            ctf_writes(STDOUT, "Active stack limit reached!\n");
            free(nstack);
            return;
        }

        stack[i]->top = stack[i]->data;

        ctf_writes(STDOUT, "Switching to new stack.\n");
        current_stack = i;
        stack_count++;
    } else {
        ctf_writes(STDOUT, "Active stack limit reached!\n");
    }
}


/*
 * Destroy stack.
 */
void destroy_stack()
{
    int i;

    if (stack_count > 0) {
        if (current_stack < stack_count && stack[current_stack] != NULL) {
            snprintf(buf, sizeof(buf), "Removing stack #%d.\n", current_stack);
            ctf_writes(STDOUT, buf);

            i = current_stack;
            free(stack[i]);
            while (i+1 < MAXSTACKS && stack[i+1] != NULL) {
                stack[i] = stack[i+1];
                i++;
            }
            stack[i] = NULL;
            stack_count--;

            if (i == current_stack && stack_count > 0) {
                current_stack--;
            }
        } else {
            ctf_writes(STDOUT, "Currently selected stack does not exist!\n"
                        "Resetting current stack to the first one.\n");
            current_stack = 0;
        }
    } else {
        ctf_writes(STDOUT, "There are no active stacks.\n");
    }
}


/*
 * Switch stack.
 */
void switch_stack()
{
    if (stack_count > 0) {
        current_stack = (current_stack+1) % stack_count;
        snprintf(buf, sizeof(buf), "Switched to stack #%d.\n", current_stack);
        ctf_writes(STDOUT, buf);
    } else {
        ctf_writes(STDOUT, "There are no active stacks.\n");
    }
}


/*
 * Usage prompt.
 */
void usage()
{
    ctf_writes(STDOUT, "Available commands:\n"
                 "top  => Display top value of the currently selected stack.\n"
                 "new  => Create a new stack and switch to it.\n"
                 "del  => Delete currently selected stack.\n"
                 "disp => Display all values on stack.\n"
                 "next => Switch between active stacks.\n"
                 "quit => Exit the calculator.\n");
}


/*
 * Command handler.
 */
int handle_command(char *command)
{
    if (!strcmp(command, "top")) {
        display_top();
    } else if (!strcmp(command, "new")) {
        create_stack();
    } else if (!strcmp(command, "del")) {
        destroy_stack();
    } else if (!strcmp(command, "disp")) {
        display_all();
    } else if (!strcmp(command, "next")) {
        switch_stack();
    } else if (!strcmp(command, "quit")) {
        return 1;
    } else if (!strcmp(command, "help")) {
        usage();
    } else {
        ctf_writes(STDOUT, "That command is not yet implemented.\n");
        return -1;
    }
    return 0;
}


/*
 * Check if operation should be allowed.
 */
bool operation_allowed()
{
    int *top = stack[current_stack]->top;
    int *base = stack[current_stack]->data;
    if (top == base || top-1 == base) {
        return false;
    }
    return true;
}


/*
 * Equation handler.
 */
int handle_equation(char *eq)
{
    int op1;
    int op2;
    int result = 0;

    if (stack_count > 0) {
        for (; *eq != '\0'; eq++) {
            if (*eq >= '0' && *eq <= '9') {
                *(stack[current_stack]->top) = (int)(*eq-'0');
                stack[current_stack]->top++;
            } else if (operation_allowed()) {
                op1 = *(stack[current_stack]->top-2);
                op2 = *(stack[current_stack]->top-1);

                if (*eq == '+') {
                    result = op1 + op2;
                } else if (*eq == '-') {
                    result = op1 - op2;
                } else if (*eq == '*') {
                    result = op1 * op2;
                } else if (*eq == '/') {
                    result = op1 / op2;
                } else {
                    continue;
                }

                stack[current_stack]->top--;
                *(stack[current_stack]->top-1) = result;
            }
        }
    } else {
        ctf_writes(STDOUT, "There are no active stacks.\n");
    }

    return 0;
}


/*
 * User greeting.
 */
void greeting()
{
    ctf_writes(STDOUT, "Welcome to Stacks!\n"
                 "All commands are prefixed with ':'.\n"
                 "Type :help for a list of commands.\n");
}


/*
 * Main function.
 */
int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    char input[BUFSIZE];
    int len;

    // place stdout into non-buffered mode
    setvbuf(stdout, NULL, _IONBF, 0);

    // greet the user
    greeting();
    hash = rand();

    // RPN calculator loop
    while (true) {
        ctf_writes(STDOUT, "==> ");
        len = ctf_readsn(STDIN, input, BUFSIZE-1);

        // process command
        if (input[0] == ':') {
            input[len] = '\0';
            if (handle_command(input+1) == 1) {
                break;
            }
        // process equation
        } else if (len > 1) {
            if (handle_equation(input) == 5) {
                break;
            }
        // process carelessness
        } else {
            break;
        }
    }

    // bid farewell to the user
    ctf_writes(STDOUT, "Farewell!\n");

    return 0;
}
