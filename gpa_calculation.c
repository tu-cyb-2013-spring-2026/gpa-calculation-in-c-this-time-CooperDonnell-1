#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define INITIAL_CAPACITY 8        // starting size for dynamic array
#define LINE_SIZE 256            // max input line length

typedef struct {
    char semester[10];           // SPRING, SUMMER, FALL
    int year;                   
    int credit_hours;           
    double quality_points;      
} SemesterRecord;

void to_uppercase(char *str) {   // convert string to uppercase
    while (*str) {
        *str = toupper((unsigned char)*str);
        str++;
    }
}

double grade_to_points(char grade) {   // convert letter grade to GPA points
    switch (toupper((unsigned char)grade)) {
        case 'A': return 4.0;
        case 'B': return 3.0;
        case 'C': return 2.0;
        case 'D': return 1.0;
        case 'F': return 0.0;
        default:  return -1.0;         // invalid grade
    }
}

int valid_semester(const char *semester) {   // check valid semester
    return strcmp(semester, "SPRING") == 0 ||
           strcmp(semester, "SUMMER") == 0 ||
           strcmp(semester, "FALL") == 0;
}

int valid_year(int year) {   // basic year validation
    return year >= 1900 && year <= 2100;
}

int find_semester_record(SemesterRecord *records, int count, const char *semester, int year) {
    for (int i = 0; i < count; i++) {   // search existing semesters
        if (records[i].year == year && strcmp(records[i].semester, semester) == 0) {
            return i;                   // found match
        }
    }
    return -1;                        
}

int main(void) {
    char line[LINE_SIZE];               // input buffer
    char grade_char;
    int hours;
    int year;
    char semester[20];

    SemesterRecord *records = malloc(INITIAL_CAPACITY * sizeof(SemesterRecord)); // allocate array
    if (records == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    int count = 0;                     // number of semesters stored
    int capacity = INITIAL_CAPACITY;   // current array size
    int total_hours = 0;               // overall hours
    double total_quality_points = 0.0; // overall quality points

    printf("----> To quit entering grades, enter a blank line.\n");

    while (1) {
        printf("Enter a letter grade and hours for a course with its year and semester: ");

        if (fgets(line, sizeof(line), stdin) == NULL) {
            break;                    // stop if input fails
        }

        if (line[0] == '\n') {
            break;                    // blank line exits loop
        }

        if (sscanf(line, " %c %d %19s %d", &grade_char, &hours, semester, &year) != 4) {
            printf("Invalid input. Use format like: A 3 FALL 2025\n");
            continue;                 // retry on bad format
        }

        grade_char = toupper((unsigned char)grade_char); // normalize grade
        to_uppercase(semester);                          // normalize semester

        double grade_points = grade_to_points(grade_char); // convert grade

        if (grade_points < 0) {
            printf("Invalid grade. Use only A, B, C, D, or F.\n");
            continue;
        }

        if (hours <= 0) {
            printf("Invalid credit hours. Hours must be greater than 0.\n");
            continue;
        }

        if (!valid_semester(semester)) {
            printf("Invalid semester. Use only SPRING, SUMMER, or FALL.\n");
            continue;
        }

        if (!valid_year(year)) {
            printf("Invalid year. Enter a year between 1900 and 2100.\n");
            continue;
        }

        int index = find_semester_record(records, count, semester, year); // find semester

        if (index == -1) {   // if semester not found, create new
            if (count == capacity) {
                capacity *= 2;   // double size
                SemesterRecord *temp = realloc(records, capacity * sizeof(SemesterRecord));
                if (temp == NULL) {
                    printf("Memory reallocation failed.\n");
                    free(records);
                    return 1;
                }
                records = temp;  // update pointer
            }

            strcpy(records[count].semester, semester); // store semester
            records[count].year = year;
            records[count].credit_hours = 0;
            records[count].quality_points = 0.0;
            index = count;
            count++;
        }

        records[index].credit_hours += hours;                 // add hours
        records[index].quality_points += (grade_points * hours); // add quality points

        total_hours += hours;                                 // update overall hours
        total_quality_points += (grade_points * hours);       // update overall points
    }

    printf("-------------------------------------\n");
    printf("Unofficial Transcript\n");

    if (count == 0) {   // no data case
        printf("No course data entered.\n");
        printf("\n--\n");
        printf("Total credit hours: 0\n");
        printf("Overall GPA: 0.000\n");
        printf("-------------------------------------\n");
        free(records);
        return 0;
    }

    for (int i = 0; i < count; i++) {
        double semester_gpa = records[i].quality_points / records[i].credit_hours; // calc GPA
        printf("%-6s %4d (%d hours) %.3f\n",
               records[i].semester,
               records[i].year,
               records[i].credit_hours,
               semester_gpa);
    }

    printf("\n--\n");
    printf("Total credit hours: %d\n", total_hours);
    printf("Overall GPA: %.3f\n", total_quality_points / total_hours); // overall GPA
    printf("-------------------------------------\n");

    free(records);   // free memory
    return 0;
}