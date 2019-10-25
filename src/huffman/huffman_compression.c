#include"../FILE_IO/File_IO.h"
#include"huffman_compression.h"

/* The function calls different modules required
 * for creating a huffman tree for a given file.
 */

void compress_by_huffman(char *file_name) {
    int dist_chars, arr[MAX_BITS];
    tree_elements *char_table;
    priority_quee *A;
    codebook *huffman_code, *canonical_code;

    // Get a character table containing 256 ASCII characters.
    get_char_table(&char_table);

    // Get the frequency count of characters present in file.
    get_char_freq(&char_table, file_name);
    
    // Get the number of distinct characters present in the file.
    dist_chars = get_dist_chars(char_table);

    // Create a priority_Quee for based on character frequencys.
    create_priority_quee(&A, dist_chars, char_table);
    
    // Get the bit strings for each character.
    huffman_code = get_codebook(dist_chars); 
    Get_Huffman_bit_strings(A, huffman_code, arr, 0); 
    
    // Map the bits strings to get the canonical codes , based on the bit lenghts.
    canonical_code = map_bit_strings(huffman_code, dist_chars); 

    // Call encode function to get compressed file. 
    encode_file(canonical_code, dist_chars ,file_name); 
    
    
    free(huffman_code);
    free(canonical_code);
    remove_priority_quee(A);
}

/* The Function constructs a character table.
 * The tables has 256 distinct characters as per
 * ASCII table and initializes the frequency of 
 * each character to zero.
 */

void get_char_table(tree_elements **char_table) {
    int i = 0;
    tree_elements *temp;
    temp = (tree_elements *)malloc(sizeof(tree_elements) * NUM_CHARS);
    if(temp == NULL) {
        perror("The error is :");
        exit(1);
    }
    for(i = 0; i < NUM_CHARS; i++) {
        write_charachter(temp + i, (char)i);
        write_frequency(temp + i, 0);
    }
    *char_table = temp;
}

/* Function gets the frequency of characters appearing
 * in the file , stores it in the char_table.  
 */

void get_char_freq(tree_elements **char_table, char *file_name) {
    int fd = 0, index = 0;
    char c;
    tree_elements *temp = *char_table;
    open_file_read(&fd, file_name);
    while(freadchar(fd, &c)) {
        index = (int)c; 
        write_frequency(temp + index, get_frequency(temp, index) + 1);
    }  
    close(fd);
}

/* Function gets the count of distinct characters
 * in charater table.
 */

int get_dist_chars(tree_elements *char_table) {
    int i = 0, count = 0;
    for(i = 0; i < NUM_CHARS; i++) {
        if(get_frequency(char_table, i) != 0) {
            count++;
        }
    }
    return count;
}

/* Creates a priority quee based on the distinct
 * characters and corresponding frequency.
 * (MIN HEAP is implemented as priority quee)
 */

void create_priority_quee(priority_quee **A, int num_dist_char, tree_elements *char_table) {
    heap h;
    priority_quee temp, *ptr1, *ptr2;
    InitHeap(&h, num_dist_char);

    for(int i = 0; i < NUM_CHARS; i++) {
        if(get_frequency(char_table, i) != 0) {
            temp = get_heap_element(get_character(char_table, i), get_frequency(char_table, i));     
            // Insert every character encontered in the MIN-HEAP.
            Insert_minHeap(&h, temp);
        } 
    }
    while(!isSizeOne(h)) {
        // Get the 1st minimum element.
        ptr1 = Remove_minHeap(&h);

        // Get the 2nd minimum element.
        ptr2 = Remove_minHeap(&h); 
        
        // Form a subtree based on the 2 minimum elements extracted.
        temp = Create_subtree(ptr1, ptr2);
        
        // Insert the new tree element in the MIN-HEAP.
        Insert_minHeap(&h, temp);
    } 
    // Remove the last element left in HEAP, this is the root of huffman tree. 
    ptr1 = Remove_minHeap(&h);  
    *A = ptr1;

    destroyHeap(&h); 
    free(char_table); 
}

/* Gets the huffman bit strings for each character 
 * using huffman tree by inoder traversing  
 */

void Get_Huffman_bit_strings(priority_quee *A, codebook *temp,int arr[], int index) {
    static int i = 0;
    if(A->left) {
        arr[index] = 0;
        Get_Huffman_bit_strings(A->left, temp, arr, index + 1);
    }    
    if(A->right) {
        arr[index] = 1;
        Get_Huffman_bit_strings(A->right, temp, arr, index + 1);
    }
    if(IsLeaf(*A)) {
        temp[i].symbol = get_quee_character(*A); 
        get_bit_string(temp[i].str, arr, index);
        i++;
    }
}

/* Gets a bit string based on the array  
 * from 0 to index containing 1 and 0 
 */

void get_bit_string(char *str, int arr[], int index) {
    int i = 0;
    for(i = 0; i < index; i++) {
        if(arr[i]) {
            str[i] = '1';
        }
        else {
            str[i] = '0'; 
        }
    }
    str[i] = '\0'; 
}

