#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int Fsize = 50;
int inputs = 10;
int TabSize = 500;

struct node {
    char *item;
    struct node *left;
    struct node *right;
    struct node *middle;
};

void myFree(struct node *node) {

    if (node == NULL) { return; }

    struct node *left = node->left;
    struct node *right = node->right;
    struct node *middle = node->middle;

    free(node->item);
    free(node);
    myFree(left);
    myFree(right);
    myFree(middle);
}

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


void complete(struct node *node) {

    struct node *q[TabSize];
    int q_start = 0;
    int q_end = 1;

    struct node *start = calloc(1, sizeof(struct node));
    start->item = calloc(Fsize, sizeof(char));
    strcpy(start->item, node->item);
    q[0] = start;

    struct node *curr = start;
    while (q_start != q_end) {
        struct node *expanding = q[q_start++];
        int type = parse(expanding->item);
        if (type) { continue; }

        if (type == 2) {
            if (expanding->item[1] == '-') {
                // double negation.
                struct node *expanded = calloc(1, sizeof(struct node));
                expanded->item = calloc(Fsize, sizeof(char));
                int i; 
                int j;
                for (j = 2; j != curr->item; ++j) {
                    expanded->item[i++] = expanding->item[j];
                }
                expanded->item[i] = '\0';
                curr->middle = expanded;
                curr = curr->middle;
                continue;
            }
        }
    }
}

int main() {

    char *name = calloc(Fsize, sizeof(char));
    char *left = calloc(Fsize, sizeof(char));
    char *right = calloc(Fsize, sizeof(char));
    FILE *fp, *fpout;

    if ((fp = fopen("input.txt", "r")) == NULL) { printf("Error opening file\n"); exit(1); }
    if ((fpout = fopen("output.txt", "w")) == NULL) { prtinf("Error opening file\n"); exit(1); }

    int j;
    for (j = 0; j != inputs; ++j) {
        fscanf(fp, "%s", name);
        int val = parse(name);
        switch (val) {
            case(0): fprintf(fpout, "%s is not a formula.  \n", name); break;
            case(1): fprintf(fpout, "%s is a proposition.  \n", name); break;
            case(2): fprintf(fpout, "%s is a negation.  \n", name); break;
            case(3): partone(name, left);
                     parttwo(name, right);
                     fprintf(fpout, "%s is a binary. The first part is %s and the second part is %s.  \n", name, left, right);
                     break;
            default: fprintf(fpout, "What the f***!  \n");
        }

        if (val != 0) {
            char *s = calloc(Fsize, sizeof(char));
            strcpy(s, name);
            struct node *root = calloc(1, sizeof(struct node));
            root->item = s;

            complete(root);
            if (closed(root)) { fprintf(fpout, "%s is not satisfiable.  \n", name); }
            else { fprintf(fpout, "%s is satisfiable.  \n", name); }

            myFree(root);
        } 
        else { fprintf(fpout, "I told you, %s is not a formula.  \n", name); }
    }

    fclose(fp);
    fclose(fpout);
    free(left);
    free(right);
    free(name);

    return 0;
}