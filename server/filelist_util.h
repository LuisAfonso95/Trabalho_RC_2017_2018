
#define ALREADY_REGISTERED -5
#define EVENT_NOT_FOUND -4
#define NO_SEATS -3
#define USER_NOT_FOUND -2
#define ERROR -1
int GetEventAllFromFile(char *event_file, char *event, int max_size, int index);
int GetEventFixedInfo(char *event_file, char *event, int max_size, int index);
int GetEventFromFile(char *event_file, char *event, int max_size, int index);
int CreateFile(char *file_name);
int EventCheckMaxEntries(char *event_file, int index);
int EventAddRegistry(char *event_file, char *username, int n_of_seats, int index);
int EventCountRegist(char *event_file, int index);
int EventAvailability(char *event_file, int index);


int SearchUserInFile(char *event_file, char *username, int index, char *info);