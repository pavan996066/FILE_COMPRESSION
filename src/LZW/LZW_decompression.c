#include "LZW_decompression.h"
#include "../FILE_IO/File_IO.h"

void decompress_by_LZW(char *file_name) {
    DICT A;
    initialize_dictionary(&A, DICT_SIZE);
    decode_file_LZW(file_name, A); 
}

void decode_file_LZW(char *file_name, DICT A) {
    int fd, fdd;
    char str[25], prev[25], curr[25], output_file[100];

    check_file_extention(file_name);
    tokenize_file_name(output_file, file_name);

    open_file_read(&fd, file_name);
    open_file_write(&fdd, output_file);

    int index, bit_string_read, cur_len = BIT_LEN, temp, len, index_dic;
    char BUFFER = 0;
    bit_string_read = 0;
    index = 0;
    temp = 0;
    prev[0] = '\0';
    curr[0] = '\0';
    index_dic = NUM_CHARS;

    while(freadchar(fd, &BUFFER)) {
        for(;(index < READ_BUF) && (bit_string_read < cur_len); index++) {
            //printf("index - %d , bit_string_read = %d ,and bit value %d\n", index, bit_string_read, BUFFER & 128);
            if(BUFFER & 128) {
                str[bit_string_read] = '1';
            }
            else {
                str[bit_string_read] = '0';
            }
            BUFFER = BUFFER << 1;
            bit_string_read++;
        }
        if(bit_string_read == cur_len) {
            str[bit_string_read] = '\0';
            temp = convert_string_to_decimal(str);
            if(temp == index_dic) {
                //printf("INDEX PROBLEM - index - %d curr and prev :%s",index_dic, curr);
                len = strlen(prev);
                prev[len] = curr[0]; 
                prev[len + 1] = '\0';
                add_in_dictionary(&A, prev, index_dic);
                write(fdd, prev, strlen(prev));
                //printf("In dict will have index %d and key(prev):%s having len %d\n",index_dic,prev,strlen(prev));
                index_dic++;
            }
            else {
                strcpy(curr, A[temp].key);
                write(fdd, curr, strlen(curr));
                //printf("index - %d  dict value (curr):%s having len %d\n",temp, curr, strlen(prev));
                if(strlen(prev) != 0) {
                    len = strlen(prev); 
                    prev[len] = curr[0];
                    prev[len + 1] = '\0';
                    add_in_dictionary(&A, prev, index_dic);
                    //printf("In dict will have index %d and key(prev):%s having len %d\n",index_dic,prev,strlen(prev));
                    index_dic++;
                } 
                strcpy(prev, curr);
            } 
            bit_string_read = 0;
            for(;index < READ_BUF; index++) {
                //printf("index - %d , bit_string_read = %d ,and bit value %d\n", index, bit_string_read, BUFFER & 128);
                if(BUFFER & 128) {
                    str[bit_string_read] = '1';
                }
                else {
                    str[bit_string_read] = '0';
                }
                BUFFER = BUFFER << 1;
                bit_string_read++;
            }
            index = 0;
            BUFFER = 0;
        }  
        if(index == READ_BUF) {
            index = 0;
            BUFFER = 0;
        } 
    }
    print_dictionary(A, index_dic);
}

int convert_string_to_decimal(char *str) {
    int temp = 0, i, pow = 1;
    for(i = strlen(str) - 1; i >= 0; i--) {
        if(str[i] == '1') {
            temp += pow;
        }
        pow = pow * 2;
    }
    return temp;
}
