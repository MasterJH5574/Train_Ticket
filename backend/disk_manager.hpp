#pragma warning(disable : 4996)

#include <iostream>
#include "exception.hpp"
#include<cassert>

#ifndef _DISK_MANAGER
#define _DISK_MANAGER

const off_t invalid_off = 0xdeadbeef;

class disk_manager {
private:
	const off_t node_size = 4096;
	int num;
	off_t file_end;
	off_t *hole_pos;
	char *indexname;
	FILE *file;

private:
	void save_info() {
		fseek(file, 0, SEEK_SET);
		fwrite(&num, sizeof(int), 1, file);
		fwrite(&file_end, sizeof(off_t), 1, file);
		if (num != 0) {
			for (int i = 0; i < num; ++i)
				fwrite(&hole_pos[i], sizeof(off_t), 1, file);
		}
		fflush(file);
		
	}

	void load_info() {
		fseek(file, 0, SEEK_SET);
		fread(&num, sizeof(int), 1, file);
		fread(&file_end, sizeof(off_t), 1, file);
		if (num) {
			hole_pos = new off_t[num + 1];
			for (int i = 0; i < num; ++i)
				fread(&hole_pos[i], sizeof(off_t), 1, file);
		}
		else hole_pos = nullptr;
	}

	void delete_temp() {
		delete hole_pos; hole_pos = nullptr;
		num = 0; file_end = invalid_off;
	}

public:
	disk_manager() {}

	disk_manager(const char *_indexname) {
		indexname = new char[strlen(_indexname) + 1];
		strcpy(indexname, _indexname);
		file = fopen(indexname, "rb+");
		if (!file) {
			file = fopen(indexname, "wb+");
			num = 0; file_end = 3 * sizeof(off_t);
			hole_pos = nullptr;
			save_info();
		}
		else hole_pos = nullptr;
	}

	~disk_manager() {
		if (indexname) delete indexname;
		if (file) fclose(file);
		if (hole_pos != nullptr) delete hole_pos;
	}

	void initialize(const char *_indexname) {
		indexname = new char[strlen(_indexname) + 1];
		strcpy(indexname, _indexname);
		file = fopen(indexname, "rb+");
		if (!file) {
			file = fopen(indexname, "wb+");
			num = 0; file_end = 3 * sizeof(off_t);
			hole_pos = nullptr;
			save_info();
		}
		else hole_pos = nullptr;
	}

	off_t new_pos() {
		load_info();
		if (num == 0) {
			off_t temp = file_end;
			file_end += node_size;
			save_info();
			return temp;
		}
		else {
			--num;
			save_info();
			return hole_pos[num];
		}
		delete_temp();
	}

	void delete_pos(off_t _del_pos) {
		load_info();
		if (num == 0) {
			num = 1;
			hole_pos = new off_t[1];
			hole_pos[0] = _del_pos;
		}
		else hole_pos[num++] = _del_pos;
		save_info();
		delete_temp();
	}
};

#endif