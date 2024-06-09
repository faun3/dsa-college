// https://github.com/zeekliviu/SDDSubjects2023/blob/main/Subiectul%203/subiect%203.jpeg

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include <time.h>

typedef struct Booking {
    unsigned int id;
    char* hotelName;
    unsigned char hotelRooms;
    unsigned char bookedRooms;
    char* clientName;
    char* bookedDates;
    float pricePerNight;
} Booking;

Booking makeBooking(unsigned int id, char* hotelName, unsigned char hotelRooms, 
    unsigned char bookedRooms, char* clientName, char* bookedDates, float pricePerNight) {
    Booking b = { id, _strdup(hotelName), hotelRooms, bookedRooms, _strdup(clientName), _strdup(bookedDates), pricePerNight };
    return b;
}

Booking cloneBooking(Booking b) {
    return makeBooking(b.id, b.hotelName, b.hotelRooms, b.bookedRooms, b.clientName, b.bookedDates, b.pricePerNight);
}

void printBooking(Booking b) {
    printf("Booking: (id: %u hotelName: %s, hotelRooms: %hhu, bookedRooms: %hhu, clientName: %s, bookedDates: %s, pricePerNight: %.2f)\n",
        b.id, b.hotelName, b.hotelRooms, b.bookedRooms, b.clientName, b.bookedDates, b.pricePerNight);
}

int hash(char* hotelName, int tableSize) {
    int val = 17;
    int len = strlen(hotelName);
    for (int i = 0; i < len; i++) {
        val += hotelName[i] * 31;
    }

    return val % tableSize;
}

typedef struct HashTable {
    Booking* data;
    int size;
} HashTable;

#define DEFAULT_TABLE_SIZE 20

HashTable makeHashTable() {
    HashTable h = { (Booking*)malloc(sizeof(Booking) * DEFAULT_TABLE_SIZE), DEFAULT_TABLE_SIZE };
    for (int i = 0; i < h.size; i++) {
        h.data[i] = makeBooking(0, NULL, 0, 0, NULL, NULL, 0);
    }
    return h;
}

void printHashTable(HashTable h) {
    for (int i = 0; i < h.size; i++) {
        printf("Bucket idx: %d\n", i);
        if (h.data[i].hotelName != NULL) {
            printBooking(h.data[i]);
        }
    }
}

void freeHashTable(HashTable* h) {
    for (int i = 0; i < h->size; i++) {
        free(h->data[i].hotelName);
        free(h->data[i].clientName);
        free(h->data[i].bookedDates);
    }
    free(h->data);
    h->data = NULL;
    h->size = 0;
}

void insertIntoHashTable(HashTable* h, Booking b) {
    int idx = hash(b.hotelName, h->size);

    if (h->data[idx].hotelName == NULL) {
        h->data[idx] = b;
    } else {
        int startIdx = (idx + 1) % h->size;
        while (startIdx != idx) {
            if (h->data[startIdx].hotelName == NULL) {
                h->data[startIdx] = b;
                break;
            } else {
                startIdx = (idx + 1) % h->size;
            }
        }
    }
}

void parseFile(HashTable h, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        return;
    }

    unsigned int id;
    char hotelName[256], clientName[256], bookedDates[256];
    unsigned char bookedRooms, hotelRooms;
    float pricePerNight;

    while (fscanf(file, "%u \"%[^\"]\" %hhu %hhu \"%[^\"]\" \"%[^\"]\" %f", 
        &id, hotelName, &hotelRooms, &bookedRooms, clientName, bookedDates, &pricePerNight) == 7) {
        Booking b = makeBooking(id, hotelName, hotelRooms, bookedRooms, clientName, bookedDates, pricePerNight);
        insertIntoHashTable(&h, b);
    }

    fclose(file);
}

float getPercentBooked(HashTable h, char* hotelNameFilter, char* bookedDatesFilter) {
    int idx = hash(hotelNameFilter, h.size);

    float percent = 0;

    if (h.data[idx].hotelName != NULL && strcmp(h.data[idx].hotelName, hotelNameFilter) == 0) {
        if (strcmp(bookedDatesFilter, h.data[idx].bookedDates) == 0) {
            percent = (float)h.data[idx].bookedRooms / (float)h.data[idx].hotelRooms * 1.0;
        }
    } else {
        int startIdx = (idx + 1) % h.size;
        while (startIdx != idx) {
            if (h.data[startIdx].hotelName != NULL && strcmp(h.data[startIdx].hotelName, hotelNameFilter) == 0) {
                if (strcmp(bookedDatesFilter, h.data[startIdx].bookedDates) == 0) {
                    percent = (float)h.data[startIdx].bookedRooms / (float)h.data[startIdx].hotelRooms;
                    break;
                } else {
                    startIdx = (idx + 1) % h.size;
                }
            }
        }
    }

    return percent;
}

Booking* bookingsByCertainClient(HashTable h, char* clientNameFilter, int* sz) {
    Booking* arr = NULL;
    *sz = 0;

    for (int i = 0; i < h.size; i++) {
        if (h.data[i].hotelName != NULL && strcmp(clientNameFilter, h.data[i].clientName) == 0) {
            (*sz)++;
            Booking* tmp = realloc(arr, sizeof(Booking) * (*sz));
            tmp[(*sz) - 1] = cloneBooking(h.data[i]);
            arr = tmp;
        }
    }

    return arr;
}

// this should be illegal to include in the exam
// when tm and time_t were never even mentioned
//
// sscanf -> reads over a string buffer for data that matches a pattern
double calculateEarnings(Booking* arr, int sz) {
    double total = 0.0;

    for (int i = 0; i < sz; i++) {
        unsigned int day1, month1, year1, day2, month2, year2;
        (void)sscanf(arr[i].bookedDates, "%d-%d-%d : %d-%d-%d", &day1, &month1, &year1, &day2, &month2, &year2);
        struct tm time1 = { 0 };
        struct tm time2 = { 0 };

        time1.tm_year = year1 - 1900;
        time1.tm_mday = day1;
        time1.tm_mon = month1 - 1;

        time2.tm_year = year2 - 1900;
        time2.tm_mday = day2;
        time2.tm_mon = month2 - 1;

        time_t t1 = mktime(&time1);
        time_t t2 = mktime(&time2);

        int dayDiff = (int)(difftime(t2, t1) / (60 * 60 * 24));

        total += dayDiff * arr[i].pricePerNight;
    }

    return total;
}

int bookedRoomsByClientName(HashTable h, char* clientNameFilter) {
    int rooms = 0;
    for (int i = 0; i < h.size; i++) {
        if (h.data[i].hotelName != NULL && strcmp(h.data[i].clientName, clientNameFilter) == 0) {
            rooms = h.data[i].bookedRooms;
            break;
        }
    }
    return rooms;
}

int main() {
    HashTable h = makeHashTable();
    parseFile(h, "data.txt");
    printHashTable(h);

    float booked = getPercentBooked(h, "The Bucharest Grand", "09-06-2024 : 12-06-2024");
    printf("\nBooked %% in The Bucharest Grand: %f\n", booked);

    int sz;
    Booking* bb = bookingsByCertainClient(h, "Silviu Gherman", &sz);
    printf("\nBooked by Silviu Gherman:\n");
    for (int i = 0; i < sz; i++) {
        printBooking(bb[i]);
    }

    double earnings = calculateEarnings(bb, sz);
    printf("\nEarnings in array: %lf\n", earnings);

    int rooms = bookedRoomsByClientName(h, "Maria Ionescu");
    printf("\nRooms booked by Maria Ionescu: %d\n", rooms);

    freeHashTable(&h);
    for (int i = 0; i < sz; i++) {
        free(bb[i].hotelName);
        free(bb[i].clientName);
        free(bb[i].bookedDates);
    }
    free(bb);
}