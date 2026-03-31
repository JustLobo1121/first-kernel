#ifndef FAT16_H
#define FAT16_H
#pragma pack(push, 1)

typedef struct {
    unsigned char  jump[3];
    char oem_name[8];
    unsigned short bytes_per_sector;
    unsigned char sectors_per_cluster;
    unsigned short reserved_sectors;
    unsigned char fat_copies;
    unsigned short root_dir_entries;
    unsigned short total_sectors;
    unsigned char media_descriptor;
    unsigned short sectors_per_fat;
    unsigned short sectors_per_track;
    unsigned short heads;
    unsigned int hidden_sectors;
    unsigned int large_sector_count;

    unsigned char drive_number;
    unsigned char reserved;
    unsigned char signature;
    unsigned int volume_id;
    char volume_label[11];
    char system_id[8];
} bpb_t;

typedef struct {
    unsigned char name[8];
    unsigned char ext[3];
    unsigned char attributes;
    unsigned char reserved;
    unsigned char create_time_ms;
    unsigned short create_time;
    unsigned short create_date;
    unsigned short access_date;
    unsigned short cluster_high;
    unsigned short modified_time;
    unsigned short modified_date;
    unsigned short cluster_low;
    unsigned int size;
} dir_entry_t;

#pragma pack(pop)
#endif