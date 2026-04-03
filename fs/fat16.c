#include "fat16.h"
#include "disk.h"
#include "string.h"

extern void print(char* message, ...);
extern void itoa(int n, char str[]);
extern int strlen(char s[]);
unsigned short current_dir_sector = 0;
unsigned short get_next_cluster(unsigned short current_cluster);
int cluster_to_sector(unsigned short cluster);

void read_current_directory(char* buffer) {
    if (current_dir_sector == 0) {
        read_sector(68, buffer);
    } else {
        read_sector(cluster_to_sector(current_dir_sector), buffer);
    }
}

void write_current_directory(char* buffer) {
    if (current_dir_sector == 0) {
        write_sector(68, buffer);
    } else {
        write_sector(cluster_to_sector(current_dir_sector), buffer);
    }
}

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
    for(int i=0; i<512; i++) buffer[i] = 0;
    dir_entry_t* entry = (dir_entry_t*) buffer;
    
    char* fake_name = "SECRET TXT";
    for(int i=0; i<11; i++) {
        if (i < 8) entry[0].name[i] = fake_name[i];
        else entry[0].ext[i-8] = fake_name[i];
    }
    
    entry[0].size = 1000;
    entry[0].attributes = 0x20;
    entry[0].cluster_low = 2;

    write_sector(68, buffer);

    char part1[512];
    for(int i=0; i<512; i++) part1[i] = 0;
    char* txt1 = ">> READING CLUSTER 2... this is part 1. the disk will review the fat table and jump to the next sector...\n";
    for(int i=0; txt1[i] != '\0'; i++) part1[i] = txt1[i];
    write_sector(100, part1);

    char part2[512];
    for(int i=0; i<512; i++) part2[i] = 0;
    char* txt2 = ">> READING CLUSTER 3... this is part 2! the Kernel just follow the map successfuly.\n";
    for(int i=0; txt2[i] != '\0'; i++) part2[i] = txt2[i];
    write_sector(101, part2);

    char fat_table[512];
    read_sector(50, fat_table);
    unsigned short* fat_entries = (unsigned short*) fat_table;
    
    fat_entries[2] = 3;
    fat_entries[3] = 0xFFFF; 
    
    write_sector(50, fat_table);
    print("Fake file successfully injected into disk.\n");
}

void list_files() {
    char buffer[512];
    read_current_directory(buffer);
    dir_entry_t* entries = (dir_entry_t*) buffer;

    if (current_dir_sector == 0) {
        print("\n--- ROOT DIR ---\n");
    } else {
        print("\n--- FOLDER DIR ---\n");
    }
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

        if (entries[i].attributes == 0x10) {
            print("[DIR] ");
            print(filename);
            print("\n");
        } else {
            print("File: "); print(filename); print("."); print(ext);
            char size_str[10];
            itoa(entries[i].size, size_str);
            print(" ("); print(size_str); print(" bytes)\n");
        }        
        files_found++;
    }
    if (files_found == 0) {
        print("Files not found\n");
        print("---------------\n");
    }
}

void cat_file(char* filename) {
    char buffer[512];
    read_current_directory(buffer);
    dir_entry_t* entries = (dir_entry_t*) buffer;
    char padded_name[8];
    int len = strlen(filename);
    for (int i=0; i<8; i++) {
        if (i < len) padded_name[i] = filename[i];
        else padded_name[i] = ' ';
    }

    for (int i = 0; i < 16; i++) {
        if (entries[i].name[0] == 0x00) break;
        if (entries[i].name[0] == 0xE5) continue;

        int is_match = 1;
        for (int j = 0; j < 8; j++) {
            if (padded_name[j] != entries[i].name[j]) {
                is_match = 0;
                break;
            }
        }
        
        if (is_match == 1) {
            unsigned short cluster = entries[i].cluster_low; 
            print("\n");
            while (cluster >= 2 && cluster < 0xFFF8) {
                int target_sector = cluster_to_sector(cluster);
                char file_data[512];
                read_sector(target_sector, file_data);
                file_data[511] = '\0';
                print(file_data);
                cluster = get_next_cluster(cluster);
            }
            print("\n\n");
            return;
        }
    }
    
    print("Error: File not found.\n");
}

int cluster_to_sector(unsigned short cluster) {
    return 100 + (cluster - 2);
}

unsigned short get_next_cluster(unsigned short current_cluster) {
    int fat_offset = current_cluster * 2;
    int fat_sector = 50 + (fat_offset / 512);
    int entry_offset = fat_offset % 512;
    
    char buffer[512];
    read_sector(fat_sector, buffer); 
    
    unsigned char low_byte = (unsigned char) buffer[entry_offset];
    unsigned char high_byte = (unsigned char) buffer[entry_offset + 1];
    
    unsigned short next_cluster = (high_byte << 8) | low_byte;
    
    return next_cluster;
}

unsigned short find_free_cluster() {
    char fat_table[512];
    read_sector(50, fat_table);

    unsigned short* fat_entries = (unsigned short*) fat_table;
    for (int i=2; i<256; i++) {
        if (fat_entries[i] == 0x0000) {
            return i;
        }
    }
    return 0;
}

int find_free_dir_entry(dir_entry_t* entries) {
    for (int i=0; i<16; i++) {
        if (entries[i].name[0] == 0x00 || entries[i].name[0] == 0xE5) {
            return i;
        }
    }
    return -1;
}

void write_file(char* filename, char* content) {
    unsigned short free_cluster = find_free_cluster();
    if (free_cluster == 0) {
        print("Error: disk is full\n");
        return;
    }

    char dir_buffer[512];
    read_current_directory(dir_buffer);
    dir_entry_t* entries = (dir_entry_t*) dir_buffer;

    int free_entry = find_free_dir_entry(entries);
    if (free_entry == -1) {
        print("Error: cannot create more file within the main root\n");
        return;
    }

    for (int i = 0; i < 11; i++) {
        if (i < 8) entries[free_entry].name[i] = filename[i];
        else entries[free_entry].ext[i-8] = filename[i];
    }

    entries[free_entry].size = strlen(content);
    entries[free_entry].attributes = 0x20;
    entries[free_entry].cluster_low = free_cluster;
    write_sector(68, dir_buffer);

    int target_sector = cluster_to_sector(free_cluster);
    char data_buffer[512];

    for(int i=0; i<512; i++) data_buffer[i] = 0;
    
    for(int i=0; content[i] != '\0' && i < 511; i++) {
        data_buffer[i] = content[i];
    }
    write_sector(target_sector, data_buffer);

    char fat_table[512];
    read_sector(50, fat_table);
    unsigned short* fat_entries = (unsigned short*) fat_table;
    
    fat_entries[free_cluster] = 0xFFFF;
    write_sector(50, fat_table);

    print("file '"); print(filename); print("' save successfuly.\n");
}

void make_directory(char* dirname) {
    unsigned short free_cluster = find_free_cluster();
    if (free_cluster == 0) {
        print("Error: disk is full\n");
        return;
    }

    char dir_buffer[512];
    read_current_directory(dir_buffer);
    dir_entry_t* entries = (dir_entry_t*) dir_buffer;

    int free_entry = find_free_dir_entry(entries);
    if (free_entry == -1) {
        print("Error: root dir is full\n");
        return;
    }

    int len = strlen(dirname);
    for (int i=0; i<11; i++) {
        if (i<8) {
            entries[free_entry].name[i] = (i<len) ? dirname[i]: ' ';
        } else {
            entries[free_entry].ext[i-8] = ' ';
        }
    }

    entries[free_entry].size = 0;
    entries[free_entry].attributes = 0x10;
    entries[free_entry].cluster_low = free_cluster;
    write_sector(68, dir_buffer);

    int target_sector = cluster_to_sector(free_cluster);
    char empty_cluster[512];
    for (int i=0; i<512; i++) empty_cluster[i] = 0;
    write_sector(target_sector, empty_cluster);

    char fat_table[512];
    read_sector(50, fat_table);
    unsigned short* fat_entries = (unsigned short*) fat_table;
    fat_entries[free_cluster] = 0xFFFF;
    write_sector(50, fat_table);
    print("Directory '");print(dirname);print("' successfuly create\n");
}

void change_directory(char* dirname) {
    if (strcmp(dirname, "/") == 0 || strcmp(dirname, "..") == 0) {
        current_dir_sector = 0;
        print("current position: root directory\n");
        return;
    }

    char buffer[512];
    read_current_directory(buffer);
    dir_entry_t* entries = (dir_entry_t*) buffer;

    char padded_name[8];
    int len = strlen(dirname);
    for (int i=0; i<8; i++) padded_name[i] = (i<len) ? dirname[i]: ' ';

    for (int i = 0; i < 16; i++) {
        if (entries[i].name[0] == 0x00) break;
        if (entries[i].name[0] == 0xE5) continue;

        int is_match = 1;
        for (int j = 0; j < 8; j++) {
            if (padded_name[j] != entries[i].name[j]) {
                is_match = 0; break;
            }
        }

        if (is_match == 1) {
            if (entries[i].attributes == 0x10) {
                current_dir_sector = entries[i].cluster_low;
                print("Entering the directory '"); print(dirname); print("'\n");
            } else {
                print("Error: '"); print(dirname); print("' this is a file not a directory\n");
            }
            return;
        }
    }
    print("Error: directory not found\n");
}