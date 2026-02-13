//Aditi Hande   1002177674
//OS- Jude Agujiobi 
//Lab03 Disks Assignment

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 256
char current_user[50] = "guest";   // default username

// Each 256-byte chunk of the disk is a block.
// Some blocks hold file data, others hold metadata.

// ----- Data block -----
typedef struct {
    unsigned char data[BLOCK_SIZE];   // 256 bytes of raw file content
} Content;

// ----- Block header / navigation info -----
typedef struct {
    int current_block;   // index of this block
    int next_block;      // next block of same file (-1 if none)
    int prev_block;      // previous block (-1 if none)
    int bytes_used;      // number of bytes actually used in this block
} Header;

// ----- File details (metadata) -----
typedef struct {
    char name[56];       // file name
    char path[56];       // optional: for directories later
    char owner[50];      // owner name
    int size_bytes;      // total size of file
    int first_block;     // index of first data block
    int rwx;             // read/write/execute flags
    int locked;          // 0 = unlocked, 1 = locked
    int block_pointers[6];
} FileDetails;

// ----- Superblock -----
typedef struct {
    int total_blocks;    // total number of 256-byte blocks
    int free_blocks;     // how many are still free
    int bitmap_start;    // block index where bitmap begins
    int fnt_start;       // start of File Name Table
    int dabpt_start;     // start of DABPT
    int data_start;      // start of data region
    int num_files;
    int block_ptrs;   //(needed for remove)
} SuperBlock;

// ---- function prototypes ----
void create_fs(char *disk_name, int num_blocks);
void format_fs(char *disk_name, int filenames, int dabpt_entries);
void list_files(char *disk_name);
void put_file(char *disk_name, char *host_file);
void get_file(char *disk_name, char *fs_file);
void create_dir(char *disk_name, char *dir_name);
void change_dir(char *disk_name, char *path);
void save_fs(char *disk_name);
void open_fs(char *disk_name);
void rename_file(char *disk_name, char *old_name, char *new_name);
void remove_file(char *disk_name, char *file_name);
void set_user(char *username);
void link_file(char *disk_name, char *source_name, char *link_name);
void save_fs(char *target_name);

//char currDisk[50];
char current_disk[100] = "";
int disk_open = 0;   // 0 = none open, 1 = a disk is open

int main() {
    char input[100];        // entire command line
    char command[50];       // first word (Createfs, Formatfs, Exit, etc.)
    char disk_name[50];     // name of the disk file
    char host_file[100];
    char fs_file[100];
    char old_name[100]; 
    char new_name[100];
    char file_name[100];
    int num_blocks;         // number of blocks for Createfs
    int num_files;          // number of directory entries for Formatfs
    int num_ptrs;           // number of block pointers per file for Formatfs
    char name[100];
    char arg1[100];
    char arg2[100];

    while (1) {
        printf("FS> ");
        //scanf("%s", command);
        fgets(input, sizeof(input), stdin);

        // Added cleanup to strip newline + ignore blank input
        input[strcspn(input, "\n")] = 0;   // remove newline
        if (strlen(input) == 0) continue;  // skip empty/whitespace lines

        // Extract first word (command) only once
        if (sscanf(input, "%s", command) != 1) continue;

        // Createfs command section
        if (strcmp(command, "Createfs") == 0) {
            if (sscanf(input, "%*s %s %d", disk_name, &num_blocks) == 2) {
                create_fs(disk_name, num_blocks);
            } else {
                printf("Usage: Createfs <disk_name> <num_blocks>\n");
            }
        }

       // Formatfs command section
        else if (strcmp(command, "Formatfs") == 0) {
            if (sscanf(input, "%*s %s %d %d", disk_name, &num_files, &num_ptrs) == 3) {
                printf("[DEBUG] disk=%s files=%d ptrs=%d\n", disk_name, num_files, num_ptrs); // 🔹 Debug print
                format_fs(disk_name, num_files, num_ptrs);
            } else {
                printf("Usage: Formatfs <disk_name> <num_dir_entries> <block_ptrs>\n");
            }
        }

        else if (strcmp(command, "List") == 0) {
    int args = sscanf(input, "%*s %s", name);

    if (disk_open) {
        if (args > 0) {
            printf("Error: disk already open ('%s'). Do not specify a disk name.\n", current_disk);
        } else {
            list_files("");
        }
    } else {
        if (args == 1)
            list_files(name);
        else
            printf("Error: no disk open. Use Openfs <disk_name> first.\n");
    }
}


        else if (strcmp(command, "Put") == 0) {
    int args = sscanf(input, "%*s %s %s", name, arg1);

    if (disk_open) {
        if (args == 2) {
            printf("Error: disk already open ('%s'). Do not specify a disk name.\n", current_disk);
        } else if (args == 1) {
            put_file("", name);
        } else {
            printf("Usage: Put <host_file>\n");
        }
    } else {
        printf("Error: no disk open. Use Openfs <disk_name> first.\n");
    }
}

        else if (strcmp(command, "Get") == 0) {
    int args = sscanf(input, "%*s %s %s", name, arg1);

    if (disk_open) {
        if (args == 2) {
            printf("Error: disk already open ('%s'). Do not specify a disk name.\n", current_disk);
        } else if (args == 1) {
            get_file("", name);
        } else {
            printf("Usage: Get <fs_file>\n");
        }
    } else {
        printf("Error: no disk open. Use Openfs <disk_name> first.\n");
    }
}

        
        else if (strcmp(command, "Savefs") == 0) {
    char name[100] = "";
    sscanf(input, "%*s %s", name);
    save_fs(name);
}


        else if (strcmp(command, "Openfs") == 0) {
            if (sscanf(input, "%*s %s", name) == 1) {
                open_fs(name);
            } 
            else {
                printf("Usage: Openfs <disk_name>\n");
            }
        }


        else if (strcmp(command, "User") == 0) {
        // Example: User Aditi
            if (sscanf(input, "%*s %s", name) == 1) {
                set_user(name);
            } 
            else {
                printf("Usage: User <username>\n");
            }
        }

else if (strcmp(command, "Remove") == 0) {
    int args = sscanf(input, "%*s %s %s", name, arg1);

    if (disk_open) {
        if (args == 2) {
            printf("Error: disk already open ('%s'). Do not specify a disk name.\n", current_disk);
        } else if (args == 1) {
            remove_file("", name);
        } else {
            printf("Usage: Remove <file_name>\n");
        }
    } else {
        printf("Error: no disk open. Use Openfs <disk_name> first.\n");
    }
}


        else if (strcmp(command, "Link") == 0) {
    int args = sscanf(input, "%*s %s %s %s", name, arg1, arg2);

    if (disk_open) {
        if (args == 3) {
            printf("Error: disk already open ('%s'). Do not specify a disk name.\n", current_disk);
        } else if (args == 2) {
            link_file("", name, arg1);
        } else {
            printf("Usage: Link <source_file> <link_name>\n");
        }
    } else {
        printf("Error: no disk open. Use Openfs <disk_name> first.\n");
    }
}


        else if (strcmp(command, "Exit") == 0) {
            break;
        }
        else {
            printf("Unknown command.\n");
        }
    }
    return 0;
}

// Create a new filesystem (virtual disk)
void create_fs(char *disk_name, int num_blocks) {
    FILE *disk = fopen(disk_name, "wb");
    if (!disk) {
        printf("Error: cannot create disk file.\n");
        return;
    }

    SuperBlock sb;
    sb.total_blocks = num_blocks;
    sb.free_blocks = num_blocks - 1; // 1 block used for superblock itself
    sb.bitmap_start = 0;                 
    sb.fnt_start = 0;                    
    sb.dabpt_start = 0;
    sb.data_start = 0;  

    // Write the superblock in the first 256 bytes
    fwrite(&sb, sizeof(SuperBlock), 1, disk);

    // --- pad remaining bytes of the first block ---
    if (sizeof(SuperBlock) < BLOCK_SIZE) {
    char padding[BLOCK_SIZE - sizeof(SuperBlock)];
    memset(padding, 0, sizeof(padding));
    fwrite(padding, sizeof(padding), 1, disk);
    }

    // Fill the rest of the disk with zero bytes
    char empty_block[BLOCK_SIZE] = {0};
    for (int i = 1; i < num_blocks; i++) {
        fwrite(empty_block, BLOCK_SIZE, 1, disk);
    }

    fclose(disk);
    printf("File system '%s' created with %d blocks (256 bytes each).\n",
           disk_name, num_blocks);
}

void format_fs(char *disk_name, int num_dir_entries, int num_block_ptrs)
{
    FILE *disk = fopen(disk_name, "r+b");
    if (!disk) {
        printf("Error: could not open disk '%s'\n", disk_name);
        return;
    }

    SuperBlock sb;
    sb.total_blocks = 0;
    sb.free_blocks = 0;
    sb.num_files = num_dir_entries;
    sb.bitmap_start = 1;                 // bitmap at block 1
    sb.fnt_start = 2;                    // file name table after bitmap
    sb.dabpt_start = sb.fnt_start + num_dir_entries / 8 + 1;
    sb.data_start = sb.dabpt_start + 1;  // simple layout; refine later

    // determine total blocks by checking file size
    fseek(disk, 0, SEEK_END);         //open file and go to its end
    long disk_size = ftell(disk);     //use ftell to get the file size
    sb.total_blocks = disk_size / BLOCK_SIZE;   //use the filesize/blocksize(hardcoded 256 bytes) = total blocks

    // --- 1. write superblock in block 0 ---
    fseek(disk, 0, SEEK_SET);
    fwrite(&sb, sizeof(SuperBlock), 1, disk);

    // --- 2. initialize bitmap (block 1) ---
    fseek(disk, BLOCK_SIZE * sb.bitmap_start, SEEK_SET);
    unsigned char bitmap[BLOCK_SIZE];
    memset(bitmap, 0, BLOCK_SIZE);
    // mark blocks 0 and 1 as used
    bitmap[0] = 0b00000011;                               ////////////////?
    fwrite(bitmap, BLOCK_SIZE, 1, disk);

    // --- 3. clear directory table region (block 2 etc.) ---
    fseek(disk, BLOCK_SIZE * sb.fnt_start, SEEK_SET);
    unsigned char zero[BLOCK_SIZE] = {0};
    for (int i = 0; i < num_dir_entries; i++) {
        fwrite(zero, BLOCK_SIZE, 1, disk);
    }

    fclose(disk);

    printf("Formatted disk '%s' with %d directory entries and %d block pointers per file.\n",
           disk_name, num_dir_entries, num_block_ptrs);
           
    //printf("No. of files/sb.num_dir_entries is %d\n\n", sb.num_files);
}

// list_files() ====
void list_files(char *disk_name) {
        // Use currently opened disk if none given
    if (strlen(disk_name) == 0) {
        if (!disk_open) {
            printf("Error: no disk is currently open.\n");
            return;
        }
        disk_name = current_disk;
    }

    FILE *disk = fopen(disk_name, "rb");
    if (!disk) {
        printf("Error: could not open disk '%s'\n", disk_name);
        return;
    }

    // Read the superblock
    SuperBlock sb;
    fseek(disk, 0, SEEK_SET);
    fread(&sb, sizeof(SuperBlock), 1, disk);

    printf("Listing files on disk '%s':\n", disk_name);
    printf("--------------------------------------------------\n");

    // Move to the File Name Table start
    fseek(disk, BLOCK_SIZE * sb.fnt_start, SEEK_SET);

    // Each entry is 256 bytes (FileDetails or placeholder)
    FileDetails file;
    int file_count = 0;   ///incrmnt
    //printf("No. of files in disk is %d\n\n", file_count);
    for (int i = 0; i < sb.num_files; i++) {  // temporary limit (will use sb.num_files later)
        fread(&file, sizeof(FileDetails), 1, disk);

        // Only print if name is not empty
        if (strlen(file.name) > 0) {
            printf("%-25s %6d bytes  (owner: %s)\n",
                   file.name, file.size_bytes, file.owner);

            file_count++;
        }
    }

    if (file_count == 0)
        printf("(no files found)\n");

    printf("--------------------------------------------------\n");
    fclose(disk);
}

// put_file() ====
void put_file(char *disk_name, char *host_file) {
        // Use currently opened disk if none given
    if (strlen(disk_name) == 0) {
        if (!disk_open) {
            printf("Error: no disk is currently open.\n");
            return;
        }
        disk_name = current_disk;
    }

    FILE *disk = fopen(disk_name, "r+b");
    if (!disk) {
        printf("Error: could not open disk '%s'\n", disk_name);
        return;
    }

    FILE *src = fopen(host_file, "rb");
    if (!src) {
        printf("Error: could not open source file '%s'\n", host_file);
        fclose(disk);
        return;
    }

    // --- read superblock ---
    SuperBlock sb;
    fseek(disk, 0, SEEK_SET);
    fread(&sb, sizeof(SuperBlock), 1, disk);

    // --- find first empty FileDetails entry ---
    FileDetails file;
    int found_index = -1;

    fseek(disk, BLOCK_SIZE * sb.fnt_start, SEEK_SET);
    for (int i = 0; i < sb.num_files; i++) {  // temp limit
        long pos = ftell(disk);
        fread(&file, sizeof(FileDetails), 1, disk);
        if (strlen(file.name) == 0) {
            found_index = i;
            fseek(disk, pos, SEEK_SET);
            break;
        }
    }

    if (found_index == -1) {
        printf("No free directory entry available.\n");
        fclose(disk);
        fclose(src);
        return;
    }

    // --- fill FileDetails struct ---
    memset(&file, 0, sizeof(FileDetails));
    strncpy(file.name, host_file, sizeof(file.name) - 1);
    file.size_bytes = 0;
    file.locked = 0;

    // --- write file content into data blocks ---
    unsigned char buffer[BLOCK_SIZE];
    int bytes_read, total_written = 0;
    long data_start_pos = BLOCK_SIZE * sb.data_start;

    fseek(disk, data_start_pos, SEEK_SET);
    while ((bytes_read = fread(buffer, 1, BLOCK_SIZE, src)) > 0) {
        fwrite(buffer, 1, bytes_read, disk);
        total_written += bytes_read;
    }

    file.size_bytes = total_written;

    strcpy(file.owner, current_user);  // record who owns the file

    // --- write file metadata entry back ---
    fseek(disk, BLOCK_SIZE * (sb.fnt_start + found_index), SEEK_SET);
    fwrite(&file, sizeof(FileDetails), 1, disk);

    printf("File '%s' added to disk '%s' (%d bytes).\n",
           host_file, disk_name, total_written);

    fclose(src);
    fclose(disk);
}

//get_file() ====
void get_file(char *disk_name, char *fs_file) {
        // Use currently opened disk if none given
    if (strlen(disk_name) == 0) {
        if (!disk_open) {
            printf("Error: no disk is currently open.\n");
            return;
        }
        disk_name = current_disk;
    }

    FILE *disk = fopen(disk_name, "rb");
    if (!disk) {
        printf("Error: could not open disk '%s'\n", disk_name);
        return;
    }

    // --- read superblock ---
    SuperBlock sb;
    fseek(disk, 0, SEEK_SET);
    fread(&sb, sizeof(SuperBlock), 1, disk);

    // --- find requested file in File Name Table ---
    FileDetails file;
    int found = 0;

    fseek(disk, BLOCK_SIZE * sb.fnt_start, SEEK_SET);
    for (int i = 0; i < sb.num_files; i++) { // temporary fixed limit
        fread(&file, sizeof(FileDetails), 1, disk);
        if (strcmp(file.name, fs_file) == 0) {
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("File '%s' not found on disk '%s'.\n", fs_file, disk_name);
        fclose(disk);
        return;
    }

    // --- open a new file on host system to copy data ---
    FILE *out = fopen(fs_file, "wb");
    if (!out) {
        printf("Error: could not create output file '%s'.\n", fs_file);
        fclose(disk);
        return;
    }

    unsigned char buffer[BLOCK_SIZE];
    long data_start_pos = BLOCK_SIZE * sb.data_start;
    fseek(disk, data_start_pos, SEEK_SET);

    int bytes_remaining = file.size_bytes;
    while (bytes_remaining > 0) {
        int chunk = (bytes_remaining > BLOCK_SIZE) ? BLOCK_SIZE : bytes_remaining;
        fread(buffer, 1, chunk, disk);
        fwrite(buffer, 1, chunk, out);
        bytes_remaining -= chunk;
    }

    fclose(out);
    fclose(disk);

    printf("File '%s' extracted from disk '%s' (%d bytes).\n",
           fs_file, disk_name, file.size_bytes);
}

// ==== save_fs() ====
void save_fs(char *target_name) {
    if (!disk_open) {
        printf("Error: no disk is currently open.\n");
        return;
    }

    if (strlen(target_name) == 0) {
        printf("Usage: Savefs <new_disk_name>\n");
        return;
    }

    FILE *src = fopen(current_disk, "rb");
    if (!src) {
        printf("Error: could not open current disk '%s' for reading.\n", current_disk);
        return;
    }

    FILE *dst = fopen(target_name, "wb");
    if (!dst) {
        fclose(src);
        printf("Error: could not create backup disk '%s'\n", target_name);
        return;
    }

    unsigned char buffer[BLOCK_SIZE];
    size_t bytes;
    while ((bytes = fread(buffer, 1, BLOCK_SIZE, src)) > 0) {
        fwrite(buffer, 1, bytes, dst);
    }

    fclose(src);
    fclose(dst);

    printf("File system '%s' saved as '%s' successfully.\n", current_disk, target_name);
}


void open_fs(char *disk_name) {
    FILE *disk = fopen(disk_name, "rb");
    if (!disk) {
        printf("Error: could not open disk '%s'\n", disk_name);
        return;
    }

    SuperBlock sb;
    fread(&sb, sizeof(SuperBlock), 1, disk);
    fclose(disk);

    // remember this disk globally
    strncpy(current_disk, disk_name, sizeof(current_disk) - 1);
    current_disk[sizeof(current_disk) - 1] = '\0';
    disk_open = 1;

    printf("File system '%s' opened successfully.\n", disk_name);
    printf("Superblock info:\n");
    printf("  Total blocks: %d\n", sb.total_blocks);
    printf("  Block size: %d bytes\n", BLOCK_SIZE);
    printf("  File Name Table starts at block: %d\n", sb.fnt_start);
    printf("  Data region starts at block: %d\n", sb.data_start);
}


//set_user() ====
void set_user(char *username) {
    strncpy(current_user, username, sizeof(current_user) - 1);
    current_user[sizeof(current_user) - 1] = '\0';
    printf("Current user set to '%s'\n", current_user);
}


//rename_file() ====
void rename_file(char *disk_name, char *old_name, char *new_name) {
        // Use currently opened disk if none given
    if (strlen(disk_name) == 0) {
        if (!disk_open) {
            printf("Error: no disk is currently open.\n");
            return;
        }
        disk_name = current_disk;
    }

    FILE *disk = fopen(disk_name, "r+b");
    if (!disk) {
        printf("Error: could not open disk '%s'\n", disk_name);
        return;
    }

    SuperBlock sb;
    fseek(disk, 0, SEEK_SET);
    fread(&sb, sizeof(SuperBlock), 1, disk);

    FileDetails file;
    int found = 0;

    // move to start of File Name Table
    fseek(disk, BLOCK_SIZE * sb.fnt_start, SEEK_SET);

    // search for the file entry
    for (int i = 0; i < sb.num_files; i++) {
        long pos = ftell(disk);
        fread(&file, sizeof(FileDetails), 1, disk);

        if (strcmp(file.name, old_name) == 0) {
            found = 1;
            strcpy(file.name, new_name);  // rename
            fseek(disk, pos, SEEK_SET);
            fwrite(&file, sizeof(FileDetails), 1, disk);
            printf("File '%s' renamed to '%s' in disk '%s'.\n",
                   old_name, new_name, disk_name);
            break;
        }
    }

    if (!found)
        printf("File '%s' not found on disk '%s'.\n", old_name, disk_name);

    fclose(disk);
}


void remove_file(char *disk_name, char *file_name) {
        // Use currently opened disk if none given
    if (strlen(disk_name) == 0) {
        if (!disk_open) {
            printf("Error: no disk is currently open.\n");
            return;
        }
        disk_name = current_disk;
    }

    FILE *disk = fopen(disk_name, "r+b");
    if (!disk) {
        printf("Error: cannot open %s\n", disk_name);
        return;
    }

    SuperBlock sb;
    fread(&sb, sizeof(SuperBlock), 1, disk);

    // move to File Name Table start
    fseek(disk, BLOCK_SIZE * sb.fnt_start, SEEK_SET);

    FileDetails file;
    int found = 0;
    long pos = 0;

    for (int i = 0; i < sb.num_files; i++) {
        pos = ftell(disk);
        fread(&file, sizeof(FileDetails), 1, disk);

        // skip empty names
        if (strlen(file.name) == 0)
            continue;

        // compare filenames
        if (strcmp(file.name, file_name) == 0) {
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("File '%s' not found on disk '%s'.\n", file_name, disk_name);
        fclose(disk);
        return;
    }

    // check ownership
    if (strcmp(file.owner, current_user) != 0) {
        printf("Permission denied. File belongs to '%s'.\n", file.owner);
        fclose(disk);
        return;
    }

    // load and modify bitmap
    fseek(disk, BLOCK_SIZE * sb.bitmap_start, SEEK_SET);
    unsigned char bitmap[BLOCK_SIZE];
    fread(bitmap, 1, BLOCK_SIZE, disk);

    for (int i = 0; i < sb.block_ptrs; i++) {
        int blk = file.block_pointers[i];
        if (blk > 0)
            bitmap[blk / 8] &= ~(1 << (blk % 8));  // mark block free
    }

    fseek(disk, BLOCK_SIZE * sb.bitmap_start, SEEK_SET);
    fwrite(bitmap, 1, BLOCK_SIZE, disk);

    // clear the FNT entry (actually delete)
    memset(&file, 0, sizeof(FileDetails));
    fseek(disk, pos, SEEK_SET);
    fwrite(&file, sizeof(FileDetails), 1, disk);

    printf("File '%s' (owner: %s) removed successfully from disk '%s'.\n",
           file_name, current_user, disk_name);

    fclose(disk);
}

void link_file(char *disk_name, char *source_name, char *link_name) {
        // Use currently opened disk if none given
    if (strlen(disk_name) == 0) {
        if (!disk_open) {
            printf("Error: no disk is currently open.\n");
            return;
        }
        disk_name = current_disk;
    }

    FILE *disk = fopen(disk_name, "r+b");
    if (!disk) {
        printf("Error: cannot open %s\n", disk_name);
        return;
    }

    SuperBlock sb;
    fread(&sb, sizeof(SuperBlock), 1, disk);

    fseek(disk, BLOCK_SIZE * sb.fnt_start, SEEK_SET);
    FileDetails file, link;
    int found = 0;

    // find the source file
    for (int i = 0; i < sb.num_files; i++) {
        fread(&file, sizeof(FileDetails), 1, disk);
        if (strcmp(file.name, source_name) == 0) {
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Source file '%s' not found.\n", source_name);
        fclose(disk);
        return;
    }

    // create link entry (same blocks, new name)
    strcpy(link.name, link_name);
    strcpy(link.owner, current_user);
    link.size_bytes = file.size_bytes;
    link.first_block = file.first_block;
    link.rwx = file.rwx;
    link.locked = file.locked;
    for (int i = 0; i < 6; i++)
        link.block_pointers[i] = file.block_pointers[i];

    // write link to next free FNT slot
    fseek(disk, BLOCK_SIZE * sb.fnt_start, SEEK_SET);
    for (int i = 0; i < sb.num_files; i++) {
        long pos = ftell(disk);
        FileDetails temp;
        fread(&temp, sizeof(FileDetails), 1, disk);
        if (strlen(temp.name) == 0) {
            fseek(disk, pos, SEEK_SET);
            fwrite(&link, sizeof(FileDetails), 1, disk);
            printf("Linked '%s' -> '%s' (shared data blocks).\n", link_name, source_name);
            fclose(disk);
            return;
        }
    }

    printf("Error: no free entries in file table.\n");
    fclose(disk);
}


void create_dir(char *disk_name, char *dir_name){ printf("Called CreateDir\n"); }
void change_dir(char *disk_name, char *path)    { printf("Called ChangeDir\n"); }
