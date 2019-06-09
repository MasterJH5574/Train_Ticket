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
		user(const int _id, const char* _name, const char* _passwd, const char* _email, const char* _phone) {
			strcpy(name, _name);
			strcpy(passwd, _passwd);
			strcpy(email, _email);
			strcpy(phone, _phone);
			id = _id;
		}
		~user() {}
	};

	const int origin_id = 2019;
	FILE* file;
	int sz;

	user read_user(const int _id) {
		//if (!file) throw(sjtu::runtime_error());
		if (!file) return user();
		if (_id >= origin_id + sz) return user();
		fseek(file, (_id - origin_id) * sizeof(user) + sizeof(int) , SEEK_SET);
		user  temp;
		fread(&temp, sizeof(user), 1, file);
		return temp;
	}

	void save_user(user &tempuser) {
		fseek(file, (tempuser.id - origin_id) * sizeof(user) + sizeof(int) ,  SEEK_SET);
		fwrite(&tempuser, sizeof(user), 1, file);
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
	}
	~users() {
		fclose(file);
	}

	void delete_file() {
		cout << "1 ";
		sz = 0;
		save_info();
		remove("user_data");
	}

	int user_register(std::istream & is, std::ostream & os) {
		if (!file) return -1;
		int id, priv; // id && privilege, 2 for administrator, 1 for normal user
		++sz;
		save_info();
		user tempuser;
		tempuser.id = origin_id + sz - 1;
		is >> tempuser.name >> tempuser.passwd >> tempuser.email >> tempuser.phone;
		if (sz == 1) {
			tempuser.priv = 2;
		}
		else tempuser.priv = 1;
		fseek(file, (int) sizeof(int) + (sz - 1) * sizeof(user), SEEK_SET);
		fwrite(&tempuser, sizeof(user), 1, file);
		fflush(file);
		id = tempuser.id;
		return id;
	}

	bool user_login(std::istream & is, std::ostream & os) {
		int _id;
		char _passwd[45];
		is >> _id >> _passwd;
		user tempuser = read_user(_id);
		if (tempuser.priv == 0) return false;
		if (strcmp(tempuser.passwd, _passwd) == 0) return true;
		else return false;

	}

	void query_user(std::istream & is, std::ostream & os) {
		int _id;
		is >> _id;
		if (_id >= origin_id + sz) {
			os << "0 ";
			return;
		}
		else {
			user tempuser = read_user(_id);
			if (tempuser.priv == 0) {
				os << "0 ";
				return;
			}
			os << tempuser.name << " " << tempuser.email << " " << tempuser.phone << " " << tempuser.priv << " ";
			return;
		}
	}

    void query_password(std::istream & is, std::ostream & os) {
		int _id;
		is >> _id;
		if (_id >= origin_id + sz) {
			os << "-1";
			return;
		}
		else {
			user tempuser = read_user(_id);
			os << tempuser.passwd;
			return;
		}
	}
    
	bool modify_user(std::istream & is, std::ostream & os) {
		//, const int _id, const char *_name, const char *_passwd, const char *_email, const char *_phone
		int _id;
		is >> _id;
        user tempuser;
        if (_id < origin_id + sz) tempuser = read_user(_id);
        is >> tempuser.name >> tempuser.passwd >> tempuser.email >> tempuser.phone;
		if (_id < origin_id + sz) {
		    save_user(tempuser);
			return 1;
		}
		else
		    return 0;

	}

	bool modify_user_priv(std::istream & is, std::ostream & os) {
		int _id1, _id2, _priv;
		is >> _id1 >> _id2 >> _priv;

		if (_priv <= 0 || _priv > 2) return false;
		user  user1 = read_user(_id1);
		if (user1.priv == 0) return false;
		if (user1.priv != 2) return false;
		user  user2 = read_user(_id2);
		if (user2.priv == 0) return false;
		if (user2.priv == 1) {
			if (_priv == 2) {
				user2.priv = _priv;
				save_user(user2);
			}
			return true;
		}
		else
			if (user2.priv == 2) {
				if (_priv == 2)
				    return true;
				else
				    return false;

			}
	}

};

#endif
