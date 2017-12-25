#ifndef _TEST_DATA_H_
#define _TEST_DATA_H_

int expected_fat[] = {-1, -1, 0, 4, 6, 7, 0, 8, 9, 0, -1, 0, -1};

bool expected_bool[] = {true, true, true, false};

file_iterator_t expected_it[] = {
	{
		.current_block = 2,
		.entry_offset_in_current_block = -64
	},
	{
		.current_block = 2,
		.entry_offset_in_current_block = 0
	},
	{
		.current_block = 2,
		.entry_offset_in_current_block = 64
	},
	{
		.current_block = 2,
		.entry_offset_in_current_block = 192
	}
};

char expected_name[][ENTRY_NAME_SIZE] = {
	"",
	"r.txt",
	"s.txt",
	"b.txt",
	"x.txt",
};

stat_t expected_st[] = {
	{
		.name = "",
		.size = 0,
		.used_blocks_nb = 0,
		.start = 0,
	},
	{
		.name = "r.txt",
		.size = 1169,
		.used_blocks_nb = 3,
		.start = 3,
	},
	{
		.name = "s.txt",
		.size = 1560,
		.used_blocks_nb = 4,
		.start = 5,
	},
	{
		.name = "b.txt",
		.size = 441,
		.used_blocks_nb = 1,
		.start = 11,
	},
};

int expected_st_return[] = {0, 0, 0, 0};

bool expected_return[] = {true, false};

int expected_fd[] = {0, 1, 2};

file_descriptor_t expected_fd_struct[] = {
	{
		.start_block = 3,
		.current_offset_in_block = 0,
		.current_block = 3,
		.file_size = 1169,
		.is_free = false,
		.readed_bytes = 0,
	},
	{
		.start_block = 3,
		.current_offset_in_block = 145,
		.current_block = 6,
		.file_size = 1169,
		.is_free = false,
		.readed_bytes = 1169,
	},
	{
		.start_block = 3,
		.current_offset_in_block = 505,
		.current_block = 4,
		.file_size = 1169,
		.is_free = false,
		.readed_bytes = 1017,
	},
	{
		.start_block = 3,
		.current_offset_in_block = 14,
		.current_block = 6,
		.file_size = 1169,
		.is_free = false,
		.readed_bytes = 1038,
	},
};

char expected_content1[] = {
	"1. 3 sectors 3 sectors 3 sectors 3 sectors 3 sectors\n"
	"2.  3 sectors 3 sectors 3 sectors 3 sectors 3 sectors\n"
	"3.   3 sectors 3 sectors 3 sectors 3 sectors 3 sectors\n"
	"4.    3 sectors 3 sectors 3 sectors 3 sectors 3 sectors\n"
	"5.     3 sectors 3 sectors 3 sectors 3 sectors 3 sectors\n"
	"6.      3 sectors 3 sectors 3 sectors 3 sectors 3 sectors\n"
	"7.       3 sectors 3 sectors 3 sectors 3 sectors 3 sectors\n"
	"8.        3 sectors 3 sectors 3 sectors 3 sectors 3 sectors\n"
	"9.         3 sectors 3 sectors 3 sectors 3 sectors 3 sectors\n"
	"10.         1xsectors\n"
	"11.          3 sectors 3 sectors 3 sectors 3 sectors 3 sectors\n"
	"12.           3 sectors 3 sectors 3 sectors 3 sectors 3 sectors\n"
	"13.            3 sectors 3 sectors 3 sectors 3 sectors 3 sectors\n"
	"14.             3 sectors 3 sectors 3 sectors 3 sectors 3 sectors\n"
	"15.              3 sectors 3 sectors 3 sectors 3 sectors 3 sectors\n"
	"16.               3 sectors 3 sectors 3 sectors 3 sectors 3 sectors\n"
	"17.                3 sectors 3 sectors 3 sectors 3 sectors 3 sectors\n"
	"18.                 3 sEctXrs H sectors 3 sectors 3 sectors 3 sectors\n"
	"19.                  3 sectors 3 sectors 3 sectors 3 sectors 3 sectors\n"
	"20.                   3 sectors"
};

char expected_content2[] = {"3 sEctXrs H sectors 3"};

int expected_len[] = {1169, 21};

#endif