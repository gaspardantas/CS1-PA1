#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STUDENTS   100
#define MAX_NAME_LEN   21
#define MAX_FILE_NAME  31

typedef struct {
    int   id;
    char  firstName[MAX_NAME_LEN];
    char  lastName[MAX_NAME_LEN];
    float gpa;
} Student;

/* --- Prototypes --- */
static int  binarySearch(const Student s[], int size, int id);   // classic binary search on sorted IDs
static int  loadStudents(FILE *f, Student arr[]);                // parse file into array
static int  isSorted(FILE *f);                                   // sanity-check: IDs strictly increasing

/* --- Implementations --- */
static int binarySearch(const Student s[], int size, int id) {
    int l = 0, h = size - 1;
    while (l <= h) {
        int m = (l + h) / 2;
        if (s[m].id == id) {
            return m;               // found
        } else if (s[m].id < id) {
            l = m + 1;              // search right half
        } else {
            h = m - 1;              // search left half
        }
    }
    return -1;                      // not found
}

static int loadStudents(FILE *f, Student arr[]) {
    int c = 0;
    while (c < MAX_STUDENTS) {
        int   id;
        char  fn[MAX_NAME_LEN], ln[MAX_NAME_LEN];
        float gpa;

        int got = fscanf(f, "%d %20s %20s %f", &id, fn, ln, &gpa);
        if (got == EOF) break;      // normal end
        if (got != 4) {             // malformed line
            printf("Bad line at #%d\n", c + 1);
            return -1;
        }

        arr[c].id = id;
        strncpy(arr[c].firstName, fn, MAX_NAME_LEN);
        arr[c].firstName[MAX_NAME_LEN - 1] = '\0';
        strncpy(arr[c].lastName, ln, MAX_NAME_LEN);
        arr[c].lastName[MAX_NAME_LEN - 1] = '\0';
        arr[c].gpa = gpa;
        c++;
    }
    return c;                       // number of students loaded
}

static int isSorted(FILE *f) {
    int   prev = -1, cur;
    char  temp1[MAX_NAME_LEN], temp2[MAX_NAME_LEN];
    float tmpGPA;

    // read-through check: IDs must be strictly increasing
    while (fscanf(f, "%d %20s %20s %f", &cur, temp1, temp2, &tmpGPA) == 4) {
        if (prev != -1 && cur <= prev) {
            return 0;               // not strictly increasing
        }
        prev = cur;
    }
    return 1;
}

int main(void) {
    char file[MAX_FILE_NAME];

    printf("Enter the file name (including extension): ");
    if (scanf("%30s", file) != 1) {         // bound input to avoid overflow
        printf("Invalid file name input.\n");
        return 1;
    }

    FILE *f = fopen(file, "r");
    if (!f) {
        printf("Can't open %s\n", file);
        return 1;
    }

    if (!isSorted(f)) {
        printf("IDs are not strictly ordered.\n");
        fclose(f);
        return 1;
    }

    rewind(f);     // important: reset after isSorted() pass

    Student list[MAX_STUDENTS];
    int total = loadStudents(f, list);
    fclose(f);

    if (total < 1) {
        printf("No data loaded.\n");
        return 1;
    }

    int id;
    for (;;) {
        printf("Enter student ID to search (-1 to quit): ");
        if (scanf("%d", &id) != 1) {
            printf("Invalid input.\n");

            // clear stdin until end of line
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) { /* discard */ }
            continue;
        }
        if (id == -1) break;

        int pos = binarySearch(list, total, id);
        if (pos != -1) {
            printf("[%d, %s %s, GPA: %.2f]\n",
                   list[pos].id,
                   list[pos].firstName,
                   list[pos].lastName,
                   list[pos].gpa);
        } else {
            printf("This ID is not in the records.\n");
        }
    }

    printf("Bye.\n");
    return 0;
}