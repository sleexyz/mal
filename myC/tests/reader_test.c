#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../reader.h"


typedef struct TestCase TestCase;
struct TestCase {
  char* input;
  char** strs;
  Token** tokens;
  int length;
};

static TestCase case0 = (TestCase){
  .input = ";hahahaha h a h a h ah",
  .strs = (char*[]){},
  .tokens = (Token*[]) {
  },
  .length = 0
};
static TestCase case1 = (TestCase){
  .input = "(      );b a d a s df +; a ; ",
  .strs = (char*[]){"(", ")"},
  .tokens = (Token*[]) {
    &(Token){.tt = TLP},
    &(Token){.tt = TRP}
  },
  .length = 2
};

static TestCase case2 = (TestCase){
  .input = "(+ (1) (123 \"Hello!\" ))",
  .strs= (char*[]){"(", "+", "(", "1", ")", "(", "123", "\"Hello!\"", ")", ")"},
  .tokens = (Token*[]) {
    &(Token){.tt = TLP},
    &(Token){.tt = TSYM, .val=&(TokVal){.str="+"}},
    &(Token){.tt = TLP},
    &(Token){.tt = TINT, .val=&(TokVal){.i=1}},
    &(Token){.tt = TRP},
    &(Token){.tt = TLP},
    &(Token){.tt = TINT, .val=&(TokVal){.i=123}},
    &(Token){.tt = TSTR, .val=&(TokVal){.str="Hello!"}},
    &(Token){.tt = TRP},
    &(Token){.tt = TRP}
  },
  .length = 10
};

static TestCase* cases[] = {&case0, &case1, &case2};


typedef void (*TestFunc) (TestCase*);

void test(TestFunc test_func) {
  for (int i = 0; i < sizeof(cases)/sizeof(TestCase*); i++) {
    test_func(cases[i]);
  }
}



// Tokenize

void tc_print_strs(TestCase* tc) {
  if (tc->length == 0) {
    printf("...");
    return;
  }

  int i;
  for(i = 0; i < tc->length -1; i++) {
    printf("\"%s\" ", tc->strs[i]);
  }
  printf("\"%s\"", tc->strs[i]);
}

void test_tokenize(TestCase* tc) {
  printf("** Input:            %s\n", tc->input);
  printf("** Expected Strings: ");
  tc_print_strs(tc);
  printf("\n");

  LList* tokens = tokenize(tc->input);
  assert(tokens->length == tc->length);
  printf("** Got:              ");
  ll_print(tokens);


  LNode* curr = tokens->head;
  for (int i = 0; i < tc->length; i++, curr=curr->next) {
    /* assert(tok_equals(tc->tokens[i], curr->val->tok)); */
    tok_assert_equals(tc->tokens[i], curr->val->tok);
  }
  //Exit invariant
  assert(curr == NULL);


  ll_free_recur(tokens);

  printf("\n");
}



// Reader

void test_reader(TestCase* tc) {
  printf("** Input: %s\n", tc->input);
  Reader* r = r_create(tc->input);
  Token* curr;
  Token* next = r_peek(r);

  printf("** Expecting: ");
  tc_print_strs(tc);
  printf("\n");

  printf("** Got:       ");

  int count = 0;

  while (r_check(r), curr = r_next(r), curr != NULL) {
    r_check(r);
    assert(next == curr); // This tests peeking
    next = r_peek(r);


    assert(tok_equals(tc->tokens[count], curr));
    printf("\"");
    tok_print(curr);
    printf("\" ");

    count += 1;
    r_check(r);
  }
  if (count > 0) {
    printf("\b");
  }else {
    printf("...");
  }
  printf("\n");
  assert(count == tc->length);


  r_free(r);
  printf("\n");
}

void test_parser (TestCase* tc) {
  printf("**   Input: %s\n", tc->input);

  Reader* r = r_create(tc->input);

  LNode* output = parse(r);

  if (output == NULL) {
    printf("**  Output: ... \n");
  }else {
    printf("**  Output: ");
    ln_print(output);
    printf("\n");

    ln_free_recur(output);
  }
  r_free(r);
}


// Main

int main(int argc, char* argv[]) {
  printf("\n* Testing tokenize:\n");
  test(test_tokenize);

  printf("\n* Testing reader:\n");
  test(test_reader);

  printf("\n* Testing parser:\n");
  test(test_parser);

  printf("\n* Success!\n");
  return 0;
}
