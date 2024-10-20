#include <stdio.h>

void print_hex_memory(const unsigned char* data, size_t size) {
    size_t offset = 0; // 记录当前偏移量

    while (offset < size) {
        size_t lineBytes = size - offset < 16 ? size - offset : 16; // 每行最多打印16字节

        printf("%08zx  ", offset); // 打印偏移量

        // 打印十六进制数据
        for (size_t i = 0; i < lineBytes; i++) {
            printf("%02x ", data[offset + i]);
        }

        // 对齐补足打印空格（如果当前行不足16字节）
        if (lineBytes < 16) {
            for (size_t i = lineBytes; i < 16; i++) {
                printf("   ");
            }
        }

        // 打印字符（可显示的部分）
        printf(" |");
        for (size_t i = 0; i < lineBytes; i++) {
            if (data[offset + i] >= 32 && data[offset + i] <= 126) {
                printf("%c", data[offset + i]); // 可显示字符
            } else {
                printf("."); // 非可显示字符用 '.' 替代
            }
        }
        printf("|\n");

        offset += lineBytes; // 更新偏移量
    }
}