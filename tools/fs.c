typedef struct dir_entry_st {
	uchar* name;
	uint size;
	unit start;
} __attribute__((packed)) dir_entry_t;

uint* fat;