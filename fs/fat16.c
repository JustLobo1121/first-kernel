#include "fat16.h"
#include "disk.h"

extern void print(char* message, ...);
extern void itoa(int n, char str[]);

void read_fs_info() {
    char buffer[512];
    read_sector(0, buffer);
    bpb_t* bpb = (bpb_t*) buffer;

    char oem_name[9];
    for (int i=0;i<8;i++) oem_name[i] = bpb->oem_name[i];
    oem_name[8] = '\0';

    char fs_type[9];
    for (int i=0;i<8;i++) fs_type[i] = bpb->system_id[i];
    fs_type[8] = '\0';

    print("\n--- DISC ANALYSIS ---\n");
    print("Manufacturer / OS : "); print(oem_name); print("\n");
    print("Type of format : "); print(fs_type); print("\n");
    
    char str_num[10];
    itoa(bpb->bytes_per_sector, str_num);
    print("Bytes x Sector  : "); print(str_num); print("\n");

    itoa(bpb->total_sectors, str_num);
    print("Total Sectors: "); print(str_num); print("\n");
    print("--------------------------\n");
}

void create_dummy_file() {
    char buffer[512];
    for (int i=0; i<512; i++) buffer[i] = 0;
    dir_entry_t* entry = (dir_entry_t*) buffer;

    char* fake_name = "SECRETO TXT";
    for(int i=0; i<11; i++) {
        if (i < 8) entry[0].name[i] = fake_name[i];
        else entry[0].ext[i-8] = fake_name[i];
    }
    
    entry[0].size = 9000;
    entry[0].attributes = 0x20;
    entry[0].cluster_low = 100;

    write_sector(68, buffer);

    char file_content[512];
    for(int i=0; i<512; i++) file_content[i] = 0;
    
    char* secret_message = "test 0 1 2 3";
    
    for(int i=0; secret_message[i] != '\0' && i < 511; i++) {
        file_content[i] = secret_message[i];
    }
    
    write_sector(100, file_content);

    print("Fake file successfully injected into disk.\n");
}

void list_files() {
    char buffer[512];
    read_sector(68, buffer);
    dir_entry_t* entries = (dir_entry_t*) buffer;

    print("\n--- ROOT DIR ---\n");
    int files_found = 0;
    for (int i=0;i<16;i++) {
        if (entries[i].name[0] == 0x00) {
            break;
        }
        if (entries[i].name[0] == 0xE5) {
            continue;
        }
        char filename[9];
        for(int j=0; j<8; j++) filename[j] = entries[i].name[j];
        filename[8] = '\0';
        
        char ext[4];
        for(int j=0; j<3; j++) ext[j] = entries[i].ext[j];
        ext[3] = '\0';

        print("File: "); print(filename); print("."); print(ext);
        
        char size_str[10];
        itoa(entries[i].size, size_str);
        print(" ("); print(size_str); print(" bytes)\n");
        
        files_found++;
    }
    if (files_found == 0) {
        print("Files not found\n");
        print("---------------\n");
    }
}

void cat_file(char* filename) {
    char buffer[512];
    read_sector(68, buffer);
    dir_entry_t* entries = (dir_entry_t*) buffer;

    for (int i = 0; i < 16; i++) {
        if (entries[i].name[0] == 0x00) break;
        if (entries[i].name[0] == 0xE5) continue;
        int is_match = 1;
        for (int j = 0; j < 8; j++) {
            if (filename[j] == '\0' && entries[i].name[j] == ' ') break;
            if (filename[j] != entries[i].name[j]) {
                is_match = 0;
                break;
            }
        }
        
        if (is_match == 1) {
            int target_sector = entries[i].cluster_low; 
            
            char file_data[512];
            read_sector(target_sector, file_data);
            file_data[511] = '\0';
            
            print("\n");
            print(file_data);
            print("\n\n");
            return;
        }
    }
    
    print("Error: File not found.\n");
}
