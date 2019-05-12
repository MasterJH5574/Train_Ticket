#pragma warning(disable : 4996)
#include "exception.hpp"
#include <iostream>
#include <cstring>

#ifndef _USERS
#define _USERS

using namespace std;
class users {
private:
	struct user {
		int id, priv; // id && privilege, 2 for administrator, 1 for normal user
		char name[45], passwd[25], email[25], phone[25];
		//only for initial current_user
		user() {
			priv = 0;
		}
		user(const int _id, const char *_name, const char *_passwd, const char *_email, const char *_phone) {
			strcpy(name, _name);
			strcpy(passwd, _passwd);
			strcpy(email, _email);
			strcpy(phone, _phone);
			id = _id;
		}
		~user() {
			delete name;
			delete passwd;
			delete email;
			delete phone;
		}
	};

	const int origin_id = 2019;
	FILE *file;
	int sz;
	user *current_user;

	user *read_user(const int _id) {
		//if (!file) throw(sjtu::runtime_error());
		if (!file) return NULL;
		if (_id >= origin_id + sz) return NULL;
		fseek(file, sizeof(int) + (_id - origin_id) * sizeof(user), SEEK_SET);
		user *temp = new user();
		fread(temp, sizeof(user), 1, file);
		return temp;
	}
	
	void save_user(user *tempuser) {
		fseek(file, sizeof(int) + (tempuser->id - origin_id) * sizeof(user), SEEK_SET);
		fwrite(tempuser, sizeof(user), 1, file);
		fflush(file);
	}
	
	void save_info() {
		fseek(file, 0, SEEK_SET);
		fwrite(&sz, sizeof(int), 1, file);
		fflush(file);
	}
	
public:
	users() {
		file = fopen("user_data", "rb+");
		if (!file) {
			file = fopen("user_data", "wb+");
			fseek(file, 0, SEEK_SET);
			sz = 0;
			fwrite(&sz, sizeof(int), 1, file);
			fclose(file);
			file = fopen("user_data", "rb+");
		}
		else
			fread(&sz, sizeof(int), 1, file);
		current_user = new user();
	}
	~users() {
		fclose(file);
	}

	void delete_file() {
		cout << "1\n";
		sz = 0;
		save_info();
		remove("user_data");
	}

	int user_register(const char *_name, const char *_passwd, const char *_email, const char *_phone) {
		if (!file) return -1;
		++sz;
		save_info();
		user *tempuser = new user(origin_id + sz - 1, _name, _passwd, _email, _phone);
		if (sz == 1) {
			tempuser->priv = 2;
		}
		else tempuser->priv = 1;
		fseek(file, sizeof(int) + (sz - 1) * sizeof(user), SEEK_SET);
		fwrite(tempuser, sizeof(user), 1, file);
		fflush(file);
		return tempuser -> id;
	}

	bool user_login(const int _id, const char *_passwd) {
		user *tempuser = read_user(_id);
		if (tempuser == NULL) return false;
		if (strcmp(tempuser->passwd, _passwd) == 0) {
			current_user = tempuser;
			return true;
		}
		else return false;
	}

	void query_user(const int _id) {
		if (_id >= origin_id + sz) {
			cout << "0\n";
			return;
		}
		else {
			user *tempuser = read_user(_id);
			if (tempuser == NULL) {
				cout << "0\n";
				return;
			}
			cout << tempuser->name << " " << tempuser->email << " " << tempuser->phone << " " << tempuser->priv << "\n";
		}
	}

	bool modify_user(const int _id, const char *_name, const char *_passwd, const char *_email, const char *_phone) {
		if (_id < origin_id + sz) {
			user *tempuser = read_user(_id);
			strcpy(tempuser->name, _name);
			strcpy(tempuser->passwd, _passwd);
			strcpy(tempuser->email, _email);
			strcpy(tempuser->phone, _phone);
			save_user(tempuser);
			return 1;
		}
		else return 0;
	}

	bool modify_user_priv(const int _id1, const int _id2, const int _priv) {
		if (_priv <= 0 || _priv > 2) return false;
		user *user1 = read_user(_id1);
		if (user1 == NULL) return false;
		if (user1->priv != 2) return false;
		user *user2 = read_user(_id2);
		if (user2 == NULL) return false;
		if (user2->priv == 1) {
			if (_priv == 2) {
				user2->priv = _priv;
				save_user(user2);
			}
			return true;
		}
		else 
		if (user2->priv == 2) {
			if (_priv == 2) return true;
			else return false;
		}
	}

};

#endif
