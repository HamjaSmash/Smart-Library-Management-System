#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>


#define MAX_BOOKS 100
#define MAX_STUDENTS 50
#define MAX_WAIT 30
#define STACK_SIZE 50


typedef struct {
    int id;
    char title[50];
    char author[50];
    int available;
    float price;
} Book;

typedef struct {
    int id;
    char name[30];
    char password[20];
} Student;

typedef struct Borrower {
    int studentId;
    int bookId;
    char date[20];
    struct Borrower *next;
} Borrower;

typedef struct {
    int arr[MAX_WAIT];
    int front, rear;
} WaitQueue;

typedef struct {
    int arr[STACK_SIZE];
    int top;
} ReturnStack;


Book books[MAX_BOOKS];
Student students[MAX_STUDENTS];
WaitQueue waitLists[MAX_BOOKS];
ReturnStack returnStack;
Borrower *borrowHead = NULL;

int bookCount = 0;
int studentCount = 0;

const char LIBRARIAN_USER[] = "Hamja";
const char LIBRARIAN_PASS[] = "Hamja123";

void printCentered(const char *text) {

    int consoleWidth = 80;
    int len = 0;
    while (text[len] != '\0') len++;
    int padding = (consoleWidth - len) / 2;
    for (int i = 0; i < padding; i++) printf(" ");
    printf("%s\n", text);
}

void displayFullAsciiLogo() {
    const char *logo[] = {
        ">>===========================================<<",
        "||  /$$$$$$  /$$       /$$      /$$  /$$$$$$ ||",
        "|| /$$__  $$| $$      | $$$    /$$$ /$$__  $$||",
        "||| $$  \\/| $$      | $$$$  /$$$$| $$  \\/||",
        "|||  $$$$$$ | $$      | $$ $$/$$ $$|  $$$$$$ ||",
        "|| \\____  $$| $$      | $$  $$$| $$ \\____  $$||",
        "|| /$$  \\ $$| $$      | $$\\  $ | $$ /$$  \\ $$||",
        "|||  $$$$$$/| $$$$$$$$| $$ \\/  | $$|  $$$$$$/||",
        "|| \\/ |/|/     |/ \\/ ||",
        ">>===========================================<<"
    };

    int lines = sizeof(logo) / sizeof(logo[0]);

    printf("\033[1;36m");
    for (int i = 0; i < lines; i++) {
        printCentered(logo[i]);
    }
    printf("\033[0m");
}


void getHiddenPassword(char *password) {
    int i = 0;
    char ch;
    while (1) {
        ch = _getch();
        if (ch == 13) {
            password[i] = '\0';
            break;
        } else if (ch == 8 && i > 0) {
            i--;
            printf("\b \b");
        } else if (ch != 8) {
            password[i++] = ch;
            printf("*");
        }
    }
}

void pressEnter() {
    printf("\nPress Enter");
    getchar();
}


void initSystem() {
    for (int i = 0; i < MAX_BOOKS; i++)
        waitLists[i].front = waitLists[i].rear = -1;
    returnStack.top = -1;
}


int findBookIndex(int id) {
    for (int i = 0; i < bookCount; i++)
        if (books[i].id == id) return i;
    return -1;
}

void addBook() {
    if (bookCount >= MAX_BOOKS) { printf("Library is full!\n\n"); return; }

    printf("\n\n========= Add Book =========\n\n");
    printf("Book ID: "); scanf("%d", &books[bookCount].id); getchar();
    printf("Title: "); fgets(books[bookCount].title, 50, stdin);
    books[bookCount].title[strcspn(books[bookCount].title, "\n")] = 0;
    printf("Author: "); fgets(books[bookCount].author, 50, stdin);
    books[bookCount].author[strcspn(books[bookCount].author, "\n")] = 0;
    printf("Price: "); scanf("%f", &books[bookCount].price);

    books[bookCount].available = 1;
    waitLists[bookCount].front = waitLists[bookCount].rear = -1;
    bookCount++;

    printf("\n\nBook Added Successfully!\n\n");
}

void updateBook() {
    int id; printf("Enter Book ID to update: "); scanf("%d", &id);
    int idx = findBookIndex(id);
    if (idx == -1) { printf("\n\nBook not found!\n\n"); return; }
    getchar();

    char temp[50];
    printf("New Title : ");
    fgets(temp, 50, stdin);
    if (temp[0] != '\n') {
        temp[strcspn(temp, "\n")] = 0;
        strcpy(books[idx].title, temp);
    }

    printf("New Author : ");
    fgets(temp, 50, stdin);
    if (temp[0] != '\n') {
        temp[strcspn(temp, "\n")] = 0;
        strcpy(books[idx].author, temp);
    }

    float price;
    printf("New Price : ");
    scanf("%f", &price);
    if (price >= 0) books[idx].price = price;

    printf("\n\nBook Updated Successfully!\n\n");
}

void removeBook() {
    int id; printf("Enter Book ID to remove: "); scanf("%d", &id);
    int idx = findBookIndex(id);
    if (idx == -1) { printf("\n\nBook not found!\n\n"); return; }

    for (int i = idx; i < bookCount - 1; i++) {
        books[i] = books[i + 1];
        waitLists[i] = waitLists[i + 1];
    }
    bookCount--;

    printf("\n\nBook Removed Successfully!\n\n");
}

void viewBooks() {
    if (bookCount == 0) { printf("\n\n No books available!\n\n"); return; }
    printf("--- Book List ---\n\n");
    for (int i = 0; i < bookCount; i++) {
        printf("%d | %s | %s | %s | $%.2f\n",
               books[i].id, books[i].title, books[i].author,
               books[i].available ? "Available" : "Borrowed",
               books[i].price);
    }
}


void registerStudent() {
    if (studentCount >= MAX_STUDENTS) { printf("Max students reached!\n\n"); return; }

    printf("========= Student Registration =========\n\n");
    printf("ID: "); scanf("%d", &students[studentCount].id);
    printf("Name: "); scanf("%s", students[studentCount].name);
    printf("Password: "); getHiddenPassword(students[studentCount].password); printf("\n");

    studentCount++;
    printf("Registration Successful!\n\n");
}

int loginStudent() {
    int id; char pass[20];
    printf("========= Student Login =========\n\n");
    printf("ID: "); scanf("%d", &id);
    printf("Password: "); getHiddenPassword(pass); printf("\n");

    for (int i = 0; i < studentCount; i++) {
        if (students[i].id == id && strcmp(students[i].password, pass) == 0)
            return i;
    }
    printf("Invalid, Try Again\n\n");
    return -1;
}


void addBorrower(int sid, int bid, const char *date) {
    Borrower *newB = malloc(sizeof(Borrower));
    newB->studentId = sid;
    newB->bookId = bid;
    strcpy(newB->date, date);
    newB->next = borrowHead;
    borrowHead = newB;
}

void removeBorrower(int sid, int bid) {
    Borrower *cur = borrowHead, *prev = NULL;
    while (cur) {
        if (cur->studentId == sid && cur->bookId == bid) {
            if (prev) prev->next = cur->next;
            else borrowHead = cur->next;
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void listBorrowers() {
    if (!borrowHead) { printf("No active borrows.\n\n"); return; }
    Borrower *cur = borrowHead;
    while (cur) {
        printf("Student %d has Book %d since %s\n", cur->studentId, cur->bookId, cur->date);
        cur = cur->next;
    }
}


void enqueueWait(int idx, int sid) {
    WaitQueue *q = &waitLists[idx];
    if (q->rear == MAX_WAIT - 1) { printf("Waitlist full!\n\n"); return; }
    if (q->front == -1) q->front = 0;
    q->arr[++q->rear] = sid;
}

int dequeueWait(int idx) {
    WaitQueue *q = &waitLists[idx];
    if (q->front == -1) return -1;
    int sid = q->arr[q->front++];
    if (q->front > q->rear) q->front = q->rear = -1;
    return sid;
}


void pushReturn(int bid) {
    if (returnStack.top < STACK_SIZE - 1)
        returnStack.arr[++returnStack.top] = bid;
}

void viewRecentReturns() {
    printf("Recent Returns:\n");
    for (int i = returnStack.top; i >= 0 && i > returnStack.top - 5; i--)
        printf("- Book ID %d\n", returnStack.arr[i]);
}


void borrowBook(int sidx) {
    int bid; printf("Book ID to borrow: "); scanf("%d", &bid);
    int idx = findBookIndex(bid);
    if (idx == -1) { printf("Book not found!\n"); return; }

    if (books[idx].available) {
        books[idx].available = 0;
        addBorrower(students[sidx].id, bid, "2025-08-12");
        printf("Book Borrowed!\n");
    } else {
        enqueueWait(idx, students[sidx].id);
        printf("Book is unavailable. Added to waitlist.\n");
    }
}

void returnBook(int sidx) {
    int bid; printf("Book ID to return: "); scanf("%d", &bid);
    int idx = findBookIndex(bid);
    if (idx == -1) { printf("Book not found!\n"); return; }

    books[idx].available = 1;
    removeBorrower(students[sidx].id, bid);
    pushReturn(bid);
    printf("Book Returned!\n");

    int next = dequeueWait(idx);
    if (next != -1)
        printf("Student %d can now borrow this book.\n", next);
}

void readBook() {
    int bid; printf("Book ID to read: "); scanf("%d", &bid);
    int idx = findBookIndex(bid);
    if (idx == -1) { printf("Book not found!\n"); return; }
    printf("Reading '%s' by %s (simulated)\n", books[idx].title, books[idx].author);
}

void buyBook() {
    int bid; printf("Book ID to buy: "); scanf("%d", &bid);
    int idx = findBookIndex(bid);
    if (idx == -1) { printf("Book not found!\n"); return; }

    printf("Price: $%.2f\nPayment Method:\n 1.Card\n 2.PayPal/Google Pay\n 3.Bkash/Nagad/Rocket\nChoice: ", books[idx].price);
    int choice; scanf("%d", &choice);

    printf("Purchase Successful!\n\n");
    books[idx].available = 0;
}


void librarianMenu() {
    while (1) {
        printf("========== Librarian Menu =========\n\n");
        printf("1.Add Book\n2.Update Book\n3.Remove Book\n4.View Books\n5.Reports\n6.Fine\n7.View Waitlists\n8.Logout\nChoice: ");
        int ch; scanf("%d", &ch);

        if (ch == 1) addBook();
        else if (ch == 2) updateBook();
        else if (ch == 3) removeBook();
        else if (ch == 4) viewBooks();
        else if (ch == 5) { viewBooks(); viewRecentReturns(); listBorrowers(); }
        else if (ch == 6) { int sid, fine; printf("Student ID: "); scanf("%d", &sid); printf("Fine amount: "); scanf("%d", &fine); printf("Fine of %d issued to student %d\n", fine, sid); }
        else if (ch == 7) {
            for (int i = 0; i < bookCount; i++) {
                if (waitLists[i].front != -1) {
                    printf("Book %d waitlist: ", books[i].id);
                    for (int j = waitLists[i].front; j <= waitLists[i].rear; j++)
                        printf("%d ", waitLists[i].arr[j]);
                    printf("\n");
                }
            }
        }
        else if (ch == 8) break;

        getchar(); pressEnter();
    }
}

void studentMenu(int sidx) {
    while (1) {
        printf("========= Student Menu =========\n\n");
        printf("1.View Books\n2.Read Book\n3.Borrow Book\n4.Return Book\n5.Buy Book\n6.Logout\nChoice: ");

        int ch; scanf("%d", &ch);

        if (ch == 1) viewBooks();
        else if (ch == 2) readBook();
        else if (ch == 3) borrowBook(sidx);
        else if (ch == 4) returnBook(sidx);
        else if (ch == 5) buyBook();
        else if (ch == 6) break;

        getchar(); pressEnter();
    }
}


int main() {
    initSystem();
    displayFullAsciiLogo();

    while (1) {
        printf("\n\n========= Library Management System =========\n\n");
        printf("1.Librarian Login\n2.Student Register\n3.Student Login\n4.Exit \n Choice:");

        int ch;
        scanf("%d", &ch);

        if (ch == 1) {
            char user[20], pass[20];
            printf("\n\n========= Librarian Login =========\n\n");
            printf("Username: "); scanf("%s", user);
            printf("Password: "); getHiddenPassword(pass); printf("\n");

            if (strcmp(user, LIBRARIAN_USER) == 0 && strcmp(pass, LIBRARIAN_PASS) == 0)
                librarianMenu();
            else
                printf("Invalid, Try Again\n");
        }
        else if (ch == 2) registerStudent();
        else if (ch == 3) {
            int idx = loginStudent();
            if (idx != -1) studentMenu(idx);
        }
        else if (ch == 4) break;

        getchar();
    }

    return 0;
}
