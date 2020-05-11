// This program acts as an inventory for a small shop. A lot of the code is based on the code for the phonebook.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define SEARCH 1
#define ADD 2
#define DELETE 3
#define LIST 4
#define EXIT 5
#define DATABASESIZE 1000
#define MAXCHARACTERS 30

struct details {  // used a similar data structure to phonebook code
    char* itemName;
    char* price;
    char* barcode;
    char* stockNum;
    char* location;  // added feature
    char* numSold;
};

struct details *database[DATABASESIZE];
void displayOptions(void);
void start(void);
int getOptions(void);
void doOption(int);
void list(void);
void search(void);
void add(void);
void delete(void);
void removeItem(char*, int);
char* readString(char*);   // used phonebook code here
struct details *addItem(char*, char*, char*, char*, char*, char*);  // used phonebook code here
void initialiseData(void);
void save(void);
void load(void);
void removeNewLine(char*);  // used phonebook code here
void checkStock(void);    // added feature 
void update(void);
void updateOption(int, int);
void updatePrice(int);
void updateStockNum(int);
void updateBarcode(int);
void updateLocation(int);
void enterSale(void);   // enter sales of items and update stock
void bestseller(void);  // checks which item has been sold the most and least. Then outputs it to user
void locationList(void);

void start() {  
    while (true) {      // loop only terminates when user inputs 8
        displayOptions();
        int option = getOptions();
        if (option == 12) { 
           break;
        }
        else {
            doOption(option);
        }
    }
}

void displayOptions() {
    printf("\nEnter 1 to search for an item.\n");
    printf("Enter 2 to add an item to the database.\n");
    printf("Enter 3 to delete an item from the database.\n");
    printf("Enter 4 to list all items in the database.\n");
    printf("Enter 5 to save database to a file.\n");
    printf("Enter 6 to load a saved database.\n");
    printf("Enter 7 to check which items are low in stock.\n");
    printf("Enter 8 to update items.\n");
    printf("Enter 9 to record sale.\n");
    printf("Enter 10 to find the bestseller and worst selling items in store.\n");
    printf("Enter 11 to find all items in a certain location.\n");
    printf("Enter 12 to exit program.\n");
}

int getOptions() {
    int option;
    char *input = readString("\nEnter your choice here: ");
    sscanf(input, "%i", &option);
    free(input);
    return option;
}

void doOption(int option) {
    switch(option) {
        case 1 : search(); break;
        case 2 : add(); break;
        case 3 : delete(); break;
        case 4 : list(); break;
        case 5 : save(); break;
        case 6 : load(); break;
        case 7 : checkStock(); break;
        case 8 : update(); break;
        case 9 : enterSale(); break; 
        case 10 : bestseller(); break;
        case 11 : locationList(); break;
        default : printf("That is not a valid option. Try again.\n");
    }
}

void search() {
    char *input = readString("Enter the name of the item you wish to search for: ");
    int index;
    for (index = 0; database[index] != NULL; index++) {  // searches for item name until it reaches item or NULL
        if (strcmp(database[index]->itemName, input) == 0) {
            printf("\nThe price of %s is: %s.\n", database[index]->itemName, database[index]->price);
            printf("The barcode of %s is: %s.\n", database[index]->itemName, database[index]->barcode);
            printf("There are %s in stock.\n", database[index]->stockNum);
            printf("The location of %s is %s.\n", database[index]->itemName, database[index]->location);
            printf("The number of %s sold is %s.\n", database[index]->itemName, database[index]->numSold);
            free(input);
            return;
        }
        else {
            continue;
        }
    }
    printf("Sorry, %s could not be found in database.\n", input);
    free(input);
    return;
}

void add() {
    char *name = readString("Enter the name of the item you wish to add: ");
    char *price = readString("Enter the price of the item you wish to add: ");
    char *barcode = readString("Enter the barcode of the item you wish to add: ");
    char *stockNum = readString("Enter the number in stock: ");
    char *location = readString("Enter the location where the item can be found in the store: ");
    char *numSold = readString("Enter the number sold: ");
    int index = 0;
    while ((database[index] != NULL) && (strcmp(database[index]->itemName,name) != 0)) {
        index++;
    }
    if (index > DATABASESIZE - 2) {
        printf("The database is full. Item details could not be entered.\n");
    }
    else {
        struct details *entry = addItem(name, barcode, price, stockNum, location, numSold);
        database[index] = entry;
        database[index+1] = '\0';
        printf("%s has been added to database.\n", name);
    }
    free(name);   // freeing the heap memory from the pointers initialised in this function
    free(price);
    free(barcode);
    free(stockNum);
    free(location);
    free(numSold);
    return;
}

void delete() {
    char *input = readString("Enter the name of the item you wish to remove: ");
    int index;
    for (index = 0; database[index] != NULL; index++) {
        if (strcmp(database[index]->itemName,input) == 0) {   // looks for item name in database until it reaches item or NULL
            removeItem(input, index);
            printf("%s has been deleted from the database.\n", input);
            free(input);
            return;
        }
    }
    printf("%s could not be found in the database.\n", input);
    free(input);
}

void removeItem(char* input, int index) {
    struct details *removed = database[index];
    do {
        database[index] = database[index + 1];
    } while (database[++index] != NULL);
    free(removed->itemName);   // frees the heap memory that the pointer removed is pointing at
    free(removed->barcode);
    free(removed->price);
    free(removed->stockNum);
    free(removed->location);
    free(removed->numSold);
    free(removed);
    return;
}

char* readString(char* prompt) {  // reads user input and returns it in char * type while also creating memory in heap 
    char array[MAXCHARACTERS];
    printf("%s", prompt);
    fgets(array, MAXCHARACTERS, stdin);
    size_t length = strlen(array);
    char *entry = calloc(sizeof(char), length);
    strncpy(entry, array, length - 1);
    entry[length] = '\0';
    return entry;
}

struct details *addItem(char* itemName, char* barcode, char* price, char* stockNum, char* location, char* numSold) { // function creates memory then copies the data from parameters into the heap memory. creates 1 structure which it returns
    struct details *entry = calloc(sizeof(struct details),1);
    entry->itemName = calloc(sizeof(char), strlen(itemName) + 1);
    strcpy(entry->itemName, itemName);
    entry->barcode = calloc(sizeof(char), strlen(barcode) + 1);
    strcpy(entry->barcode, barcode);
    entry->price = calloc(sizeof(char), strlen(price) + 1);
    strcpy(entry->price, price);
    entry->stockNum = calloc(sizeof(char), strlen(stockNum) + 1);
    strcpy(entry->stockNum, stockNum);
    entry->location = calloc(sizeof(char), strlen(location) + 1);
    strcpy(entry->location, location);
    entry->numSold = calloc(sizeof(char), strlen(numSold) + 1);
    strcpy(entry->numSold, numSold);
    return entry;
}

void initialiseData() {
    database[0] = addItem("Cereal bar", "1234", "10123", "4", "Aisle 10", "1");
    database[1] = addItem("Chocolate", "1354", "13546", "5", "Warehouse", "2");
    database[2] = NULL;
}

void list() {
    int index = 0;
    while ((database[index] != NULL) && (index < DATABASESIZE)) {   // lists all item names one by one until it reaches NULL
        printf("The item name is: %s.\n", database[index]->itemName);
        index++;
    }
}

void save() {
    char *input = readString("Enter the output filename here: ");
    FILE *outputFile = fopen(input,"w");
    if (outputFile == NULL) {
        printf("Could not save file.\n");
        fclose(outputFile);
        free(input);
        return;
    }
    int index;
    for (index = 0; database[index] != NULL; index++) {       // copying data from database into file
        fprintf(outputFile, "Name: %s\n", database[index]->itemName);
        fprintf(outputFile, "Price: %s\n", database[index]->price);
        fprintf(outputFile, "Barcode: %s\n", database[index]->barcode);
        fprintf(outputFile, "Number in stock: %s\n", database[index]->stockNum);
        fprintf(outputFile, "Location: %s\n", database[index]->location);
        fprintf(outputFile, "Number sold: %s\n", database[index]->numSold);
    }
    printf("The database has been saved.\n");
    fclose(outputFile);
    free(input);
}

void load() {
    char *input = readString("Enter the name of the file: ");
    FILE *file = fopen(input,"r");
    if (file == NULL) {
        printf("The file you entered is invalid.\n");
        fclose(file);
        free(input);
    }
    int index;
    char itemName[MAXCHARACTERS];
    char price[MAXCHARACTERS];
    char barcode[MAXCHARACTERS];
    char stockNum[MAXCHARACTERS];
    char location[MAXCHARACTERS];
    char numSold[MAXCHARACTERS];
    for (index = 0; index < DATABASESIZE - 1; index++) {
        fgets(itemName,MAXCHARACTERS - 1, file);
        char *data = fgets(price, MAXCHARACTERS - 1, file);     // getting data from the specified file
        char *data1 = fgets(barcode, MAXCHARACTERS - 1, file);
        char *data2 = fgets(stockNum, MAXCHARACTERS - 1, file);
        char *data3 = fgets(location, MAXCHARACTERS - 1, file);
        char *data4 = fgets(location, MAXCHARACTERS - 1, file);
        if (data == NULL) {
            fclose(file);
            free(input);
            break;
        }
        removeNewLine(itemName);     // removes \n from end of data
        removeNewLine(price);
        removeNewLine(barcode);
        removeNewLine(stockNum);
        removeNewLine(location);
        removeNewLine(numSold);
        database[index] = addItem(itemName, barcode, price, stockNum, location, numSold);
    }
    database[index] = NULL;
    printf("Database has been loaded.\n");
    fclose(file);
    free(input);
}

void removeNewLine(char *array) {
    size_t length = strlen(array);
    if (length == 0) {
        return;
    }
    if (array[length - 1] == '\n') {    
        array[length - 1] = '\0';      // adds NULL to end
    }
}

void checkStock() {
    int index;
    char numb[2] = {'5','\0'};     // lower stock limit
    char high[4] = {'5','0','0','\0'};  // upper stock limit
    size_t len = strlen(high);
    size_t length = strlen(numb);
    char *upperlim = calloc(sizeof(char), len);
    char *threshold = calloc(sizeof(char), length);  // making a type char* so we can compare it to stock number
    strncpy(upperlim, high, len);
    strncpy(threshold, numb, length);
    for (index = 0; database[index] != NULL; index++) {
        if ((strcmp(database[index]->stockNum, threshold) < 0) && (strlen(database[index]->stockNum) < 2)) {     // check if stockNum is greater than 5 or has more than 2 digits. if it has 2 digits it is greater than 5
            printf("%s is running low on stock. Only %s are left, order more.\n", database[index]->itemName, database[index]->stockNum);
        }
        else if ((strcmp(database[index]->stockNum, upperlim) > 0) && (strlen(database[index]->stockNum) > 2)) {
            printf("%s is overstocked. There are currently %s in stock. Don't order more deliveries.\n", database[index]->itemName, database[index]->stockNum);
        }

    }
    free(threshold);
    free(upperlim);
}

void update() {
    char *input = readString("Enter the name of the item you wish to update: ");
    int index;
    for (index = 0; database[index] != NULL; index++) {
        if (strcmp(database[index]->itemName,input) == 0) {
            while (true) {
                printf("\nEnter 1 to update the price of %s.\n", database[index]->itemName);
                printf("Enter 2 to update the number of %s in stock.\n", database[index]->itemName);
                printf("Enter 3 to update the barcode of %s.\n", database[index]->itemName);
                printf("Enter 4 to update location of %s.\n", database[index]->itemName);
                printf("Enter 5 to return to menu.\n");
                int option = getOptions();
                if (option == 5) {
                    free(input);
                    break;
                }
                updateOption(option, index);
            }
        }
    }
}

void updateOption(int option, int index) {
    switch(option) {
        case 1 : updatePrice(index); break;
        case 2 : updateStockNum(index); break;
        case 3 : updateBarcode(index); break;
        case 4 : updateLocation(index); break;
        default : printf("Please enter a valid option.\n");
    }
}

void updatePrice(int index) {
    free(database[index]->price);
    char *input = readString("Enter the new price here: ");
    database[index]->price = calloc(sizeof(char), strlen(input) + 1);
    strcpy(database[index]->price, input);
    free(input);
}

void updateStockNum(int index) {
    free(database[index]->stockNum);
    char *input = readString("Enter the new number in stock: ");
    database[index]->stockNum = calloc(sizeof(char), strlen(input) + 1);
    strcpy(database[index]->stockNum, input);
    free(input);
}

void updateBarcode(int index) {
    free(database[index]->barcode);
    char *input = readString("Enter the new barcode here: ");
    database[index]->barcode = calloc(sizeof(char), strlen(input) + 1);
    strcpy(database[index]->barcode, input);
    free(input);
}

void updateLocation(int index) {
    free(database[index]->location);
    char *input = readString("Enter the new location here: ");
    database[index]->location = calloc(sizeof(char), strlen(input) + 1);
    strcpy(database[index]->location, input);
    free(input);
}

void enterSale() {
    char *input = readString("Enter the name of the item sold here: ");
    int index;
    for (index = 0; database[index] != NULL; index++) {
        if (strcmp(database[index]->itemName, input) == 0) {
            char *numberSold = readString("Enter the number sold here: ");
            free(database[index]->numSold);
            database[index]->numSold = calloc(sizeof(char), strlen(numberSold) + 1);
            strcpy(database[index]->numSold, numberSold);
            printf("Current number of %s in stock is %s.\n", database[index]->itemName, database[index]->stockNum);
            updateStockNum(index);
            printf("The sale has been recorded and the number of %s in stock has been updated.\n", database[index]->itemName);
            free(input);
            free(numberSold);
            return;
        }
    }
    printf("%s could not be found in the database.\n", input);
    free(input);
}

void bestseller() {    // make another function using strcmp
    int index;
    char array[1] = {'0'};
    char *largest = database[0]->numSold;
    char *name = database[0]->itemName;
    char *smallest = database[0]->numSold;
    char *name1 = database[0]->itemName;
    for (index = 0; database[index] != NULL; index++) {
        if (database[index]->numSold > largest) {
            largest = database[index]->numSold;
            name = database[index]->itemName;
        }
        else if ((database[index]->numSold < largest) && (*database[index]->numSold != array[0])) {
            smallest = database[index]->numSold;
            name1 = database[index]->itemName;
        }
        if (*database[index]->numSold == array[0]) {
            printf("%s hasn't sold at all.\n", database[index]->itemName);
        }
    }
    printf("The bestseller item is %s. The number sold is %s.\n", name, largest);
    printf("The worst selling item that has sold any units is %s. Only %s were sold.\n", name1, smallest);
}

void locationList() {
    char *location = readString("Enter the name of the location here: ");
    int index, count = 0;
    for (index = 0; database[index] != NULL; index++) {
        if (strcmp(database[index]->location, location) == 0) {
            printf("%s is in %s.\n", database[index]->itemName, database[index]->location);
            count++;
        }
    }
    if (count == 0) {
        printf("This location contains no items.\n");
    }
    free(location);
}

int main(void) {
    database[0] = NULL;
    initialiseData();
    start();
    return 0;
}