#include <stdio.h>
#include <string.h>   /* for all the new-fangled string functions */
#include <stdlib.h>     /* malloc, free, rand */



int Fsize=50; /*maximum formula length*/
int inputs=11;/* number of formulas expected in input.txt*/
int ThSize=100;/* maximum size of set of formulas, if needed*/
int TabSize=500; /*maximum length of tableau queue, if needed*/



/* A set will contain a list of words. Use NULL for emptyset.  */
  struct set{
    char *item;/*first word of non-empty set*/
    struct set *tail;/*remaining words in the set*/
  };

/* A tableau will contain a list of pointers to sets (of words).  Use NULL for empty list.*/
struct tableau {
  struct set *S; /* pointer to first set in non-empty list */
  struct tableau *rest; /*list of pointers to other sets*/
};

/* A fuction to free all the allocated bits on the tableau */
void deepFree(struct tableau *t) {
  if(!t) return;
  while(t->S) {
    free(t->S->item);
    struct set *tempSet = t->S;
    t->S = t->S->tail;
    free(tempSet);
  }
  struct tableau *tempTableau = t;
  t = t->rest;
  free(tempTableau);
  deepFree(t);
}

// trying to see if my own method of freeing works
void freeSet(struct tableau *t) {

    struct set *head = t->S;
    struct set *temp;

    while (head != NULL) {
        temp = head;
        head = head->tail;
        free(temp->item);
        free(temp);
    }
}

void myFree(struct tableau *t) {

    struct tableau *temp;

    while (t != NULL) {
        temp = t;
        t = t->rest;
        freeSet(temp);
        free(temp);
    }
}

/*You need to change this next bit and include functions for parse, closed and complete.*/
int parse(char *g) {
    
    int len = strlen(g);
    int negation = 0; // 1 if negation 0 if not
    int formula = 0; // 1 if parsed down to z.

    if (g[0] == '-') { negation = 1; }

    if (len == 0) { return 0; }

    if (len == 1) {
        if (*g == 'p' || *g == 'q' || *g == 'r') {
            return 1;
        }
        else {
            return 0;
        }
    }

    char str[len + 1];
    strcpy(str, g);
    int len_str = strlen(str);

    int i = 0;  // index of old string.
    int j = 0;  // index of new updated string.
    int limit = len_str;  // points to the end of old string. 
    int reduced = 0; // if you i goes from 0 to limit without reducing then we cant reduce str anymore so break.
    
    while (1) {

        if ((str[i] == 'p') || (str[i] == 'q') || (str[i] == 'r')) { // Reduce prop to prop which can then be reduced to formulas. formula = z.
            str[j] = 'z';
            i++;
            j++;
            reduced = 1;
        }
        
        else if ((str[i] == '^') || (str[i] == '>') || (str[i] == 'v')) { // reduce all connectives to y.
            str[j] = 'y';
            i++;
            j++;
            reduced = 1;
        }
        
        else if ((str[i] =='(') && (str[i + 1] == 'z') && (str[i + 2] == 'y') && (str[i + 3] == 'z') && (str[i + 4] == ')')) {
            // youve seen ( z y z ) so reduce it to z. Corresponds to ( formula connective formula )
            str[j] = 'z';
            j++;
            i += 5;
            reduced = 1;
        }
        
        else if ((str[i] == '-') && (str[i + 1] == 'z')) {
            // make sure the next character is 'z'. if it is then reduce to z. corresponds to - formula
            str[j] = 'z';
            j++;
            i += 2;
            reduced = 1;
        }
        
        else {
            str[j] = str[i];    // character that cant be reduced eg (
            i++;
            j++;

            if (str[0] == 'z') {
                // reduced succesfully. if only formula remains.
                formula = 1;
                break;
            }

            if (i >= limit) {
                // reached end of string.
                if (reduced == 0) {
                    // went thru without reducing at all 
                    break;
                }
                
                limit = j;   // Set the limit of the new string. Now characters past this limit are ignored.
                j = 0;
                i = 0;
                reduced = 0;
            }
        }
    }

    if ((formula) && (negation)) {
        // parsed succesfully and first character is also -
        return 2;
    }

    else if (formula) {
        return 3;
    }

    return 0;
}

void partone(char *fmla, char *dest){
    
    int index_of_connective = -1;
    int bracket_counter = 0;
    int i;
    for (i = 0; i != strlen(fmla); ++i) {
        if (fmla[i] == '(') { bracket_counter++; }
        else if (fmla[i] == ')') { bracket_counter--; }
        else if (fmla[i] == '^' || fmla[i] == '>' || fmla[i] == 'v') {
            if (bracket_counter == 1) {
                index_of_connective = i;
                break;
            }
        }
    }

    dest[index_of_connective - 1] = '\0';
    int k;
    int l = 0;
    for (k = 1; k != index_of_connective; ++k) {
        dest[l++] = fmla[k];
    }
}

void parttwo(char *fmla, char *dest){
  
    int index_of_connective = -1;
    int bracket_counter = 0;
    
    int i;
    for (i = 0; i != strlen(fmla); ++i) {
        if (fmla[i] == '(') { bracket_counter++; }
        else if (fmla[i] == ')') { bracket_counter--; }
        else if (fmla[i] == '^' || fmla[i] == '>' || fmla[i] == 'v') {
            if (bracket_counter == 1) {
                index_of_connective = i;
                break;
            }
        }
    }
    // ((pvq)^r) index =6  len = 9. we want index 8
    int len = strlen(fmla) - index_of_connective - 1;
    dest[len - 1] = '\0';
    int m = 0;
    int k;

    for (k = index_of_connective + 1; k != strlen(fmla) - 1; ++k) {
        dest[m++] = fmla[k];
    }
}

/**
 * Used to get the top level connective.
 * 
 * @param item is a pointer to the first element in the string.
 * 
 * @returns ^ or > or v depending on which one is top level connective.
 */
char getConnective(char *item) {

    int bracket_counter = 0;
    int i;
    for (i = 0; i != strlen(item); ++i) {
        if (item[i] == '(') { bracket_counter++; }
        else if (item[i] == ')') { bracket_counter--; }
        else if (item[i] == '^' || item[i] == 'v' || item[i] == '>') {
            if (bracket_counter == 1) {
                return item[i];
            }
        }
    }

    return '\0';
}

int countSets(struct tableau *curr) {

    int i = 0;
    struct set *s = curr->S;
    while (s != NULL) {
        i++;
        s = s->tail;
    }

    return i;
}

int checkIfClosed(int seen[], int seenCounter) {

    int p = 0;
    int q = 0;
    int r = 0;
    int negP = 0;
    int negQ = 0;
    int negR = 0;
    int i;
    for (i = 0; i != seenCounter; ++i) {
        int curr = seen[i];
        printf("curr: %d\n", curr);
        if (curr == 0) {
            p = 1;
            if (negP == 1) { return 1; }
        }
        else if (curr == 1) {
            q = 1;
            if (negQ == 1) { return 1; }
        }
        else if (curr == 2) {
            r = 1;
            if (negR == 1) { return 1; }
        }
        else if (curr == 3) {
            negP = 1;
            if (p == 1) {  return 1; }
        }
        else if (curr == 4) {
            negQ = 1;
            if (q == 1) { return 1; }
        }
        else if (curr == 5) {
            negR = 1;
            if (r == 1) { return 1; }
        }
    }
    return 0;
}

int closedBranch(struct tableau *curr, int branchNo, int seen[], int seenCounter) {

    if (curr == NULL) {
        return checkIfClosed(seen, seenCounter);
    }

    int numBranches = countSets(curr);
    if (branchNo > numBranches) {
        struct set *set = curr->S;
        int i;
        for (i = 0; i != numBranches; ++i) {
            set = set->tail;
        }

        printf("BranchNo < num: %s\n\n", set->item);
        int type = parse(set->item);
        if (type) {
            if (set->item[0] == 'p') {
                seen[seenCounter++] = 0;
                int ret = closedBranch(curr->rest, branchNo, seen, seenCounter);

            }
            else if (set->item[0] == 'q') {
                seen[seenCounter++] = 1;
                return closedBranch(curr->rest, branchNo, seen, seenCounter);
            }
            else if (set->item[0] == 'r') {
                seen[seenCounter++] = 2;
                return closedBranch(curr->rest, branchNo, seen, seenCounter);
            }
        }

        if (type == 2) {
            if (set->item[1] == '-') {
                return closedBranch(curr->rest, branchNo, seen, seenCounter);
            }
            int negType = parse(set->item + 1);
            if (negType) {
                if (set->item[1] == 'p') {
                    seen[seenCounter++] = 3;
                    return closedBranch(curr->rest, branchNo, seen, seenCounter);
                }
                else if (set->item[1] == 'q') {
                    seen[seenCounter++] = 4;
                    return closedBranch(curr->rest, branchNo, seen, seenCounter);
                }
                else if (set->item[1] == 'r') {
                    seen[seenCounter++] = 5;
                    return closedBranch(curr->rest, branchNo, seen, seenCounter);
                }
            }

            char connective = getConnective(set->item);
            if (connective == '^') {
                if  (!closedBranch(curr->rest, branchNo, seen, seenCounter)) { return 0; }
                return closedBranch(curr->rest, branchNo + 1, seen, seenCounter);
            }
            else if (connective == 'v') {
                return closedBranch(curr->rest, branchNo, seen, seenCounter);
            }
            else {
                return closedBranch(curr->rest, branchNo, seen, seenCounter);
            }
        }

        char connective = getConnective(set->item);
        if (connective == '^') {
            return closedBranch(curr->rest, branchNo, seen, seenCounter);
        }
        else if (connective == 'v') {
            if (!closedBranch(curr->rest, branchNo, seen, seenCounter)) { return 0; }
            return closedBranch(curr->rest, branchNo + 1, seen, seenCounter);
        }
        else {
            if (!closedBranch(curr->rest, branchNo, seen, seenCounter)) { return 0; }
            return closedBranch(curr->rest, branchNo + 1, seen, seenCounter);
        }
    }
    
    else {
        struct set *set = curr->S;
        int i;
        for (i = 0; i != branchNo; ++i) {
            set = set->tail;
        }

        printf("Curr:  %s\n\n", set->item);
        int type = parse(set->item);
        if (type) {
            if (set->item[0] == 'p') {
                seen[seenCounter++] = 0;
                return closedBranch(curr->rest, branchNo, seen, seenCounter);
            }
            else if (set->item[0] == 'q') {
                seen[seenCounter++] = 1;
                return closedBranch(curr->rest, branchNo, seen, seenCounter);
            }
            else if (set->item[0] == 'r') {
                seen[seenCounter++] = 2;
                return closedBranch(curr->rest, branchNo, seen, seenCounter);
            }
        }

        if (type == 2) {
            if (set->item[1] == '-') {
                return closedBranch(curr->rest, branchNo, seen, seenCounter);
            }
            int negType = parse(set->item + 1);
            if (negType) {
                if (set->item[1] == 'p') {
                    seen[seenCounter++] = 3;
                    return closedBranch(curr->rest, branchNo, seen, seenCounter);
                }
                else if (set->item[1] == 'q') {
                    seen[seenCounter++] = 4;
                    return closedBranch(curr->rest, branchNo, seen, seenCounter);
                }
                else if (set->item[1] == 'r') {
                    seen[seenCounter++] = 5;
                    return closedBranch(curr->rest, branchNo, seen, seenCounter);
                }
            }

            char connective = getConnective(set->item);
            if (connective == '^') {
                if  (!closedBranch(curr->rest, branchNo, seen, seenCounter)) { return 0; }
                if (!closedBranch(curr->rest, branchNo + 1, seen, seenCounter)) { return 0; }
                return 1;
            }
            else if (connective == 'v') {
                return closedBranch(curr->rest, branchNo, seen, seenCounter);
            }
            else {
                return closedBranch(curr->rest, branchNo, seen, seenCounter);
            }
        }

        char connective = getConnective(set->item);
        if (connective == '^') {
            return closedBranch(curr->rest, branchNo, seen, seenCounter);
        }
        else if (connective == 'v') {
            if (!closedBranch(curr->rest, branchNo, seen, seenCounter)) { return 0; }
            if (!closedBranch(curr->rest, branchNo + 1, seen, seenCounter)) { return 0; }
            return 1;
        }
        else {
            if (!closedBranch(curr->rest, branchNo, seen, seenCounter)) { return 0; }
            if (!closedBranch(curr->rest, branchNo + 1, seen, seenCounter)) { return 0; }
            return 1;
        }
    }
}

int closed(struct tableau *t) {

    int branchNo = 0;
    int seen[TabSize];
    int seenCounter = 0;
    return closedBranch(t->rest, branchNo, seen, seenCounter);
}

struct set* copyStartSet(struct set *new, struct set *curr, struct set *start) {

    if (start != curr) {

        new->item = calloc(Fsize, sizeof(char));
        strcpy(new->item, start->item);

        start = start->tail;
        while (start != curr) {
            new->tail = calloc(1, sizeof(struct set));
            new = new->tail;
            new->item = calloc(Fsize, sizeof(char));
            strcpy(new->item, curr->item);
            start = start->tail;
        }

        new->tail = calloc(1, sizeof(struct set));
        return new->tail;
    }

    return new;
}

void copyEndSet(struct set *new, struct set *curr) {
    
    if (curr->tail != NULL) {
        curr = curr->tail;
        while (curr != NULL) {
            new->tail = calloc(1, sizeof(struct set));
            new = new->tail;
            new->item = calloc(Fsize, sizeof(char));
            strcpy(new->item, curr->item);
            curr = curr->tail;
        }
    }
}

struct set* wholeSet(struct set *s) {

    if (s == NULL) { return NULL; }

    struct set *copied = calloc(1, sizeof(struct set));
    struct set *head = copied;
    
    copied->item = calloc(Fsize, sizeof(char));
    strcpy(copied->item, s->item);
    s = s->tail;
    while (s != NULL) {
        copied->tail = calloc(1, sizeof(struct set));
        copied = copied->tail;
        copied->item = calloc(Fsize, sizeof(char));
        strcpy(copied->item, s->item);
        s = s->tail;
    }

    return head;
}

void complete(struct tableau *t) {

    int qStart = 0;
    int qEnd = 1;
    struct set *q[TabSize];
    
    struct tableau *curr = t;
    struct set *start;
    q[0] = wholeSet(t->S);
    while (qStart != qEnd) {
        start = q[qStart++];
        struct tableau *newT = calloc(1, sizeof(struct tableau));

        newT->S = start;
        curr->rest = newT;
        curr = newT;

        struct set *setCounter = start;
        while (setCounter != NULL) {
            char *item = setCounter->item;

            if (item[0] == '-') {
                if (item[1] == '-') {
                    // double negation
                    struct set *new = calloc(1, sizeof(struct set));
                    struct set *head = new;
                    new = copyStartSet(new, setCounter, start);
                    
                    new->item = calloc(Fsize, sizeof(char));
                    int j;
                    int k = 0;
                    for (j = 2; j != strlen(item); ++j) {
                        new->item[k++] = item[j];
                    }
                    new->item[k] = '\0';

                    copyEndSet(new, setCounter);
                    q[qEnd++] = head;
                    setCounter = setCounter->tail;
                    continue;
                }
                else if (item[1] == 'p' || item[1] == 'q' || item[1] == 'r') {
                    // negation of prop
                    setCounter = setCounter->tail;
                    continue;
                }
                
                char connective = getConnective(item);
                if (connective == '^') {
                    // negation and
                    struct set *new = calloc(1, sizeof(struct set));
                    struct set *head = new;
                    new = copyStartSet(new, setCounter, start);
                    struct set *newTail = calloc(1, sizeof(struct set));

                    new->item = calloc(Fsize, sizeof(char));
                    new->item[0] = '-';
                    partone(item + 1, new->item + 1);
                    newTail->item = calloc(Fsize, sizeof(char));
                    newTail->item[0] = '-';
                    parttwo(item + 1, newTail->item + 1);

                    new->tail = newTail;
                    copyEndSet(newTail, setCounter);
                    q[qEnd++] = head;
                    setCounter = setCounter->tail;
                    continue;
                }
                else if (connective == 'v') {
                    // negation or
                    struct set *new1 = calloc(1, sizeof(struct set));
                    struct set *new2 = calloc(1, sizeof(struct set));
                    struct set *head1 = new1;
                    struct set *head2 = new2;
                    new1 = copyStartSet(new1, setCounter, start);
                    new2 = copyStartSet(new2, setCounter, start);

                    new1->item = calloc(Fsize, sizeof(char));
                    new1->item[0] = '-';
                    partone(item + 1, new1->item + 1);
                    new2->item = calloc(Fsize, sizeof(char));
                    new2->item[0] = '-';
                    parttwo(item + 1, new2->item + 1);

                    copyEndSet(new1, setCounter);
                    copyEndSet(new2, setCounter);
                    q[qEnd++] = head1;
                    q[qEnd++] = head2;
                    setCounter = setCounter->tail;
                    break;
                }
                else {
                    // negation implication
                    struct set *new1 = calloc(1, sizeof(struct set));
                    struct set *new2 = calloc(1, sizeof(struct set));
                    struct set *head1 = new1;
                    struct set *head2 = new2;
                    new1 = copyStartSet(new1, setCounter, start);
                    new2 = copyStartSet(new2, setCounter, start);

                    new1->item = calloc(Fsize, sizeof(char));
                    partone(item + 1, new1->item);
                    new2->item = calloc(Fsize, sizeof(char));
                    new2->item[0] = '-';
                    parttwo(item + 1, new2->item + 1);

                    copyEndSet(new1, setCounter);
                    copyEndSet(new2, setCounter);
                    q[qEnd++] = head1;
                    q[qEnd++] = head2;
                    setCounter = setCounter->tail;
                    break;
                }
            }
            
            if (item[0] == 'p' || item[0] == 'q' || item[0] == 'r') { 
                
                setCounter = setCounter->tail;
                continue; 
            }

            char connective = getConnective(item);
            if (connective == '^') {
                // and
                struct set *new1 = calloc(1, sizeof(struct set));
                struct set *new2 = calloc(1, sizeof(struct set));
                struct set *head1 = new1;
                struct set *head2 = new2;
                new1 = copyStartSet(new1, setCounter, start);
                new2 = copyStartSet(new2, setCounter, start);

                new1->item = calloc(Fsize, sizeof(char));
                partone(item, new1->item);
                new2->item = calloc(Fsize, sizeof(char));
                parttwo(item, new2->item);
                
                copyEndSet(new1, setCounter);
                copyEndSet(new2, setCounter);
                q[qEnd++] = head1;
                q[qEnd++] = head2;
                setCounter = setCounter->tail;
                break;
            }
            else if (connective == 'v') {
                // or
                struct set *new = calloc(1, sizeof(struct set));
                struct set *head = new;
                new = copyStartSet(new, setCounter, start);
                struct set *newTail = calloc(1, sizeof(struct set));

                new->item = calloc(Fsize, sizeof(char));
                partone(item, new->item);
                newTail->item = calloc(Fsize, sizeof(char));
                parttwo(item, newTail->item);

                new->tail = newTail;
                copyEndSet(newTail, setCounter);
                q[qEnd++] = head;
                setCounter = setCounter->tail;
                continue;
            }
            else if (connective == '>') {
                // implication
                struct set *new = calloc(1, sizeof(struct set));
                struct set *head = new;
                new = copyStartSet(new, setCounter, start);
                struct set *newTail = calloc(1, sizeof(struct set));

                new->item = calloc(Fsize, sizeof(char));
                new->item[0] = '-';
                partone(item, new->item + 1);
                newTail->item = calloc(Fsize, sizeof(char));
                parttwo(item, newTail->item);

                new->tail = newTail;
                copyEndSet(newTail, setCounter);
                q[qEnd++] = head;
                setCounter = setCounter->tail;
                continue;
            }
        }
    }
}


int main()
{/*input 10 strings from "input.txt" */

    /*You should not need to alter the input and output parts of the program below.*/
    char *name = calloc(Fsize, sizeof(char));
    char *left = calloc(Fsize, sizeof(char));
    char *right = calloc(Fsize, sizeof(char));
    FILE *fp, *fpout;

    /* reads from input.txt, writes to output.txt*/
    if ((  fp=fopen("input.txt","r"))==NULL){printf("Error opening file");exit(1);}
    if ((  fpout=fopen("output.txt","w"))==NULL){printf("Error opening file");exit(1);}

    int j;

    for(j=0;j<inputs;j++)
    {
        fscanf(fp, "%s",name);/*read formula*/
        switch (parse(name))
        {
            case(0): fprintf(fpout, "%s is not a formula.  \n", name);break;
            case(1): fprintf(fpout, "%s is a proposition. \n ", name);break;
            case(2): fprintf(fpout, "%s is a negation.  \n", name);break;
            case(3):
              partone(name, left);
              parttwo(name, right);
              fprintf(fpout, "%s is a binary. The first part is %s and the second part is %s  \n", name, left, right);
              break;
            default:fprintf(fpout, "What the f***!  ");
        }



        if (parse(name)!=0)
        {
          /* Iitialise the tableau with the formula */
          char* s = calloc(Fsize, sizeof(char));
          strcpy(s, name);
          struct set* S = calloc(1, sizeof(struct set));
          S->item = s;
          struct tableau* t = calloc(1, sizeof(struct tableau));
          t->S = S;

          /* Completes the tableau and checks if it is closed */
          complete(t);
          //printTableau(t);
          if (closed(t))  fprintf(fpout, "%s is not satisfiable.\n", name);
          else fprintf(fpout, "%s is satisfiable.\n", name);

          /* Frees all the bits of memory on the tableau*/
          deepFree(t);
        }
        else  fprintf(fpout, "I told you, %s is not a formula.\n", name);
    }

    fclose(fp);
    fclose(fpout);
    free(left);
    free(right);
    free(name);

  return(0);
}